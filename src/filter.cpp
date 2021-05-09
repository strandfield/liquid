// Copyright (C) 2019-2021 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "liquid/filters.h"

#include "liquid/errors.h"

namespace liquid
{

liquid::Value ArrayFilters::applyAny(const std::string& name, const liquid::Array& vec, const std::vector<liquid::Value>& args)
{
  if (name == "join")
    return join(vec, args.front());
  else if (name == "concat")
    return concat(vec, args.front().toArray());
  else if (name == "first")
    return first(vec);
  else if (name == "last")
    return last(vec);
  else if (name == "map")
    return map(vec, args.front().as<std::string>());
  else if (name == "push")
    return push(vec, args.front());
  else if (name == "pop")
    return pop(vec);
  else
    throw EvaluationException{ "Invalid filter name '" + name + "'" };
}

std::string ArrayFilters::join(const std::vector<std::string>& strings, const std::string& sep)
{
  std::string result;

  if (strings.empty())
    return result;

  result = strings.front();
  
  if (strings.size() == 1)
    return result;

  for (size_t i(1); i < strings.size(); ++i)
  {
    result += sep;
    result += strings.at(i);
  }

  return result;
}

std::string ArrayFilters::join(const liquid::Array& vec, const liquid::Value& sep)
{
  std::vector<std::string> strings;

  for (size_t i(0); i < vec.length(); ++i)
  {
    if (vec.at(i).is<std::string>())
      strings.push_back(vec.at(i).as<std::string>());
  }

  return join(strings, sep.is<std::string>() ? sep.as<std::string>() : std::string(""));
}

liquid::Array ArrayFilters::concat(const liquid::Array& a, const liquid::Array& b)
{
  liquid::Array result;

  for (int i(0); i < a.length(); ++i)
    result.push(a.at(i));

  for (int i(0); i < b.length(); ++i)
    result.push(b.at(i));

  return result;
}

liquid::Value ArrayFilters::first(const liquid::Array& a)
{
  return a.at(0);
}

liquid::Value ArrayFilters::last(const liquid::Array& a)
{
  return a.at(a.length() - 1);
}

liquid::Array ArrayFilters::map(const liquid::Array& a, const std::string& field)
{
  liquid::Array result;

  for (int i(0); i < a.length(); ++i)
    result.push(a.at(i).property(field));

  return result;
}

liquid::Array ArrayFilters::push(const liquid::Array& a, const liquid::Value& elem)
{
  liquid::Array result;

  for (int i(0); i < a.length(); ++i)
    result.push(a.at(i));

  result.push(elem);

  return result;
}

liquid::Array ArrayFilters::pop(const liquid::Array& a)
{
  liquid::Array result;

  for (int i(0); i < a.length() - 1; ++i)
    result.push(a.at(i));

  return result;
}

liquid::Value BuiltinFilters::apply(const std::string& name, const liquid::Value& object, const std::vector<liquid::Value>& args)
{
  if (object.isArray())
  {
    return ArrayFilters::applyAny(name, object.toArray(), args);
  }
  else
  {
    throw EvaluationException{ "Invalid filter name '" + name + "'" };
  }
}

} // namespace liquid

