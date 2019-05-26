// Copyright (C) 2019 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef LIQUID_FILTER_H
#define LIQUID_FILTER_H

#include "liquid-defs.h"

#include <json-toolkit/serialization.h>

namespace liquid
{

namespace filters
{

template<typename R, typename T>
json::Json apply(R(*f)(T), const json::Json& obj, const std::vector<json::Json>& args, json::Serializer& s)
{
  using U = typename std::remove_const<typename std::remove_reference<T>::type>::type;

  if (args.size() != 0)
    throw std::runtime_error("Invalid argument count for filter");

  return s.encode(f(s.decode<U>(obj)));
}

template<typename R, typename T1, typename T2>
json::Json apply(R(*f)(T1, T2), const json::Json& obj, const std::vector<json::Json>& args, json::Serializer& s)
{
  using U1 = typename std::remove_const<typename std::remove_reference<T1>::type>::type;
  using U2 = typename std::remove_const<typename std::remove_reference<T2>::type>::type;

  if (args.size() != 1)
    throw std::runtime_error("Invalid argument count for filter");

  return s.encode(f(s.decode<U1>(obj), s.decode<U2>(args.at(0))));
}

template<typename R, typename T1, typename T2, typename T3>
json::Json apply(R(*f)(T1, T2, T3), const json::Json& obj, const std::vector<json::Json>& args, json::Serializer& s)
{
  using U1 = typename std::remove_const<typename std::remove_reference<T1>::type>::type;
  using U2 = typename std::remove_const<typename std::remove_reference<T2>::type>::type;
  using U3 = typename std::remove_const<typename std::remove_reference<T3>::type>::type;

  if (args.size() != 2)
    throw std::runtime_error("Invalid argument count for filter");

  return s.encode(f(s.decode<U1>(obj), s.decode<U2>(args.at(0)), s.decode<U3>(args.at(1))));
}

} // namespace filters

} // namespace liquid

#endif // LIQUID_FILTER_H
