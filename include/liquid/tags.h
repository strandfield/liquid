// Copyright (C) 2019-2021 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef LIQUID_TAGS_H
#define LIQUID_TAGS_H

#include "liquid/tag.h"

#include "liquid/object.h"

namespace liquid
{

namespace tags
{

class Comment : public Tag
{
public:
  Comment();
  ~Comment() = default;

  void accept(Renderer& r);
};

class Assign : public Tag
{
public:
  Assign(const std::string& varname, const std::shared_ptr<Object> & expr, size_t off = std::numeric_limits<size_t>::max());
  ~Assign() = default;

  void accept(Renderer& r);

public:
  std::string variable;
  std::shared_ptr<Object> value;
  bool parent_scope = false;
  bool global_scope = false;
};

class Capture : public Tag
{
public:
  explicit Capture(const std::string& varname, size_t off = std::numeric_limits<size_t>::max());
  ~Capture() = default;

  void accept(Renderer& r);

public:
  std::string variable;
  std::vector<std::shared_ptr<templates::Node>> body;
};

class For : public Tag
{
public:
  For(const std::string& varname, const std::shared_ptr<Object> & expr, size_t off = std::numeric_limits<size_t>::max());
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
  explicit Break(size_t off = std::numeric_limits<size_t>::max());
  ~Break() = default;

  void accept(Renderer& r);
};

class Continue : public Tag
{
public:
  explicit Continue(size_t off = std::numeric_limits<size_t>::max());
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

  If(std::shared_ptr<Object> cond, size_t off = std::numeric_limits<size_t>::max());
  ~If() = default;

  void accept(Renderer& r);

public:
  std::vector<Block> blocks;
};

class Eject : public Tag
{
public:
  Eject();
  ~Eject() = default;

  void accept(Renderer& r);
};

class Discard : public Tag
{
public:
  Discard();
  ~Discard() = default;

  void accept(Renderer& r);
};

class Include : public Tag
{
public:
  explicit Include(std::string n);
  ~Include() = default;

  void accept(Renderer& r);

public:
  std::string name;
  std::map<std::string, std::shared_ptr<Object>> objects;
};

class Newline : public Tag
{
public:
  explicit Newline(size_t off = std::numeric_limits<size_t>::max());
  ~Newline() = default;

  void accept(Renderer& r);
};

} // tags

} // namespace liquid

#endif // LIQUID_TAGS_H
