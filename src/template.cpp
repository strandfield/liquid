// Copyright (C) 2019 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "liquid/template.h"

#include "liquid/parser.h"
#include "liquid/renderer.h"

#include <fstream>
#include <sstream>

namespace liquid
{

namespace templates
{

bool Node::isText() const
{
  return false;
}

bool Node::isTag() const
{
  return false;
}

bool Node::isObject() const
{
  return false;
}

TextNode::TextNode(std::string str, size_t off)
  : Node(off),
    text(std::move(str))
{

}

bool TextNode::isText() const
{
  return true;
}

} // namespace templates

Template::Template()
{

}

Template::Template(std::string src, std::vector<std::shared_ptr<templates::Node>> nodes, std::string filepath)
  : mFilePath(std::move(filepath)),
    mSource(std::move(src)),
    mNodes(std::move(nodes))
{

}

Template::~Template()
{

}

const std::string& Template::filePath() const
{
  return mFilePath;
}

const std::string& Template::source() const
{
  return mSource;
}

std::string Template::render(const json::Object& data) const
{
  Renderer r;
  return r.render(*this, data);
}

std::pair<int, int> Template::linecol(size_t off) const
{
  int line = 0;

  for (size_t i(0); i < off; ++i)
  {
    line += source().at(i) == '\n' ? 1 : 0;
  }

  int col = 0;

  while (off > 0 && source().at(off - 1) != '\n')
  {
    --off;
    ++col;
  }

  return { line, col };
}

std::string Template::getLine(size_t off) const
{
  size_t begin = off;

  while (begin > 0 && source().at(begin-1) != '\n') --begin;

  size_t end = off;

  while (end < source().size() && source().at(end) != '\n') ++end;

  return std::string(source().begin() + begin, source().begin() + end);
}

Template parse(const std::string& str, std::string filepath)
{
  liquid::Parser lp;
  return Template{ str, lp.parse(str), filepath };
}

Template parseFile(std::string filepath)
{
  std::ifstream file{ filepath };
  std::stringstream buffer;
  buffer << file.rdbuf();
  return liquid::parse(buffer.str(), std::move(filepath));
}

} // namespace liquid
