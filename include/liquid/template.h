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
  virtual ~Node() = default;

  Node(size_t off = std::numeric_limits<size_t>::max()) : m_offset(off) { }

  template<typename T>
  bool is() const { return dynamic_cast<const T*>(this) != nullptr; }

  template<typename T>
  const T& as() const { return *dynamic_cast<const T*>(this); }

  template<typename T>
  T& as() { return *dynamic_cast<T*>(this); }

  virtual bool isText() const;
  virtual bool isTag() const;
  virtual bool isObject() const;

  size_t offset() const { return m_offset; }

private:
  size_t m_offset;
};

class LIQUID_API TextNode : public Node
{
public:
  TextNode(std::string str, size_t off = std::numeric_limits<size_t>::max());
  ~TextNode() = default;

  bool isText() const override;

public:
  std::string text;
};

} // namespace templates

class LIQUID_API Template
{
public:
  Template();
  Template(const Template &) = default;
  Template(Template&&) noexcept = default;
  ~Template();

  typedef templates::Node Node;

  Template(std::string src, std::vector<std::shared_ptr<templates::Node>> nodes, std::string filepath = {});

  const std::string& filePath() const;
  const std::string& source() const;
  const std::vector<std::shared_ptr<templates::Node>>& nodes() const { return mNodes; }

  std::string render(const json::Object& data) const;

  template<typename R>
  std::string render(const json::Object& data) const
  {
    R renderer;
    return renderer.render(*this, data);
  }

  Template& operator=(const Template&) = default;
  Template& operator=(Template&&) noexcept = default;

private:
  std::string mFilePath;
  std::string mSource;
  std::vector<std::shared_ptr<templates::Node>> mNodes;
};

LIQUID_API Template parse(const std::string& str, std::string filepath = {});
LIQUID_API Template parseFile(std::string filepath);

} // namespace liquid

#endif // LIQUID_TEMPLATE_H
