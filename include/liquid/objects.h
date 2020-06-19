// Copyright (C) 2019 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef LIQUID_OBJECTS_H
#define LIQUID_OBJECTS_H

#include "liquid/object.h"

#include <json-toolkit/json.h>

namespace liquid
{

namespace objects
{

class Value : public Object
{
public:
  Value(const json::Json& val, size_t off = std::numeric_limits<size_t>::max());
  ~Value() = default;

  json::Json accept(Renderer& r) override;

public:
  json::Json value;
};

class Variable : public Object
{
public:
  Variable(std::string n, size_t off = std::numeric_limits<size_t>::max());
  ~Variable() = default;

  json::Json accept(Renderer& r) override;

public:
  std::string name;
};

class ArrayAccess : public Object
{
public:
  ArrayAccess(const std::shared_ptr<Object>& obj, const std::shared_ptr<Object>& ind, size_t off = std::numeric_limits<size_t>::max());
  ~ArrayAccess() = default;

  json::Json accept(Renderer& r) override;

public:
  std::shared_ptr<Object> object;
  std::shared_ptr<Object> index;
};

class MemberAccess : public Object
{
public:
  MemberAccess(const std::shared_ptr<Object>& obj, const std::string& name, size_t off = std::numeric_limits<size_t>::max());
  ~MemberAccess() = default;

  json::Json accept(Renderer& r) override;

public:
  std::shared_ptr<Object> object;
  std::string name;
};

class BinOp : public Object
{
public:
  enum Operation {
    Less,
    Leq,
    Greater,
    Geq,
    Equal,
    Inequal,
    And,
    Or,
    Xor,
  };

  BinOp(Operation op, const std::shared_ptr<Object>& left, const std::shared_ptr<Object>& right, size_t off = std::numeric_limits<size_t>::max());
  ~BinOp() = default;

  json::Json accept(Renderer& r) override;

public:
  Operation operation;
  std::shared_ptr<Object> lhs;
  std::shared_ptr<Object> rhs;
};

class LogicalNot : public Object
{
public:
  LogicalNot(const std::shared_ptr<Object>& obj, size_t off = std::numeric_limits<size_t>::max());
  ~LogicalNot() = default;

  json::Json accept(Renderer& r) override;

public:
  std::shared_ptr<Object> object;
};

class Pipe : public Object
{
public:
  Pipe(const std::shared_ptr<Object>& object, const std::string& filtername, const std::vector<json::Json>& args = {}, size_t off = std::numeric_limits<size_t>::max());
  Pipe(const std::shared_ptr<Object>& object, const std::string& filtername, size_t off = std::numeric_limits<size_t>::max());
  ~Pipe() = default;

  json::Json accept(Renderer& r) override;

public:
  std::shared_ptr<Object> object;
  std::string filterName;
  std::vector<json::Json> arguments;
};

} // namespace objects

} // namespace liquid

#endif // LIQUID_OBJECTS_H
