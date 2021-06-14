// Copyright (C) 2019-2021 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "liquid/renderer.h"

#include "liquid/context.h"
#include "liquid/filters.h"

/*!
 * \namespace liquid
 */

namespace liquid
{

Renderer::Error::Error(size_t off, std::string mssg)
  : offset(off),
   message(std::move(mssg))
{

}

/*!
 * \class Renderer
 */

/*!
 * \fn Renderer()
 * \brief constructs a renderer
 */
Renderer::Renderer()
  : m_template(nullptr)
{

}

/*!
 * \fn ~Renderer()
 * \brief destroy the renderer
 */
Renderer::~Renderer()
{
  
}

/*!
 * \fn void reset()
 * \brief resets the renderer
 */
void Renderer::reset()
{
  m_result.clear();
  m_errors.clear();
  m_template = nullptr;
  context().scopes().clear();
  context().scopes().emplace_back();
  context().flags() = 0;
}

/*!
 * \fn Context& context()
 * \brief returns the renderer context
 */
Context& Renderer::context()
{
  return m_context;
}

/*!
 * \fn std::map<std::string, Template>& templates()
 * \brief returns the renderer built-in templates
 * 
 * These templates can be used with an 'include' tag.
 */
std::map<std::string, Template>& Renderer::templates()
{
  return m_templates;
}

/*!
 * \fn const std::map<std::string, Template>& templates() const
 * \brief returns the renderer built-in templates
 */
const std::map<std::string, Template>& Renderer::templates() const
{
  return m_templates;
}

/*!
 * \fn const std::vector<Renderer::Error>& errors() const
 * \brief returns the errors generated during the last call rendering
 */
const std::vector<Renderer::Error>& Renderer::errors() const
{
  return m_errors;
}

/*!
 * \fn const Template& model() const
 * \brief returns the template that is currently used
 * 
 * This function can only be called during rendering, i.e. inside a 
 * call of \c{render()}.
 */
const Template& Renderer::model() const
{
  assert(m_template != nullptr);
  return *m_template;
}

/*!
 * \fn std::string render(const Template& t, const liquid::Map& data)
 * \param the input template
 * \param the input data
 * \brief renders a template using the given data
 *
 * This function first resets the renderer.
 * 
 * Any error generated during rendering is written in the output.
 * You can check programmatically if any error occured with a call 
 * to \c{errors()}.
 */
std::string Renderer::render(const Template& t, const liquid::Map& data)
{
  reset();

  context().currentScope().data = data;

  m_template = &t;

  try
  {
    Context::Scope template_scope{ context(), t };

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

static std::string stringify_value(const liquid::Value& val);

static std::string stringify_map(const liquid::Map& map)
{
  std::set<std::string> names = map.propertyNames();

  if (names.empty())
    return "{}";

  std::string result;

  result.push_back('{');

  for (const auto& n : names)
  {
    result += "\"" + n +"\": " + stringify_value(map.property(n)) + ", ";
  }

  result.pop_back();
  result.pop_back();

  result.push_back('}');

  return result;
}

static std::string stringify_array(const liquid::Array& vec)
{
  std::string result;

  result.push_back('[');

  for (size_t i(0); i < vec.length(); ++i)
  {
    result += stringify_value(vec.at(i)) + ", ";
  }

  if (vec.length() > 0)
  {
    result.pop_back();
    result.pop_back();
  }

  result.push_back(']');

  return result;
}

static std::string stringify_value(const liquid::Value& val)
{
  if (val.is<std::string>())
    return "\"" + val.as<std::string>() + "\"";
  else if (val.is<bool>())
    return val.as<bool>() ? "true" : "false";
  else if (val.is<int>())
    return StringBackend::from_integer(val.as<int>());
  else if (val.is<double>())
    return StringBackend::from_number(val.as<double>());
  else if (val.isMap())
    return stringify_map(val.toMap());
  else if (val.isArray())
    return stringify_array(val.toArray());
  else
    return "";
}

std::string Renderer::defaultStringify(const liquid::Value& val)
{
  if (val.isNull())
    return {};

  if (val.is<std::string>())
    return val.as<std::string>();
  else if (val.is<bool>())
    return val.as<bool>() ? "true" : "false";
  else if (val.is<int>())
    return StringBackend::from_integer(val.as<int>());
  else if (val.is<double>())
    return StringBackend::from_number(val.as<double>());
  else if (val.isMap())
    return stringify_map(val.toMap());
  else if (val.isArray())
    return stringify_array(val.toArray());
  else
    return {};
}

std::string Renderer::stringify(const liquid::Value& val)
{
  return defaultStringify(val);
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

  if (ex.template_ && ex.offset_ < ex.template_->source().size())
  {
    if (ex.template_ != m_template && !ex.template_->filePath().empty())
    {
      std::pair<int, int> linecol = ex.template_->linecol(ex.offset_);
      write("{! " + ex.template_->filePath()  + ":" + std::to_string(linecol.first) + ":" + std::to_string(linecol.second) + ": " + ex.message_ + " !}");
    }
    else
    {
      std::pair<int, int> linecol = model().linecol(ex.offset_);
      write("{! " + std::to_string(linecol.first) + ":" + std::to_string(linecol.second) + ": " + ex.message_ + " !}");
    }
  }
  else
  {
    write("{! " + ex.message_ + " !}");
  }
}

std::string Renderer::capture(const Template& tmplt, const liquid::Map& data)
{
  Context::Scope capture_scope{ context(), tmplt, data };
  return capture(tmplt.nodes());
}

std::string Renderer::capture(const std::vector<std::shared_ptr<templates::Node>>& nodes)
{
  size_t offset = m_result.size();

  process(nodes);

  std::string captured{ m_result.begin() + offset, m_result.end() };
  m_result.resize(offset);

  return captured;
}

bool Renderer::evalCondition(const liquid::Value& val)
{
  return val.is<bool>() ? val.as<bool>() :
    (val.is<int>() ? val.as<int>() != 0 : !val.isNull());
}

liquid::Value Renderer::eval(const std::shared_ptr<Object>& obj)
{
  return obj->accept(*this);
}

std::vector<liquid::Value> Renderer::eval(const std::vector<std::shared_ptr<Object>>& objects)
{
  std::vector<liquid::Value> result;
  result.reserve(objects.size());

  for (auto obj : objects)
    result.push_back(eval(obj));

  return result;
}

liquid::Value Renderer::eval_value(const objects::Value& val)
{
  return val.value;
}

liquid::Value Renderer::eval_variable(const objects::Variable& var)
{
  for (int i = static_cast<int>(context().scopes().size()) - 1; i >= 0; --i)
  {
    const auto& data = context().scopes().at(i).data;
    
    liquid::Value val = data.property(var.name);

    if (!val.isNull())
      return val;
  }
  
  return nullptr;
}

liquid::Value Renderer::eval_memberaccess(const objects::MemberAccess& ma)
{
  const liquid::Value obj = eval(ma.object);

  if (obj.isArray())
  {
    if (ma.name == "size" || ma.name == "length")
      return liquid::Value(int(obj.length()));
    else
      return nullptr;
  }
  else if (obj.isMap())
  {
    return obj.property(ma.name);
  }
  else if (obj.is<std::string>())
  {
    if (ma.name == "size" || ma.name == "length")
      return static_cast<int>(obj.as<std::string>().size());
    else
      return nullptr;
  }
  else
  {
    throw EvaluationException{ "Value does not support member access", context().currentTemplate(), ma.object->offset() };
  }
}

liquid::Value Renderer::eval_arrayaccess(const objects::ArrayAccess & aa)
{
  const liquid::Value obj = eval(aa.object);
  const liquid::Value index = eval(aa.index);

  if (index.is<int>())
  {
    if (!obj.isArray())
      throw EvaluationException{ "Value is not an array", context().currentTemplate(),  aa.object->offset() };

    return obj.at(index.as<int>());
  }
  else if (index.is<std::string>())
  {
    if (!obj.isMap())
      throw EvaluationException{ "Value is not an object",  context().currentTemplate(), aa.object->offset() };

    return obj.property(index.as<std::string>());
  }
  else
  {
    throw EvaluationException{ "Index must be a 'string' or an 'int'",  context().currentTemplate(), aa.index->offset() };
  }
}

liquid::Value Renderer::eval_binop(const objects::BinOp & binop)
{
  switch (binop.operation)
  {
  case objects::BinOp::Or:
    return evalCondition(eval(binop.lhs)) || evalCondition(eval(binop.rhs));
  case objects::BinOp::And:
    return evalCondition(eval(binop.lhs)) && evalCondition(eval(binop.rhs));
  case objects::BinOp::Xor:
    return evalCondition(eval(binop.lhs)) ^ evalCondition(eval(binop.rhs));
  default:
    break;
  }

  const liquid::Value lhs = eval(binop.lhs);
  const liquid::Value rhs = eval(binop.rhs);

  switch (binop.operation)
  {
  case objects::BinOp::Equal:
    return liquid::compare(lhs, rhs) == 0;
  case objects::BinOp::Inequal:
    return liquid::compare(lhs, rhs) != 0;
  case objects::BinOp::Less:
    return liquid::compare(lhs, rhs) < 0;
  case objects::BinOp::Leq:
    return liquid::compare(lhs, rhs) <= 0;
  case objects::BinOp::Greater:
    return liquid::compare(lhs, rhs) > 0;
  case objects::BinOp::Geq:
    return liquid::compare(lhs, rhs) >= 0;
  case objects::BinOp::Add:
    return value_add(lhs, rhs);
  case objects::BinOp::Sub:
    return value_sub(lhs, rhs);
  case objects::BinOp::Mul:
    return value_mul(lhs, rhs);
  case objects::BinOp::Div:
    return value_div(lhs, rhs);
  default:
    break;
  }

  assert(false);
  return nullptr;
}

liquid::Value Renderer::eval_logicalnot(const objects::LogicalNot& op)
{
  return !evalCondition(eval(op.object));
}

liquid::Value Renderer::eval_pipe(const objects::Pipe & pipe)
{
  liquid::Value obj = eval(pipe.object);
  std::vector<liquid::Value> args = eval(pipe.arguments);

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

liquid::Value Renderer::value_add(const liquid::Value& lhs, const liquid::Value& rhs) const
{
  if (lhs.is<int>())
  {
    if (rhs.is<int>())
      return lhs.as<int>() + rhs.as<int>();
    else if (rhs.is<double>())
      return lhs.as<int>() + rhs.as<double>();
  }
  else if (lhs.is<double>())
  {
    if (rhs.is<int>())
      return lhs.as<double>() + rhs.as<int>();
    else if (rhs.is<double>())
      return lhs.as<double>() + rhs.as<double>();
  }
  else if (lhs.is<std::string>())
  {
    if (rhs.is<std::string>())
      return lhs.as<std::string>() + rhs.as<std::string>();
  }
  else if (lhs.isArray())
  {
    if (rhs.isArray())
      return ArrayFilters::concat(lhs.toArray(), rhs.toArray());
  }

  throw EvaluationException{ "operator + cannot proceed with given operands" };
}

liquid::Value Renderer::value_sub(const liquid::Value& lhs, const liquid::Value& rhs) const
{
  if (lhs.is<int>())
  {
    if (rhs.is<int>())
      return lhs.as<int>() - rhs.as<int>();
    else if (rhs.is<double>())
      return lhs.as<int>() - rhs.as<double>();
  }
  else if (lhs.is<double>())
  {
    if (rhs.is<int>())
      return lhs.as<double>() - rhs.as<int>();
    else if (rhs.is<double>())
      return lhs.as<double>() - rhs.as<double>();
  }

  throw EvaluationException{ "operator - cannot proceed with given operands" };
}

liquid::Value Renderer::value_mul(const liquid::Value& lhs, const liquid::Value& rhs) const
{
  if (lhs.is<int>())
  {
    if (rhs.is<int>())
      return lhs.as<int>() * rhs.as<int>();
    else if (rhs.is<double>())
      return lhs.as<int>() * rhs.as<double>();
  }
  else if (lhs.is<double>())
  {
    if (rhs.is<int>())
      return lhs.as<double>() * rhs.as<int>();
    else if (rhs.is<double>())
      return lhs.as<double>() * rhs.as<double>();
  }

  throw EvaluationException{ "operator * cannot proceed with given operands" };
}

liquid::Value Renderer::value_div(const liquid::Value& lhs, const liquid::Value& rhs) const
{
  if (lhs.is<int>())
  {
    if (rhs.is<int>())
      return lhs.as<int>() / rhs.as<int>();
    else if (rhs.is<double>())
      return lhs.as<int>() / rhs.as<double>();
  }
  else if (lhs.is<double>())
  {
    if (rhs.is<int>())
      return lhs.as<double>() / rhs.as<int>();
    else if (rhs.is<double>())
      return lhs.as<double>() / rhs.as<double>();
  }

  throw EvaluationException{ "operator / cannot proceed with given operands" };
}

liquid::Value Renderer::applyFilter(const std::string& name, const liquid::Value& object, const std::vector<liquid::Value>& args)
{
  return BuiltinFilters::apply(name, object, args);
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
  if (assign.global_scope)
  {
    context().scopes()[0].data.insert(assign.variable, eval(assign.value));
  }
  else if (assign.parent_scope)
  {
    context().parentFileScope().data.insert(assign.variable, eval(assign.value));
  }
  else
  {
    context().currentFileScope().data.insert(assign.variable, eval(assign.value));
  }
}

void Renderer::visitTag(const tags::Capture& tag)
{
  std::string captured = capture(tag.body);
  context().currentFileScope().data.insert(tag.variable, std::move(captured));
}

void Renderer::visitTag(const tags::For & tag)
{
  liquid::Value container = eval(tag.object);

  Context::Scope forloop{ context(), Context::ControlBlockScope };
  forloop["forloop"] = liquid::Map();

  if (container.isArray())
  {
    for (int i(0); i < container.length(); ++i)
    {
      forloop[tag.variable] = container.at(i);

      forloop["forloop"].toMap()["index"] = i;
      forloop["forloop"].toMap()["first"] = (i == 0);
      forloop["forloop"].toMap()["last"] = (i == container.length() - 1);

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
    throw EvaluationException{ "No template named '" + tag.name + "'", context().currentTemplate(), tag.offset() };
  }

  const Template& tmplt = it->second;

  Context::Scope include_scope{ context(), tmplt };
  include_scope["include"] = liquid::Map();
  include_scope["include"].toMap()["__"] = true;

  for (const auto& e : tag.objects)
  {
    const std::string& var_name = e.first;
    liquid::Value var_value = eval(e.second);
    include_scope["include"].toMap()[var_name] = var_value;
  }

  process(tmplt.nodes());
}

void Renderer::visitTag(const tags::Newline&)
{
  m_result.push_back('\n');
}

liquid::Value Renderer::visitObject(const objects::Value& val)
{
  return eval_value(val);
}

liquid::Value Renderer::visitObject(const objects::Variable& var)
{
  return eval_variable(var);
}

liquid::Value Renderer::visitObject(const objects::MemberAccess& ma)
{
  return eval_memberaccess(ma);
}

liquid::Value Renderer::visitObject(const objects::ArrayAccess& aa)
{
  return eval_arrayaccess(aa);
}

liquid::Value Renderer::visitObject(const objects::BinOp& binop)
{
  return eval_binop(binop);
}

liquid::Value Renderer::visitObject(const objects::LogicalNot& obj)
{
  return eval_logicalnot(obj);
}

liquid::Value Renderer::visitObject(const objects::Pipe& pipe)
{
  return eval_pipe(pipe);
}

/*!
 * \endclass
 */

/*!
 * \endnamespace
 */

} // namespace liquid
