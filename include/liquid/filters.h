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
  static json::Json applyAny(const std::string& name, const json::Array& vec, const std::vector<json::Json>& args);

  static std::string join(const std::vector<std::string>& strings, const std::string& sep);
  static json::Array concat(const json::Array& a, const json::Array& b);
  static json::Json first(const json::Array& a);
  static json::Json last(const json::Array& a);
  static json::Array map(const json::Array& a, const std::string& field);
  static json::Array push(const json::Array& a, const json::Json& elem);
  static json::Array pop(const json::Array& a);
};

} // namespace liquid

#endif // LIQUID_FILTERS_H
