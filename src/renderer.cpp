// Copyright (C) 2019 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "liquid/renderer.h"

#include "liquid/context.h"

#include <json-toolkit/stringify.h>

namespace liquid
{

EvaluationException::EvaluationException(const std::string& mssg, size_t off)
  : offset_(off),
    message_(mssg)
{

}

const char* EvaluationException::what() const noexcept
{
  return "liquid::Renderer evaluation error";
}

Renderer::Error::Error(size_t off, std::string mssg)
  : offset(off),
   message(std::move(mssg))
{

}

Renderer::Renderer()
  : m_template(nullptr)
{


}

Renderer::~Renderer()
{
  
}

void Renderer::reset()
{
  m_result.clear();
  m_errors.clear();
  m_template = nullptr;
  context().scopes().clear();
  context().scopes().emplace_back();
  context().flags() = 0;
}

Context& Renderer::context()
{
  return m_context;
}

std::map<std::string, Template>& Renderer::templates()
{
  return m_templates;
}

const std::map<std::string, Template>& Renderer::templates() const
{
  return m_templates;
}

const std::vector<Renderer::Error>& Renderer::errors() const
{
  return m_errors;
}

const Template& Renderer::model() const
{
  assert(m_template != nullptr);
  return *m_template;
}

std::string Renderer::render(const Template& t, const json::Object& data)
{
  reset();

  m_template = &t;
  context().currentScope().data = data;

  try
  {
    for (auto n : t.nodes())
    {
      process(n);

      if (context().flags() & Context::Eject)
        break;
    }
  }
  catch (const EvaluationException& ex)
  {
    log(ex);
  }

  m_template = nullptr;

  if (context().flags() & Context::Eject)
  {
    if (context().flags() == Context::Discard)
      m_result.clear();

    context().flags() = 0;
  }

  return m_result;
}

void Renderer::process(const std::shared_ptr<Template::Node>& n)
{
  if (n->isText())
  {
    write(n->as<templates::TextNode>().text);
  }
  else if (n->isObject())
  {
    write(stringify(eval(std::static_pointer_cast<Object>(n))));
  }
  else if (n->isTag())
  {
    static_cast<Tag*>(n.get())->accept(*this);
  }
}

std::string Renderer::stringify(const json::Json & val)
{
  if (val.isNull())
    return {};

  if (val.isString())
    return val.toString();
  else if (val.isInteger())
    return StringBackend::from_integer(val.toInt());
  else if (val.isNumber())
    return StringBackend::from_number(val.toNumber());
  else
    return json::stringify(val);
}

void Renderer::write(const std::string& str)
{
  m_result += str;
}

void Renderer::record(const EvaluationException& ex)
{
  m_errors.emplace_back(ex.offset_, ex.message_);
}

void Renderer::log(const EvaluationException& ex)
{
  record(ex);

  std::pair<int, int> linecol = model().linecol(ex.offset_);
  write("{! " + std::to_string(linecol.first) + ":" + std::to_string(linecol.second) + ": " + ex.message_ + " !}");
}

bool Renderer::evalCondition(const json::Json& val)
{
  return val.isBoolean() ? val.toBool() :
    (val.isInteger() ? val.toInt() != 0 : !val.isNull());
}

json::Json Renderer::eval(const std::shared_ptr<Object>& obj)
{
  return obj->accept(*this);
}

json::Json Renderer::eval_value(const objects::Value& val)
{
  return val.value;
}

json::Json Renderer::eval_variable(const objects::Variable& var)
{
  for (int i = static_cast<int>(context().scopes().size()) - 1; i >= 0; --i)
  {
    const json::Object data = context().scopes().at(i).data;
    
    json::Json val = data[var.name];

    if (val != nullptr)
      return val;
  }
  
  return nullptr;
}

json::Json Renderer::eval_memberaccess(const objects::MemberAccess& ma)
{
  const json::Json obj = eval(ma.object);

  if (obj.isArray())
  {
    if (ma.name == "size" || ma.name == "length")
      return json::Json(obj.length());
    else
      return nullptr;
  }
  else if (obj.isObject())
  {
    return obj[ma.name];
  }
  else if (obj.isString())
  {
    if (ma.name == "size" || ma.name == "length")
      return json::Json(static_cast<int>(obj.toString().size()));
    else
      return nullptr;
  }
  else
  {
    throw EvaluationException{ "Value does not support member access", ma.object->offset() };
  }
}

json::Json Renderer::eval_arrayaccess(const objects::ArrayAccess & aa)
{
  const json::Json obj = eval(aa.object);
  const json::Json index = eval(aa.index);

  if (index.isInteger())
  {
    if (!obj.isArray())
      throw EvaluationException{ "Value is not an array", aa.object->offset() };

    return obj.at(index.toInt());
  }
  else if (index.isString())
  {
    if (!obj.isObject())
      throw EvaluationException{ "Value is not an object", aa.object->offset() };

    return obj[index.toString()];
  }
  else
  {
    throw EvaluationException{ "Index must be a 'string' or an 'int'", aa.index->offset() };
  }
}

json::Json Renderer::eval_binop(const objects::BinOp & binop)
{
  switch (binop.operation)
  {
  case objects::BinOp::Or:
    return evalCondition(eval(binop.lhs)) || evalCondition(eval(binop.rhs));
  case objects::BinOp::And:
    return evalCondition(eval(binop.lhs)) && evalCondition(eval(binop.rhs));
  default:
    break;
  }

  const json::Json lhs = eval(binop.lhs);
  const json::Json rhs = eval(binop.rhs);

  switch (binop.operation)
  {
  case objects::BinOp::Equal:
    return lhs == rhs;
  case objects::BinOp::Inequal:
    return lhs != rhs;
  case objects::BinOp::Less:
    return json::compare(lhs, rhs) < 0;
  case objects::BinOp::Leq:
    return json::compare(lhs, rhs) <= 0;
  case objects::BinOp::Greater:
    return json::compare(lhs, rhs) > 0;
  case objects::BinOp::Geq:
    return json::compare(lhs, rhs) >= 0;
  default:
    break;
  }

  assert(false);
  return nullptr;
}

json::Json Renderer::eval_pipe(const objects::Pipe & pipe)
{
  json::Json obj = eval(pipe.object);
  std::vector<json::Json> args = pipe.arguments;

  try
  {
    return applyFilter(pipe.filterName, obj, args);
  }
  catch (EvaluationException& ex)
  {
    ex.offset_ = pipe.offset();
    throw;
  }
}

json::Json Renderer::applyFilter(const std::string& name, const json::Json& object, const std::vector<json::Json>& args)
{
  throw EvaluationException{ "Invalid filter name '" + name + "'" };
}

void Renderer::process(const std::vector<std::shared_ptr<Template::Node>>& nodes)
{
  for (const auto & n : nodes)
  {
    process(n);

    if (context().flags() != 0)
      return;
  }
}

void Renderer::visitTag(const tags::Assign & assign)
{
  context().currentScope().data[assign.variable] = eval(assign.value);
}

void Renderer::visitTag(const tags::For & tag)
{
  json::Json container = eval(tag.object);

  Context::Scope forloop{ context() };

  if (container.isArray())
  {
    for (int i(0); i < container.length(); ++i)
    {
      forloop[tag.variable] = container.at(i);

      forloop["forloop"]["index"] = i;
      forloop["forloop"]["first"] = (i == 0);
      forloop["forloop"]["last"] = (i == container.length() - 1);

      process(tag.body);

      if (context().flags() & (Context::Continue | Context::Break))
      {
        int rflags = context().flags();
        context().flags() = 0;

        if (rflags & Context::Break)
          return;
      }
      else if (context().flags() & Context::Eject)
      {
        return;
      }
    }
  }
  else
  {
    /// TODO:
  }
}

void Renderer::visitTag(const tags::If & tag)
{
  for (size_t i(0); i < tag.blocks.size(); ++i)
  {
    const auto& b = tag.blocks.at(i);

    if (evalCondition(eval(b.condition)))
    {
      process(b.body);
      return;
    }
  }
}

void Renderer::visitTag(const tags::Break & tag)
{
  context().flags() |= Context::Break;
}

void Renderer::visitTag(const tags::Continue & tag)
{
  context().flags() |= Context::Continue;
}

void Renderer::visitTag(const tags::Eject& tag)
{
  context().flags() |= Context::Eject;
}

void Renderer::visitTag(const tags::Discard& tag)
{
  context().flags() |= Context::Discard;
}

void Renderer::visitTag(const tags::Include& tag)
{
  auto it = templates().find(tag.name);

  if (it == templates().end())
  {
    throw EvaluationException{ "No template named '" + tag.name + "'" };
  }

  const Template& tmplt = it->second;

  Context::Scope include_scope{ context() };
  include_scope["include"]["__"] = true;

  for (const auto& e : tag.objects)
  {
    const std::string& var_name = e.first;
    json::Json var_value = eval(e.second);
    include_scope["include"][var_name] = var_value;
  }

  process(tmplt.nodes());
}

json::Json Renderer::visitObject(const objects::Value& val)
{
  return eval_value(val);
}

json::Json Renderer::visitObject(const objects::Variable& var)
{
  return eval_variable(var);
}

json::Json Renderer::visitObject(const objects::MemberAccess& ma)
{
  return eval_memberaccess(ma);
}

json::Json Renderer::visitObject(const objects::ArrayAccess& aa)
{
  return eval_arrayaccess(aa);
}

json::Json Renderer::visitObject(const objects::BinOp& binop)
{
  return eval_binop(binop);
}

json::Json Renderer::visitObject(const objects::Pipe& pipe)
{
  return eval_pipe(pipe);
}

} // namespace liquid
