// Copyright (C) 2021 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "liquid/value.h"
#include "liquid/value_p.h"

#include <stdexcept>

namespace liquid
{

const std::shared_ptr<IValue> Value::null_impl = std::make_shared<NullValue>();

NullValue::NullValue()
{

}

bool NullValue::is_null() const
{
  return true;
}


std::type_index NullValue::type_index() const
{
  return std::type_index(typeid(nullptr_t));
}


VectorValue::VectorValue()
{

}

VectorValue::VectorValue(std::vector<Value> vals)
  : values(std::move(vals))
{

}

bool VectorValue::is_array() const
{
  return true;
}

std::type_index VectorValue::type_index() const
{
  return std::type_index(typeid(std::vector<Value>));
}

void* VectorValue::data()
{
  return reinterpret_cast<void*>(&values);
}

size_t VectorValue::length() const
{
  return values.size();
}

Value VectorValue::at(size_t index) const
{
  return values.at(index);
}


MapValue::MapValue()
{

}

MapValue::MapValue(std::map<std::string, Value> m)
  : dict(std::move(m))
{

}

bool MapValue::is_map() const
{
  return true;
}

std::type_index MapValue::type_index() const 
{
  return std::type_index(typeid(std::map<std::string, Value>));
}

void* MapValue::data()
{
  return reinterpret_cast<void*>(&dict);
}

std::set<std::string> MapValue::propertyNames() const
{
  std::set<std::string> names;

  for (const auto& e : dict)
  {
    names.insert(e.first);
  }

  return names;
}

Value MapValue::property(const std::string& name) const
{
  auto it = dict.find(name);
  return it != dict.end() ? it->second : Value();
}



IValue::~IValue()
{

}

bool IValue::is_null() const
{
  return false;
}

bool IValue::is_array() const
{
  return false;
}

bool IValue::is_map() const
{
  return false;
}

void* IValue::data()
{
  return nullptr;
}

size_t IValue::length() const
{
  return 0;
}

Value IValue::at(size_t /* index */) const
{
  return Value();
}

std::set<std::string> IValue::propertyNames() const
{
  return {};
}

Value IValue::property(const std::string& /* name */) const
{
  return Value();
}


Value::Value()
  : d(null_impl)
{

}

Value::Value(nullptr_t)
  : d(null_impl)
{

}

Value::Value(bool b)
  : d(std::make_shared<GenericValue<bool>>(b))
{

}

Value::Value(int n)
  : d(std::make_shared<GenericValue<int>>(n))
{

}

Value::Value(double x)
  : d(std::make_shared<GenericValue<double>>(x))
{

}

Value::Value(std::string str)
  : d(std::make_shared<GenericValue<std::string>>(std::move(str)))
{

}

Value::Value(const char* str)
  : d(std::make_shared<GenericValue<std::string>>(std::string(str)))
{

}

Value::Value(std::vector<Value> vals)
  : d(std::make_shared<VectorValue>(std::move(vals)))
{

}

Value::Value(std::map<std::string, Value> dict)
  : d(std::make_shared<MapValue>(std::move(dict)))
{

}

Value::Value(std::shared_ptr<IValue> impl)
  : d(impl != nullptr ? impl : null_impl)
{

}

bool Value::isNull() const
{
  return d->is_null();
}

bool Value::isArray() const
{
  return d->is_array();
}

bool Value::isMap() const
{
  return d->is_map();
}

bool Value::isSimple() const
{
  return !isNull() && !isArray() && !isMap();
}

Array Value::toArray() const
{
  return Array(d);
}

Map Value::toMap() const
{
  return Map(d);
}

std::type_index Value::typeIndex() const
{
  return d->type_index();
}

void* Value::data() const
{
  return d->data();
}

size_t Value::length() const
{
  return d->length();
}

Value Value::at(size_t index) const
{
  return d->at(index);
}

std::set<std::string> Value::propertyNames() const
{
  return d->propertyNames();
}

Value Value::property(const std::string& name) const
{
  return d->property(name);
}

const std::shared_ptr<IValue>& Value::impl() const
{
  return d;
}


Array::Array()
  : d(std::make_shared<VectorValue>())
{

}

Array::Array(std::vector<Value> vals)
 : d(std::make_shared<VectorValue>(std::move(vals)))
{

}

Array::Array(std::shared_ptr<IValue> impl)
  : d(impl)
{
  if (!d || !d->is_array())
    d = std::make_shared<VectorValue>();
}

size_t Array::length() const
{
  return d->length();
}

Value Array::at(size_t index) const
{
  return d->at(index);
}

bool Array::isWritable() const
{
  return d->type_index() == std::type_index(typeid(decltype(VectorValue::values)));
}

void Array::push(Value val)
{
  assert(isWritable());

  if (!isWritable())
    throw std::runtime_error{ "Array is not writable" };

  auto* vec = static_cast<VectorValue*>(d.get());
  vec->values.push_back(val);
}

Value& Array::operator[](size_t index)
{
  auto* self = static_cast<VectorValue*>(d.get());
  return self->values[index];
}

Array::operator Value() const
{
  return Value(d);
}

const std::shared_ptr<IValue>& Array::impl() const
{
  return d;
}


Map::Map()
  : d(std::make_shared<MapValue>())
{

}

Map::Map(std::map<std::string, Value> dict)
  : d(std::make_shared<MapValue>(std::move(dict)))
{

}

Map::Map(std::initializer_list<std::pair<const std::string, Value>>&& pairs)
  : d(std::make_shared<MapValue>(std::map<std::string, Value>(pairs)))
{

}

Map::Map(std::shared_ptr<IValue> impl)
  : d(impl)
{
  if (!d || !d->is_map())
    d = std::make_shared<MapValue>();
}

std::set<std::string> Map::propertyNames() const
{
  return d->propertyNames();
}

Value Map::property(const std::string& name) const
{
  return d->property(name);
}

bool Map::isWritable() const
{
  return d->type_index() == std::type_index(typeid(decltype(MapValue::dict)));
}

void Map::insert(const std::string& name, Value val)
{
  assert(isWritable());

  if (!isWritable())
    throw std::runtime_error{ "Map is not writable" };

  auto* self = static_cast<MapValue*>(d.get());
  self->dict[name] = val;
}

Value& Map::operator[](const std::string& name)
{
  auto* self = static_cast<MapValue*>(d.get());
  return self->dict[name];
}

Map::operator Value() const
{
  return Value(d);
}

const std::shared_ptr<IValue>& Map::impl() const
{
  return d;
}


inline int sign(int diff)
{
  return (0 < diff) - (diff < 0);
}

template<typename T>
int number_compare(const T* lhs_node, const T* rhs_node)
{
  const auto diff = lhs_node->value - rhs_node->value;
  return (0 < diff) - (diff < 0);
}

inline int array_compare(const Value& lhs, const Value& rhs)
{
  int size_diff = static_cast<int>(lhs.length()) - static_cast<int>(rhs.length());

  if (size_diff != 0)
    return (0 < size_diff) - (size_diff < 0);

  for (size_t i(0); i < lhs.length(); ++i)
  {
    int c = liquid::compare(lhs.at(i), rhs.at(i));

    if (c != 0)
      return c;
  }

  return 0;
}

inline int object_compare(const Value& lhs, const Value& rhs)
{
  if (lhs.data() == rhs.data())
    return 0;

  std::set<std::string> lhs_props = lhs.propertyNames();
  std::set<std::string> rhs_props = rhs.propertyNames();

  int size_diff = static_cast<int>(lhs_props.size()) - static_cast<int>(rhs_props.size());

  if (size_diff != 0)
    return sign(size_diff);

  auto lhs_it = lhs_props.begin();
  auto rhs_it = rhs_props.begin();

  for (; lhs_it != lhs_props.end(); ++lhs_it, ++rhs_it)
  {
    int c = lhs_it->compare(*rhs_it);

    if (c != 0)
      return c;

    c = liquid::compare(lhs.property(*lhs_it), rhs.property(*rhs_it));

    if (c != 0)
      return c;
  }

  return 0;
}

inline int comp(bool a, bool b)
{
  return sign(a - b);
}

inline int comp(int a, int b)
{
  return sign(a - b);
}

inline int comp(double a, double b)
{
  return sign(a-b);
}

inline int comp(int a, double b)
{
  return sign(a-b);
}

inline int comp(double a, int b)
{
  return sign(a-b);
}

inline int comp(const std::string& a, const std::string& b)
{
  return std::strcmp(a.data(), b.data());
}

int compare(const Value& lhs, const Value& rhs)
{
  std::type_index lhs_type = lhs.typeIndex();
  std::type_index rhs_type = rhs.typeIndex();

  if (lhs_type != rhs_type)
  {
    if (lhs.is<int>() && rhs.is<double>())
      return comp(lhs.as<int>(), rhs.as<double>());
    else if (lhs.is<double>() && rhs.is<int>())
      return comp(lhs.as<double>(), rhs.as<int>());

    if (lhs_type < rhs_type)
      return -1;
    else
      return 1;
  }
    
  if (lhs.isArray())
    return array_compare(lhs, rhs);
  else if (lhs.isMap())
    return object_compare(lhs, rhs);
  else if (lhs.isNull())
    return 0;
  else if (lhs.is<bool>())
    return comp(lhs.as<bool>(), rhs.as<bool>());
  else if (lhs.is<int>())
    return comp(lhs.as<int>(), rhs.as<int>());
  else if (lhs.is<double>())
    return comp(lhs.as<double>(), rhs.as<double>());
  else if (lhs.is<std::string>())
    return comp(lhs.as<std::string>(), rhs.as<std::string>()); 

  assert(false);
  throw std::runtime_error{ "liquid::compare() : values are not comparable" };
}

} // namespace liquid
