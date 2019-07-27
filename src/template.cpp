// Copyright (C) 2019 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "liquid/template.h"

#include "liquid/parser.h"
#include "liquid/renderer.h"

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


TextNode::TextNode(const std::string& str)
  : text(str)
{

}

TextNode::TextNode(std::string&& str)
  : text(std::move(str))
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

Template::Template(const std::vector<std::shared_ptr<templates::Node>>& nodes)
  : mNodes(nodes)
{

}

Template::Template(std::vector<std::shared_ptr<templates::Node>>&& nodes)
  : mNodes(std::move(nodes))
{

}

Template::~Template()
{

}

std::string Template::render(const json::Object& data) const
{
  Renderer r;
  return r.render(*this, data);
}

Template parse(const std::string& str)
{
  liquid::Parser lp;
  return Template{ lp.parse(str) };
}

} // namespace liquid
