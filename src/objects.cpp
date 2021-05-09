// Copyright (C) 2019-2021 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "liquid/objects.h"

#include "liquid/renderer.h"

namespace liquid
{

Object::Object(size_t off)
  : Node(off)
{

}

namespace objects
{

Value::Value(const liquid::Value& val, size_t off)
  : Object(off),
    value(val)
{

}

liquid::Value Value::accept(Renderer& r)
{
  return r.visitObject(*this);
}

Variable::Variable(std::string n, size_t off)
  : Object(off),
    name(std::move(n))
{

}

liquid::Value Variable::accept(Renderer& r)
{
  return r.visitObject(*this);
}

ArrayAccess::ArrayAccess(const std::shared_ptr<Object>& obj, const std::shared_ptr<Object>& ind, size_t off)
  : Object(off), 
    object(obj),
    index(ind)
{

}

liquid::Value ArrayAccess::accept(Renderer& r)
{
  return r.visitObject(*this);
}

MemberAccess::MemberAccess(const std::shared_ptr<Object>& obj, const std::string& name, size_t off)
  : Object(off),
    object(obj),
    name(name)
{

}

liquid::Value MemberAccess::accept(Renderer& r)
{
  return r.visitObject(*this);
}

BinOp::BinOp(Operation op, const std::shared_ptr<Object>& left, const std::shared_ptr<Object>& right, size_t off)
  : Object(off), 
    operation(op),
    lhs(left),
    rhs(right)
{

}

liquid::Value BinOp::accept(Renderer& r)
{
  return r.visitObject(*this);
}

LogicalNot::LogicalNot(const std::shared_ptr<Object>& obj, size_t off)
  : Object(off),
    object(obj)
{

}

liquid::Value LogicalNot::accept(Renderer& r)
{
  return r.visitObject(*this);
}

Pipe::Pipe(const std::shared_ptr<Object>& object, const std::string& filtername, const std::vector<std::shared_ptr<Object>>& args, size_t off)
  : Object(off), 
    object(object),
    filterName(filtername),
    arguments(args)
{

}

Pipe::Pipe(const std::shared_ptr<Object>& object, const std::string& filtername, size_t off)
  : Object(off),
    object(object),
    filterName(filtername)
{

}

liquid::Value Pipe::accept(Renderer& r)
{
  return r.visitObject(*this);
}

} // namespace objects

} // namespace liquid
