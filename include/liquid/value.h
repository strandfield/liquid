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
class Array;
class Map;

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
  
  Value(nullptr_t);
  Value(bool b);
  Value(int n);
  Value(double x);
  Value(std::string str);
  Value(const char* str);

  Value(std::vector<Value> vals);
  Value(std::map<std::string, Value> dict);

  explicit Value(std::shared_ptr<IValue> impl);

  static const std::shared_ptr<IValue> null_impl;

  bool isNull() const;
  bool isArray() const;
  bool isMap() const;
  bool isSimple() const;

  Array toArray() const;
  Map toMap() const;

  std::type_index typeIndex() const;
  void* data() const;

  template<typename T>
  bool is() const;

  template<typename T>
  T& as() const;

  size_t length() const;
  Value at(size_t index) const;

  std::set<std::string> propertyNames() const;
  Value property(const std::string& name) const;

  const std::shared_ptr<IValue>& impl() const;

private:
  std::shared_ptr<IValue> d;
};

class LIQUID_API Array
{
public:
  Array();
  Array(const Array&) = default;
  ~Array() = default;

  Array(std::vector<Value> vals);

  explicit Array(std::shared_ptr<IValue> impl);

  size_t length() const;
  Value at(size_t index) const;

  bool isWritable() const;
  void push(Value val);

  Value& operator[](size_t index);

  operator Value() const;

  const std::shared_ptr<IValue>& impl() const;

private:
  std::shared_ptr<IValue> d;
};

class LIQUID_API Map
{
public:
  Map();
  Map(const Map&) = default;
  ~Map() = default;

  Map(std::map<std::string, Value> dict);
  Map(std::initializer_list<std::pair<const std::string, Value>>&& pairs);

  explicit Map(std::shared_ptr<IValue> impl);

  std::set<std::string> propertyNames() const;
  Value property(const std::string& name) const;

  bool isWritable() const;
  void insert(const std::string& name, Value val);

  Value& operator[](const std::string& name);

  operator Value() const;

  const std::shared_ptr<IValue>& impl() const;

private:
  std::shared_ptr<IValue> d;
};

LIQUID_API int compare(const Value& lhs, const Value& rhs);

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
