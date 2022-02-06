// Copyright (C) 2021 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "liquid/value.h"
#include "liquid/value_p.h"

#include <cstring>
#include <stdexcept>

/*!
 * \namespace liquid
 */

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
  return std::type_index(typeid(std::nullptr_t));
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

/*!
 * \class IValue
 */

/*!
 * \fn virtual ~IValue()
 */
IValue::~IValue()
{

}

/*!
 * \fn virtual bool is_null() const
 * \brief returns whether this is the null value
 * 
 * The default implementation returns false.
 */
bool IValue::is_null() const
{
  return false;
}

/*!
 * \fn virtual bool is_array() const
 * \brief returns whether this value is an array
 * 
 * The default implementation returns false.
 */
bool IValue::is_array() const
{
  return false;
}

/*!
 * \fn virtual bool is_map() const
 * \brief returns whether this value is a map
 * 
 * The default implementation returns false.
 */
bool IValue::is_map() const
{
  return false;
}

/*!
 * \fn virtual void* data()
 * \brief returns a pointer to the C++ variable this object exposes
 * 
 * The default implementation returns nullptr.
 * 
 * You should override \c{type_index()} to return the type_index of the variable.
 */
void* IValue::data()
{
  return nullptr;
}

/*!
 * \fn virtual size_t length() const
 * \brief returns the length of an array.
 * 
 * The default implementation returns 0.
 * 
 * If \c{is_array()} returns false, this function should return 0.
 */
size_t IValue::length() const
{
  return 0;
}

/*!
 * \fn virtual Value at(size_t index) const
 * \brief access an array by index
 *
 * The default implementation returns a null value.
 *
 * If \c{is_array()} returns false, this function should return a null value.
 */
Value IValue::at(size_t /* index */) const
{
  return Value();
}

/*!
 * \fn virtual std::set<std::string> propertyNames() const
 * \brief returns the names of the property in a map
 *
 * The default implementation returns an empty set.
 *
 * If \c{is_map()} returns false, this function should return an empty set.
 * 
 * This function is not required to return an exhaustive list.
 * For example, if an IValue exposes an object that has some computed properties, 
 * or properties that would induce a cycle (e.g. 'parent' and 'children'), it is 
 * recommended not to list them in this function.
 */
std::set<std::string> IValue::propertyNames() const
{
  return {};
}

/*!
 * \fn virtual Value property(const std::string& name) const
 * \brief the name of the property
 *
 * The default implementation returns a null value.
 *
 * If \c{is_map()} returns false, this function should return a null value.
 */
Value IValue::property(const std::string& /* name */) const
{
  return Value();
}

/*!
 * \endclass
 */

/*!
 * \class Value
 */

/*!
 * \fn Value()
 * \brief constructs a null value
 */
Value::Value()
  : d(null_impl)
{

}

/*!
 * \fn Value(nullptr_t)
 * \brief constructs a null value
 */
Value::Value(std::nullptr_t)
  : d(null_impl)
{

}

/*!
 * \fn Value(bool b)
 * \brief constructs a boolean value
 */
Value::Value(bool b)
  : d(std::make_shared<GenericValue<bool>>(b))
{

}

/*!
 * \fn Value(int n)
 * \brief constructs an integer value
 */
Value::Value(int n)
  : d(std::make_shared<GenericValue<int>>(n))
{

}

/*!
 * \fn Value(double x)
 * \brief constructs a real value
 */
Value::Value(double x)
  : d(std::make_shared<GenericValue<double>>(x))
{

}

/*!
 * \fn Value(std::string str)
 * \brief constructs a string value
 */
Value::Value(std::string str)
  : d(std::make_shared<GenericValue<std::string>>(std::move(str)))
{

}

/*!
 * \fn Value(const char* str)
 * \brief constructs a string value
 */
Value::Value(const char* str)
  : d(std::make_shared<GenericValue<std::string>>(std::string(str)))
{

}

/*!
 * \fn Value(std::vector<Value> vals)
 * \brief constructs an array of values
 */
Value::Value(std::vector<Value> vals)
  : d(std::make_shared<VectorValue>(std::move(vals)))
{

}

/*!
 * \fn Value(std::map<std::string, Value> dict)
 * \brief constructs a map of values
 */
Value::Value(std::map<std::string, Value> dict)
  : d(std::make_shared<MapValue>(std::move(dict)))
{

}

/*!
 * \fn Value(std::shared_ptr<IValue> impl)
 * \brief constructs a value from an implementation
 */
