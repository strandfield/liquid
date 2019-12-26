// Copyright (C) 2019 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "liquid/objects.h"

namespace liquid
{

namespace objects
{


Value::Value(const json::Json& val)
  : value(val)
{

}

Variable::Variable(std::string n)
  : name(std::move(n))
{

}

ArrayAccess::ArrayAccess(const std::shared_ptr<Object>& obj, const std::shared_ptr<Object>& ind)
  : object(obj),
  index(ind)
{

}

MemberAccess::MemberAccess(const std::shared_ptr<Object>& obj, const std::string& name)
  : object(obj),
  name(name)
{

}

BinOp::BinOp(Operation op, const std::shared_ptr<Object>& left, const std::shared_ptr<Object>& right)
  : operation(op),
  lhs(left),
  rhs(right)
{

}

Pipe::Pipe(const std::shared_ptr<Object>& object, const std::string& filtername, const std::vector<json::Json>& args)
  : object(object),
  filterName(filtername),
  arguments(args)
{

}

} // namespace objects

} // namespace liquid
