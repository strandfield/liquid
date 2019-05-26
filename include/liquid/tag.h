// Copyright (C) 2019 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef LIQUID_TAG_H
#define LIQUID_TAG_H

#include "liquid/template.h"

namespace liquid
{

class Renderer;

class LIQUID_API Tag : public templates::Node
{
public:
  bool isTag() const override { return true; }
  virtual void accept(Renderer& renderer) = 0;
};

} // namespace liquid

#endif // LIQUID_TAG_H
