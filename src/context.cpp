// Copyright (C) 2019 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "liquid/context.h"

namespace liquid
{

Context::Context()
  : m_flags(0)
{
  
}

Context::Scope::Scope(Context& c)
  : context_(&c)
{
  c.scopes().emplace_back();
}

Context::Scope::~Scope()
{
  context_->scopes().pop_back();
}

json::Json& Context::Scope::operator[](const std::string& str)
{
  return context_->scopes().back().data[str];
}

} // namespace liquid
