// Copyright (C) 2019 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef LIQUID_RENDERER_H
#define LIQUID_RENDERER_H

#include "liquid/context.h"
#include "liquid/objects.h"
#include "liquid/tags.h"

namespace liquid
{

class LIQUID_API Renderer
{
public:
  Renderer();
  ~Renderer();

  void reset();

  Context& context();

  std::string render(const Template& t, const json::Object& data);

  void setStripWhitespacesAtTag(bool on = true);
  bool stripWhiteSpacesAtTag() const;

  json::Json eval(const std::shared_ptr<Object>& obj);
  void process(const std::shared_ptr<Template::Node>& node);
  void process(const std::vector<std::shared_ptr<Template::Node>>& nodes);
  virtual std::string stringify(const json::Json& val);

  static void lstrip(std::string& str);
  static void rstrip(std::string& str);

  static bool evalCondition(const json::Json& val);

  /* Tags */
  virtual void visitTag(const Tag& tag);
  void visitTag(const tags::Assign& tag);
  void visitTag(const tags::For& tag);
  void visitTag(const tags::If& tag);
  void visitTag(const tags::Break& tag);
  void visitTag(const tags::Continue& tag);

protected:
  void write(const std::string& str);

  /* Objects */
  json::Json eval_value(const objects::Value& val);
  json::Json eval_variable(const objects::Variable& var);
  json::Json eval_memberaccess(const objects::MemberAccess& ma);
  json::Json eval_arrayaccess(const objects::ArrayAccess& aa);
  json::Json eval_binop(const objects::BinOp& binop);
  json::Json eval_pipe(const objects::Pipe& pipe);

  virtual json::Json applyFilter(const std::string& name, const json::Json& object, const std::vector<json::Json>& args);

private:
  bool m_strip_whitespace_at_tag;
  std::shared_ptr<Template::Node> m_last_processed_node;
  Context m_context;
  std::string m_result;
};

} // namespace liquid

#endif // LIQUID_RENDERER_H