Value::Value(std::shared_ptr<IValue> impl)
  : d(impl != nullptr ? impl : null_impl)
{

}

/*!
 * \fn bool isNull() const
 * \brief returns whether the value is null
 */
bool Value::isNull() const
{
  return d->is_null();
}

/*!
 * \fn bool isArray() const
 * \brief returns whether the value is an array
 * 
 * A value for which \c{isArray()} returns true can be converted to an 
 * Array using \c{toArray()}.
 */
bool Value::isArray() const
{
  return d->is_array();
}

/*!
 * \fn bool isMap() const
 * \brief returns whether the value is a map
 * 
 * A value for which \c{isMap()} returns true can be converted to a 
 * Map using \c{toMap()}.
 */
bool Value::isMap() const
{
  return d->is_map();
}

/*!
 * \fn bool isSimple() const
 * \brief returns whether the value is simple
 * 
 * A value is simple if it is neither an array, a map, or null.
 * 
 * You can test the type of the value using \c{is<T>()} and retrieve it 
 * using \c{as<T>()}.
 */
bool Value::isSimple() const
{
  return !isNull() && !isArray() && !isMap();
}

/*!
 * \fn Array toArray() const
 * \brief attempts a conversion to an array
 *
 * If the value is not an array, an empty array is returned.
 */
Array Value::toArray() const
{
  return Array(d);
}

/*!
 * \fn Map toMap() const
 * \brief attempts a conversion to a map
 *
 * If the value is not a map, an empty map is returned.
 */
Map Value::toMap() const
{
  return Map(d);
}

/*!
 * \fn std::type_index typeIndex() const
 * \brief returns the type_index of the C++ value
 *
 * This is the type of the pointer returned by \c{data()}.
 */
std::type_index Value::typeIndex() const
{
  return d->type_index();
}

/*!
 * \fn void* data() const
 * \brief returns a pointer to the C++ value stored in this object
 *
 * The type of the C++ value can be retrieved using \c{typeIndex()} and 
 * or tested with \c{is<T>()}.
 */
void* Value::data() const
{
  return d->data();
}

/*!
 * \fn size_t length() const
 * \brief returns the length of the array represented by this value
 *
 * If this value is not an array, this returns 0.
 */
size_t Value::length() const
{
  return d->length();
}

/*!
 * \fn Value at(size_t index) const
 * \brief access the array by index
 *
 * If this value is not an array, this returns a null Value.
 */
Value Value::at(size_t index) const
{
  return d->at(index);
}

/*!
 * \fn std::set<std::string> propertyNames() const
 * \brief returns the keys of a map value
 *
 * If this value is not a map, this returns an empty set.
 * 
 * Note that depending on how the map is implemented, this set can 
 * be smaller than the actual set of proprety names.
 * For example, an object that provides a 'parent' property may choose 
 * not to report it in propertyNames() as iterating recursively 
 * would produce an infinite loop.
 */
std::set<std::string> Value::propertyNames() const
{
  return d->propertyNames();
}

/*!
 * \fn Value property(const std::string& name) const
 * \brief retrieves the property of a map
 *
 * If the map does not contain a property with the given name, 
 * a null value is returned.
 */
Value Value::property(const std::string& name) const
{
  return d->property(name);
}

/*!
 * \fn const std::shared_ptr<IValue>& impl() const
 * \brief returns a pointer to the implementation
 */
const std::shared_ptr<IValue>& Value::impl() const
{
  return d;
}

/*!
 * \endclass
 */

/*!
 * \class Array
 */

/*!
 * \fn Array()
 * \brief constructs an empty array
 */
Array::Array()
  : d(std::make_shared<VectorValue>())
{

}

/*!
 * \fn Array(std::vector<Value> vals)
 * \brief constructs an array from a list of values
 */
Array::Array(std::vector<Value> vals)
 : d(std::make_shared<VectorValue>(std::move(vals)))
{

}

/*!
 * \fn Array(std::shared_ptr<IValue> impl)
 * \brief constructs an array from its implementation
 */
Array::Array(std::shared_ptr<IValue> impl)
  : d(impl)
{
  if (!d || !d->is_array())
    d = std::make_shared<VectorValue>();
}

/*!
 * \fn size_t length() const
 * \brief returns the length of the array
 */
size_t Array::length() const
{
  return d->length();
}

/*!
 * \fn Value at(size_t index) const
 * \param index
 * \brief retrieves an element by index
 */
Value Array::at(size_t index) const
{
  return d->at(index);
}

