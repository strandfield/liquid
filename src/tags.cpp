// Copyright (C) 2019 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "liquid/tags.h"

#include "liquid/renderer.h"

namespace liquid
{

Tag::Tag(size_t off)
  : Node(off)
{

}

namespace tags
{

Comment::Comment()
{

}

void Comment::accept(Renderer& /* r */)
{

}

Assign::Assign(const std::string& varname, const std::shared_ptr<Object>& expr, size_t off)
  : Tag(off), 
    variable(varname),
    value(expr)
{

}

void Assign::accept(Renderer& r)
{
  r.visitTag(*this);
}


For::For(const std::string& varname, const std::shared_ptr<Object>& expr, size_t off)
  : Tag(off),
    variable(varname),
    object(expr)
{

}

void For::accept(Renderer& r)
{
  r.visitTag(*this);
}

Break::Break(size_t off)
  : Tag(off)
{

}

void Break::accept(Renderer& r)
{
  r.visitTag(*this);
}

Continue::Continue(size_t off)
  : Tag(off)
{

}

void Continue::accept(Renderer& r)
{
  r.visitTag(*this);
}

If::If(std::shared_ptr<Object> cond, size_t off)
  : Tag(off)
{
  Block b;
  b.condition = cond;
  blocks.push_back(b);
}

void If::accept(Renderer& r)
{
  r.visitTag(*this);
}

Eject::Eject()
{

}

void Eject::accept(Renderer& r)
{
  r.visitTag(*this);
}

Discard::Discard()
{

}

void Discard::accept(Renderer& r)
{
  r.visitTag(*this);
}


Include::Include(std::string n)
  : name(std::move(n))
{
}

void Include::accept(Renderer& r)
{
  r.visitTag(*this);
}

} // namespace tags

} // namespace liquid
