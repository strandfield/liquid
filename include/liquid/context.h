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
    Eject = 4,
    Discard = Eject | 8,
  };

  int& flags() { return m_flags; }

  enum ScopeKind
  {
    GlobalScope,
    FileScope,
    ControlBlockScope,
  };

  struct Scope
  {
    Scope(Context& c, ScopeKind k);
    ~Scope();

    json::Json& operator[](const std::string& str);

  private:
    Context* context_;
  };

  struct ScopeData
  {
    ScopeKind kind = GlobalScope;
    json::Object data;
  };

  ScopeData& currentScope() { return m_stack.back(); }
  ScopeData& currentFileScope();
  ScopeData& parentFileScope();
  std::vector<ScopeData>& scopes() { return m_stack; }

private:
  int m_flags;
  std::vector<ScopeData> m_stack;
};

} // namespace liquid

#endif // LIQUID_CONTEXT_H
