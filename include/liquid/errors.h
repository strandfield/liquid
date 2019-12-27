// Copyright (C) 2019 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef LIQUID_ERRORS_H
#define LIQUID_ERRORS_H

#include "liquid/liquid-defs.h"

#include <stdexcept>

namespace liquid
{

class LIQUID_API Exception : public std::exception
{
public:
  Exception() = default;
};

} // namespace liquid

#endif // LIQUID_ERRORS_H
