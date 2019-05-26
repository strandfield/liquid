// Copyright (C) 2019 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef LIQUID_GENERIC_TAG_H
#define LIQUID_GENERIC_TAG_H

#include "liquid/tag.h"

#include "liquid/renderer.h"

#include <cassert>

namespace liquid
{

template<typename T, typename R = Renderer>
class GenericTag : public Tag
{
public:

  void accept(Renderer& r) override
  {
    assert(dynamic_cast<R*>(&r) != nullptr);
    static_cast<R*>(&r)->visitTag(*static_cast<T*>(this));
  }
};

} // namespace liquid

#endif // LIQUID_GENERIC_TAG_H
