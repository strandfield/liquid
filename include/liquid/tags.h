// Copyright (C) 2019 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef LIQUID_TAGS_H
#define LIQUID_TAGS_H

#include "liquid/tag.h"

#include "liquid/object.h"

#include <json-toolkit/json.h>

namespace liquid
{

namespace tags
{

class Assign : public Tag
{
public:
  Assign(const std::string& varname, const std::shared_ptr<Object> & expr);
  ~Assign() = default;

  void accept(Renderer& r);

public:
  std::string variable;
  std::shared_ptr<Object> value;
};

class For : public Tag
{
public:
  For(const std::string& varname, const std::shared_ptr<Object> & expr);
  ~For() = default;

  void accept(Renderer& r);

public:
  std::string variable;
  std::shared_ptr<Object> object;
  std::vector<std::shared_ptr<templates::Node>> body;
};

class Break : public Tag
{
public:
  Break();
  ~Break() = default;

  void accept(Renderer& r);
};

class Continue : public Tag
{
public:
  Continue();
  ~Continue() = default;

  void accept(Renderer& r);
};

class If : public Tag
{
public:
  struct Block
  {
    std::shared_ptr<Object> condition;
    std::vector<std::shared_ptr<templates::Node>> body;
  };

  If(std::shared_ptr<Object> cond);
  ~If() = default;

  void accept(Renderer& r);

public:
  std::vector<Block> blocks;
};

} // tags

} // namespace liquid

#endif // LIQUID_TAGS_H
