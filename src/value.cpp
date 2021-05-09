// Copyright (C) 2021 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "liquid/value.h"
#include "liquid/value_p.h"

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
  return dict.at(name);
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

Value Value::operator[](size_t index) const
{
  return at(index);
}

Value Value::operator[](const std::string& name) const
{
  return property(name);
}

const std::shared_ptr<IValue>& Value::impl() const
{
  return d;
}

} // namespace liquid
