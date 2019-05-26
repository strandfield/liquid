// Copyright (C) 2019 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef LIQUID_CONTEXT_H
#define LIQUID_CONTEXT_H

#include "liquid/liquid-defs.h"

#include <json-toolkit/json.h>

#include <vector>

namespace liquid
{

class LIQUID_API Context
{
public:
  Context();
  ~Context() = default;

  enum Flag {
    NoFlags = 0,
    Break = 1,
    Continue = 2,
  };

  int& flags() { return m_flags; }

  struct Scope
  {
    explicit Scope(Context& c);
    ~Scope();

    json::Json& operator[](const String& str);

  private:
    Context* context_;
  };

  struct ScopeData
  {
    json::Object data;
  };

  ScopeData& currentScope() { return m_stack.back(); }
  std::vector<ScopeData>& scopes() { return m_stack; }

private:
  int m_flags;
  std::vector<ScopeData> m_stack;
};

} // namespace liquid

#endif // LIQUID_CONTEXT_H
