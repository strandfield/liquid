// Copyright (C) 2019 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef LIQUID_RENDERER_H
#define LIQUID_RENDERER_H

#include "liquid/errors.h"
#include "liquid/context.h"
#include "liquid/objects.h"
#include "liquid/tags.h"

#include <map>

namespace liquid
{

class LIQUID_API Renderer
{
public:
  Renderer();
  ~Renderer();

  void reset();

  Context& context();

  std::map<std::string, Template>& templates();
  const std::map<std::string, Template>& templates() const;

  std::string render(const Template& t, const json::Object& data);

  json::Json eval(const std::shared_ptr<Object>& obj);
  void process(const std::shared_ptr<Template::Node>& node);
  void process(const std::vector<std::shared_ptr<Template::Node>>& nodes);
  virtual std::string stringify(const json::Json& val);

  struct Error
  {
    size_t offset;
    std::string message;

    Error(const Error&) = default;
    Error(Error&&) noexcept = default;

    Error(size_t off, std::string mssg);

    Error& operator=(Error&&) noexcept = default;
    Error& operator=(const Error&) = default;
  };

  const std::vector<Error>& errors() const;

  static bool evalCondition(const json::Json& val);

  /* Tags */
  void visitTag(const tags::Assign& tag);
  void visitTag(const tags::For& tag);
  void visitTag(const tags::If& tag);
  void visitTag(const tags::Break& tag);
  void visitTag(const tags::Continue& tag);
  void visitTag(const tags::Eject& tag);
  void visitTag(const tags::Discard& tag);
  void visitTag(const tags::Include& tag);

  /* Objects */
  json::Json visitObject(const objects::Value& val);
  json::Json visitObject(const objects::Variable& var);
  json::Json visitObject(const objects::MemberAccess& ma);
  json::Json visitObject(const objects::ArrayAccess& aa);
  json::Json visitObject(const objects::BinOp& binop);
  json::Json visitObject(const objects::Pipe& pipe);

protected:
  const Template& model() const;

  void write(const std::string& str);

  void record(const EvaluationException& ex);
  virtual void log(const EvaluationException& ex);

  /* Objects */
  json::Json eval_value(const objects::Value& val);
  json::Json eval_variable(const objects::Variable& var);
  json::Json eval_memberaccess(const objects::MemberAccess& ma);
  json::Json eval_arrayaccess(const objects::ArrayAccess& aa);
  json::Json eval_binop(const objects::BinOp& binop);
  json::Json eval_pipe(const objects::Pipe& pipe);

  virtual json::Json applyFilter(const std::string& name, const json::Json& object, const std::vector<json::Json>& args);

private:
  Context m_context;
  const Template* m_template;
  std::string m_result;
  std::vector<Error> m_errors;
  std::map<std::string, Template> m_templates;
};

} // namespace liquid

#endif // LIQUID_RENDERER_H
