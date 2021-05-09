// Copyright (C) 2021 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef LIQUID_VALUE_H
#define LIQUID_VALUE_H

#include "liquid/liquid-defs.h"

#include <cassert>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <vector>

namespace liquid
{

class Value;

class LIQUID_API IValue
{
public:
  virtual ~IValue();

  virtual bool is_null() const;
  virtual bool is_array() const;
  virtual bool is_map() const;

  virtual std::type_index type_index() const = 0;
  virtual void* data();

  virtual size_t length() const;
  virtual Value at(size_t index) const;

  virtual std::set<std::string> propertyNames() const;
  virtual Value property(const std::string& name) const;
};

template<typename T>
class GenericValue : public IValue
{
public:
  T value;

public:
  explicit GenericValue(T val)
    : value(std::move(val))
  {

  }

  std::type_index type_index() const override
  {
    return std::type_index(typeid(T));
  }

  void* data() override
  {
    return reinterpret_cast<void*>(&value);
  }
};

class LIQUID_API Value
{
public:
  Value();
  Value(const Value&) = default;
  ~Value() = default;
  
  explicit Value(int n);
  explicit Value(double x);
  explicit Value(std::string str);

  explicit Value(std::vector<Value> vals);
  explicit Value(std::map<std::string, Value> dict);

  explicit Value(std::shared_ptr<IValue> impl);

  static const std::shared_ptr<IValue> null_impl;

  bool isNull() const;
  bool isArray() const;
  bool isMap() const;
  bool isSimple() const;

  template<typename T>
  bool is() const;

  template<typename T>
  T& as() const;

  size_t length() const;
  Value at(size_t index) const;

  std::set<std::string> propertyNames() const;
  Value property(const std::string& name) const;

  Value operator[](size_t index) const;
  Value operator[](const std::string& name) const;

  const std::shared_ptr<IValue>& impl() const;

private:
  std::shared_ptr<IValue> d;
};

} // namespace liquid

namespace liquid
{

template<typename T>
inline bool Value::is() const
{
  return d->type_index() == std::type_index(typeid(T));
}

template<typename T>
inline T& Value::as() const
{
  assert(is<T>());
  void* data = d->data();
  return *reinterpret_cast<T*>(data);
}

} // namespace liquid

#endif // LIQUID_VALUE_H
