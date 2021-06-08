// Copyright (C) 2019 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef LIQUID_FILTERS_H
#define LIQUID_FILTERS_H

#include "liquid/filter.h"

namespace liquid
{

class LIQUID_API ArrayFilters
{
public:
  static liquid::Value applyAny(const std::string& name, const liquid::Array& vec, const std::vector<liquid::Value>& args);

  static std::string join(const std::vector<std::string>& strings, const std::string& sep);
  static std::string join(const liquid::Array& strings, const liquid::Value& sep);
  static liquid::Array concat(const liquid::Array& a, const liquid::Array& b);
  static liquid::Value first(const liquid::Array& a);
  static liquid::Value last(const liquid::Array& a);
  static liquid::Array map(const liquid::Array& a, const std::string& field);
  static liquid::Array push(const liquid::Array& a, const liquid::Value& elem);
  static liquid::Array pop(const liquid::Array& a);
};

class LIQUID_API BuiltinFilters
{
public:
  static liquid::Value apply(const std::string& name, const liquid::Value& object, const std::vector<liquid::Value>& args);
};

} // namespace liquid

#endif // LIQUID_FILTERS_H
