// Copyright (C) 2019 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef LIQUID_TEMPLATE_H
#define LIQUID_TEMPLATE_H

#include "liquid-defs.h"

#include <json-toolkit/json.h>

#include <memory>
#include <vector>

namespace liquid
{

namespace templates
{

class LIQUID_API Node
{
public:
  Node() = default;
  virtual ~Node() = default;

  template<typename T>
  bool is() const { return dynamic_cast<const T*>(this) != nullptr; }

  template<typename T>
  const T& as() const { return *dynamic_cast<const T*>(this); }

  template<typename T>
  T& as() { return *dynamic_cast<T*>(this); }

  virtual bool isText() const;
  virtual bool isTag() const;
  virtual bool isObject() const;
};

class LIQUID_API TextNode : public Node
{
public:
  TextNode(const String& str);
  TextNode(String&& str);
  ~TextNode() = default;

  bool isText() const override;

public:
  String text;
};

} // namespace templates

class LIQUID_API Template
{
public:
  Template();
  Template(const Template &) = default;
  ~Template();

  typedef templates::Node Node;

  Template(const std::vector<std::shared_ptr<templates::Node>>& nodes);
  Template(std::vector<std::shared_ptr<templates::Node>>&& nodes);

  const std::vector<std::shared_ptr<templates::Node>>& nodes() const { return mNodes; }

  String render(const json::Object& data) const;

  template<typename R>
  String render(const json::Object& data) const
  {
    R renderer;
    return renderer.render(*this, data);
  }

private:
  std::vector<std::shared_ptr<templates::Node>> mNodes;
};

LIQUID_API Template parse(const String& str);

} // namespace liquid

#endif // LIQUID_TEMPLATE_H
