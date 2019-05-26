// Copyright (C) 2019 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef LIQUID_GLOBAL_DEFS_H
#define LIQUID_GLOBAL_DEFS_H

#if (defined(WIN32) || defined(_WIN32)) && !defined(LIQUID_BUILD_STATIC_LIBRARY)
#if defined(LIQUID_BUILD_SHARED_LIBRARY)
#  define LIQUID_API __declspec(dllexport)
#else
#  define LIQUID_API __declspec(dllimport)
#endif
#else
#define LIQUID_API
#endif

#include "liquid/liquid-string-backend.h"

#endif // LIQUID_GLOBAL_DEFS_H
