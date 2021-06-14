// Copyright (C) 2019-2021 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef LIQUID_TEMPLATE_H
#define LIQUID_TEMPLATE_H

#include "liquid-defs.h"

#include "liquid/value.h"

#include <limits>
#include <memory>
#include <utility>
#include <vector>

/*!
 * \namespace liquid
 */

namespace liquid
{

namespace templates
{

class LIQUID_API Node
{
public:
  virtual ~Node() = default;

  explicit Node(size_t off = std::numeric_limits<size_t>::max()) : m_offset(off) { }

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
  void setOffset(size_t off) { m_offset = off;  }

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

/*!
 * \class Template
 * \brief provides a render template
 * 
 * A template consists of a list of nodes, each node being of the 3 following types:
 * \begin{list}
 *   \li text
 *   \li tag (\{\% \%\})
 *   \li object (\{\{ \}\})
 * \end{list}
 * 
 * Text nodes are copied verbatim to the output.
 * 
 * Tag nodes represents instructions and allow control flow to alter the output.
 * 
 * Object nodes are variables are expressions that are ultimately converted to 
 * a string and inserted into the output string.
 * 
 * If you use the built-in parser to create a Template, the following tags are 
 * supported:
 * \begin{list}
 *   \li assign
 *   \li capture
 *   \li for
 *   \li if
 *   \li break
 *   \li continue
 *   \li eject
 *   \li discard
 *   \li include
 *   \li newline
 * \end{list}
 * 
 */
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

  std::string render(const liquid::Map& data) const;

  template<typename R>
  std::string render(const liquid::Map& data) const
  {
    R renderer;
    return renderer.render(*this, data);
  }

  std::pair<int, int> linecol(size_t off) const;
  std::string getLine(size_t off) const;

  static void lstrip(std::string& str) noexcept;
  static void rstrip(std::string& str) noexcept;

  void stripWhitespacesAtTag();
  void skipWhitespacesAfterTag();

  Template& operator=(const Template&) = default;
  Template& operator=(Template&&) noexcept = default;

private:
  std::string mFilePath;
  std::string mSource;
  std::vector<std::shared_ptr<templates::Node>> mNodes;
};

/*!
 * \endclass
 */

LIQUID_API Template parse(const std::string& str, std::string filepath = {});
LIQUID_API Template parseFile(std::string filepath);

/*!
 * \endnamespace
 */

} // namespace liquid

#endif // LIQUID_TEMPLATE_H
