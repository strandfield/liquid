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
  Value(const json::Json& val);
  ~Value() = default;

public:
  json::Json value;
};

class Variable : public Object
{
public:
  Variable(const String& n);
  Variable(String&& n);
  ~Variable() = default;

public:
  String name;
};

class ArrayAccess : public Object
{
public:
  ArrayAccess(const std::shared_ptr<Object>& obj, const std::shared_ptr<Object>& ind);
  ~ArrayAccess() = default;

public:
  std::shared_ptr<Object> object;
  std::shared_ptr<Object> index;
};

class MemberAccess : public Object
{
public:
  MemberAccess(const std::shared_ptr<Object>& obj, const String& name);
  ~MemberAccess() = default;

public:
  std::shared_ptr<Object> object;
  String name;
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
    Or
  };

  BinOp(Operation op, const std::shared_ptr<Object>& left, const std::shared_ptr<Object>& right);
  ~BinOp() = default;

public:
  Operation operation;
  std::shared_ptr<Object> lhs;
  std::shared_ptr<Object> rhs;
};

class Pipe : public Object
{
public:
  Pipe(const std::shared_ptr<Object>& object, const String& filtername, const std::vector<json::Json>& args = {});
  ~Pipe() = default;

public:
  std::shared_ptr<Object> object;
  String filterName;
  std::vector<json::Json> arguments;
};

} // namespace objects

} // namespace liquid

#endif // LIQUID_OBJECTS_H
