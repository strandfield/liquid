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

Context::Scope::Scope(Context& c, ScopeKind k)
  : context_(&c)
{
  c.scopes().emplace_back();
  c.scopes().back().kind = k;
}

Context::Scope::Scope(Context& c, const Template& tmplt)
  : context_(&c)
{
  c.scopes().emplace_back();
  c.scopes().back().kind = Context::FileScope;
  c.scopes().back().template_ = &tmplt;
}

Context::Scope::~Scope()
{
  context_->scopes().pop_back();
}

json::Json& Context::Scope::operator[](const std::string& str)
{
  return context_->scopes().back().data[str];
}

const Template& Context::currentTemplate() const
{
  for (size_t i(m_stack.size()); i-- > 0; )
  {
    if (m_stack.at(i).template_ != nullptr)
      return *m_stack.at(i).template_;
  }

  throw std::runtime_error{ "No active template" };
}

Context::ScopeData& Context::currentFileScope()
{
  for (size_t i(m_stack.size()); i-- > 0; )
  {
    if (m_stack.at(i).kind == FileScope)
      return m_stack[i];
  }

  throw std::runtime_error{ "No active file scope" };
}

Context::ScopeData& Context::parentFileScope()
{
  bool is_next = false;

  for (size_t i(m_stack.size()); i-- > 0; )
  {
    if (m_stack.at(i).kind == FileScope)
    {
      if (is_next)
        return m_stack[i];
      else
        is_next = true;
    }
  }

  throw std::runtime_error{ "No active file scope" };
}

} // namespace liquid
