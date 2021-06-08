// Copyright (C) 2019-2021 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef LIQUID_CONTEXT_H
#define LIQUID_CONTEXT_H

#include "liquid/liquid-defs.h"

#include "liquid/value.h"

#include <vector>

namespace liquid
{

class Template;

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

  struct LIQUID_API Scope
  {
    Scope(Context& c, ScopeKind k);
    Scope(Context& c, const Template& tmplt);
    Scope(Context& c, const Template& tmplt, liquid::Map data);
    ~Scope();

    Value& operator[](const std::string& str);

  private:
    Context* context_;
  };

  struct ScopeData
  {
    ScopeKind kind = GlobalScope;
    liquid::Map data;
    const Template* template_ = nullptr;
  };

  const Template& currentTemplate() const;

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