/*!
 * \fn bool isWritable() const
 * \brief returns whether an array is writable
 * 
 * You can call \c{push()} on a writable array to add elements to it.
 * 
 * Arrays constructed using the default constructor, or the vector constructor 
 * of this class are writable.
 */
bool Array::isWritable() const
{
  return d->type_index() == std::type_index(typeid(decltype(VectorValue::values)));
}

/*!
 * \fn void push(Value val)
 * \brief adds a value to the array
 *
 * The array must be writable.
 */
void Array::push(Value val)
{
  assert(isWritable());

  if (!isWritable())
    throw std::runtime_error{ "Array is not writable" };

  auto* vec = static_cast<VectorValue*>(d.get());
  vec->values.push_back(val);
}

/*!
 * \fn Value& operator[](size_t index)
 * \brief access an element by index
 *
 * Note that unlike \c{at()}, this function returns a modifiable reference to 
 * the element.
 */
Value& Array::operator[](size_t index)
{
  auto* self = static_cast<VectorValue*>(d.get());
  return self->values[index];
}

/*!
 * \fn operator Value() const
 * \brief converts the array to a value
 */
Array::operator Value() const
{
  return Value(d);
}

/*!
 * \fn const std::shared_ptr<IValue>& impl() const
 * \brief returns a pointer to the implementation
 */
const std::shared_ptr<IValue>& Array::impl() const
{
  return d;
}

/*!
 * \endclass
 */

 /*!
  * \class Map
  */

/*!
 * \fn Map()
 * \brief constructs an empty map
 */
Map::Map()
  : d(std::make_shared<MapValue>())
{

}

/*!
 * \fn Map(std::map<std::string, Value> dict)
 * \brief constructs a map from given values
 */
Map::Map(std::map<std::string, Value> dict)
  : d(std::make_shared<MapValue>(std::move(dict)))
{

}

/*!
 * \fn Map(std::initializer_list<std::pair<const std::string, Value>>&& pairs)
 * \brief constructs a map from given values
 */
Map::Map(std::initializer_list<std::pair<const std::string, Value>>&& pairs)
  : d(std::make_shared<MapValue>(std::map<std::string, Value>(pairs)))
{

}

/*!
 * \fn Map(std::shared_ptr<IValue> impl)
 * \brief constructs a map from its implementation
 */
Map::Map(std::shared_ptr<IValue> impl)
  : d(impl)
{
  if (!d || !d->is_map())
    d = std::make_shared<MapValue>();
}

/*!
 * \fn std::set<std::string> propertyNames() const
 * \brief returns the keys of a map value
 */
std::set<std::string> Map::propertyNames() const
{
  return d->propertyNames();
}

/*!
 * \fn Value property(const std::string& name) const
 * \param property name
 * \brief retrieves a property by name
 */
Value Map::property(const std::string& name) const
{
  return d->property(name);
}

/*!
 * \fn bool isWritable() const
 * \brief returns whether the map is writable
 */
bool Map::isWritable() const
{
  return d->type_index() == std::type_index(typeid(decltype(MapValue::dict)));
}

/*!
 * \fn void insert(const std::string& name, Value val)
 * \param property name
 * \param property value
 * \brief inserts a new value into the map
 * 
 * The map must be writable.
 */
void Map::insert(const std::string& name, Value val)
{
  assert(isWritable());

  if (!isWritable())
    throw std::runtime_error{ "Map is not writable" };

  auto* self = static_cast<MapValue*>(d.get());
  self->dict[name] = val;
}

/*!
 * \fn Value& operator[](const std::string& name)
 * \param property name
 * \brief access a property by name
 *
 * Unlike \c{property()}, this function returns a modifiable reference 
 * to the value.
 */
Value& Map::operator[](const std::string& name)
{
  auto* self = static_cast<MapValue*>(d.get());
  return self->dict[name];
}

/*!
 * \fn operator Value() const
 * \brief converts the map to a value
 */
Map::operator Value() const
{
  return Value(d);
}

/*!
 * \fn const std::shared_ptr<IValue>& impl() const
 * \brief returns a pointer to the implementation
 */
const std::shared_ptr<IValue>& Map::impl() const
{
  return d;
}

/*!
 * \endclass
 */

inline int sign(int diff)
{
  return (0 < diff) - (diff < 0);
}

inline int sign(double diff)
{
  return (0. < diff) - (diff < 0.);
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

/*!
 * \endnamespace
 */

} // namespace liquid
