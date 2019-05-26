// Copyright (C) 2019 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef LIQUID_OBJECT_H
#define LIQUID_OBJECT_H

#include "liquid/template.h"

namespace liquid
{

class LIQUID_API Object : public templates::Node
{
public:
  Object() = default;
  ~Object() = default;

  bool isObject() const override { return true; }
};

} // namespace liquid

#endif // LIQUID_OBJECT_H
