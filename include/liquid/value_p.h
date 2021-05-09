// Copyright (C) 2021 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

// WARNING: This file is part of the private API of the library,
//          it may change in a non backward compatible way between minor 
//          release without notice.
//          You've been warned!

#ifndef LIQUID_VALUE_P_H
#define LIQUID_VALUE_P_H

#include "liquid/value.h"

#include <vector>
#include <map>

namespace liquid
{

class LIQUID_API NullValue : public IValue
{
public:
  NullValue();

  bool is_null() const override;
  std::type_index type_index() const override;
};

class LIQUID_API VectorValue : public IValue
{
public:
  std::vector<Value> values;

public:
  explicit VectorValue(std::vector<Value> vals);

  bool is_array() const override;

  std::type_index type_index() const override;
  void* data() override;

  size_t length() const override;
  Value at(size_t index) const override;
};

class LIQUID_API MapValue : public IValue
{
public:
  std::map<std::string, Value> dict;

public:
  explicit MapValue(std::map<std::string, Value> m);

  bool is_map() const override;

  std::type_index type_index() const override;
  void* data() override;

  std::set<std::string> propertyNames() const override;
  Value property(const std::string& name) const override;
};

} // namespace liquid

#endif // LIQUID_VALUE_P_H
