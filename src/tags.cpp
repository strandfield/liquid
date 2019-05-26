// Copyright (C) 2019 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "liquid/tags.h"

#include "liquid/renderer.h"

namespace liquid
{

namespace tags
{

Assign::Assign(const String& varname, const std::shared_ptr<Object>& expr)
  : variable(varname),
    value(expr)
{

}

void Assign::accept(Renderer& r)
{
  r.visitTag(*this);
}


For::For(const String& varname, const std::shared_ptr<Object>& expr)
  : variable(varname),
    object(expr)
{

}

void For::accept(Renderer& r)
{
  r.visitTag(*this);
}

Break::Break() { }

void Break::accept(Renderer& r)
{
  r.visitTag(*this);
}

Continue::Continue() { }

void Continue::accept(Renderer& r)
{
  r.visitTag(*this);
}

If::If(std::shared_ptr<Object> cond)
{
  Block b;
  b.condition = cond;
  blocks.push_back(b);
}

void If::accept(Renderer& r)
{
  r.visitTag(*this);
}

} // namespace tags

} // namespace liquid
