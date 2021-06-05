// Copyright (C) 2019-2021 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef LIQUID_RENDERER_H
#define LIQUID_RENDERER_H

#include "liquid/errors.h"
#include "liquid/context.h"
#include "liquid/objects.h"
#include "liquid/tags.h"

#include <map>

/*!
 * \namespace liquid
 */

namespace liquid
{

/*!
 * \class Renderer
 * \brief base class for renderers
 */
class LIQUID_API Renderer
{
public:
  Renderer();
  ~Renderer();

  void reset();

  Context& context();

  std::map<std::string, Template>& templates();
  const std::map<std::string, Template>& templates() const;

  std::string render(const Template& t, const liquid::Map& data);

  liquid::Value eval(const std::shared_ptr<Object>& obj);
  std::vector<liquid::Value> eval(const std::vector<std::shared_ptr<Object>>& objects);

  void process(const std::shared_ptr<Template::Node>& node);
  void process(const std::vector<std::shared_ptr<Template::Node>>& nodes);

  static std::string defaultStringify(const liquid::Value& val);
  virtual std::string stringify(const liquid::Value& val);

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

  static bool evalCondition(const liquid::Value& val);

  /* Tags */
  void visitTag(const tags::Assign& tag);
  void visitTag(const tags::Capture& tag);
  void visitTag(const tags::For& tag);
  void visitTag(const tags::If& tag);
  void visitTag(const tags::Break& tag);
  void visitTag(const tags::Continue& tag);
  void visitTag(const tags::Eject& tag);
  void visitTag(const tags::Discard& tag);
  void visitTag(const tags::Include& tag);
  void visitTag(const tags::Newline& tag);

  /* Objects */
  liquid::Value visitObject(const objects::Value& val);
  liquid::Value visitObject(const objects::Variable& var);
  liquid::Value visitObject(const objects::MemberAccess& ma);
  liquid::Value visitObject(const objects::ArrayAccess& aa);
  liquid::Value visitObject(const objects::BinOp& binop);
  liquid::Value visitObject(const objects::LogicalNot& obj);
  liquid::Value visitObject(const objects::Pipe& pipe);

protected:
  const Template& model() const;

  void write(const std::string& str);

  void record(const EvaluationException& ex);
  virtual void log(const EvaluationException& ex);

  std::string capture(const Template& tmplt, const liquid::Map& data);
  std::string capture(const std::vector<std::shared_ptr<templates::Node>>& nodes);

  /* Objects */
  liquid::Value eval_value(const objects::Value& val);
  liquid::Value eval_variable(const objects::Variable& var);
  liquid::Value eval_memberaccess(const objects::MemberAccess& ma);
  liquid::Value eval_arrayaccess(const objects::ArrayAccess& aa);
  liquid::Value eval_binop(const objects::BinOp& binop);
  liquid::Value eval_logicalnot(const objects::LogicalNot& op);
  liquid::Value eval_pipe(const objects::Pipe& pipe);

  liquid::Value value_add(const liquid::Value& lhs, const liquid::Value& rhs) const;
  liquid::Value value_sub(const liquid::Value& lhs, const liquid::Value& rhs) const;
  liquid::Value value_mul(const liquid::Value& lhs, const liquid::Value& rhs) const;
  liquid::Value value_div(const liquid::Value& lhs, const liquid::Value& rhs) const;

  virtual liquid::Value applyFilter(const std::string& name, const liquid::Value& object, const std::vector<liquid::Value>& args);

private:
  Context m_context;
  const Template* m_template;
  std::string m_result;
  std::vector<Error> m_errors;
  std::map<std::string, Template> m_templates;
};

/*!
 * \endclass
 */

/*!
 * \endnamespace
 */

} // namespace liquid

#endif // LIQUID_RENDERER_H
