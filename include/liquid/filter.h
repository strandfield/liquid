// Copyright (C) 2019-2021 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef LIQUID_FILTER_H
#define LIQUID_FILTER_H

#include "liquid-defs.h"

#include "liquid/value.h"

namespace liquid
{

namespace filters
{

template<typename R, typename T>
liquid::Value apply(R(*f)(T), const liquid::Value& obj, const std::vector<liquid::Value>& args)
{
  using U = typename std::remove_const<typename std::remove_reference<T>::type>::type;

  if (args.size() != 0)
    throw std::runtime_error("Invalid argument count for filter");

  return f(obj.as<U>());
}

template<typename R, typename T1, typename T2>
liquid::Value apply(R(*f)(T1, T2), const liquid::Value& obj, const std::vector<liquid::Value>& args)
{
  using U1 = typename std::remove_const<typename std::remove_reference<T1>::type>::type;
  using U2 = typename std::remove_const<typename std::remove_reference<T2>::type>::type;

  if (args.size() != 1)
    throw std::runtime_error("Invalid argument count for filter");

  return f(obj.as<U1>(), args.at(0).as<U2>());
}

template<typename R, typename T1, typename T2, typename T3>
liquid::Value apply(R(*f)(T1, T2, T3), const liquid::Value& obj, const std::vector<liquid::Value>& args)
{
  using U1 = typename std::remove_const<typename std::remove_reference<T1>::type>::type;
  using U2 = typename std::remove_const<typename std::remove_reference<T2>::type>::type;
  using U3 = typename std::remove_const<typename std::remove_reference<T3>::type>::type;

  if (args.size() != 2)
    throw std::runtime_error("Invalid argument count for filter");

  return f(obj.as<U1>(), args.at(0).as<U2>(), args.at(1).as<U3>());
}

} // namespace filters

} // namespace liquid

#endif // LIQUID_FILTER_H
