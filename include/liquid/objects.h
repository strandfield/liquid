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
  Variable(const std::string& n);
  Variable(std::string&& n);
  ~Variable() = default;

public:
  std::string name;
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
  MemberAccess(const std::shared_ptr<Object>& obj, const std::string& name);
  ~MemberAccess() = default;

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
  Pipe(const std::shared_ptr<Object>& object, const std::string& filtername, const std::vector<json::Json>& args = {});
  ~Pipe() = default;

public:
  std::shared_ptr<Object> object;
  std::string filterName;
  std::vector<json::Json> arguments;
};

} // namespace objects

} // namespace liquid

#endif // LIQUID_OBJECTS_H
