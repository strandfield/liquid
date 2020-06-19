// Copyright (C) 2019 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "liquid/filters.h"

#include "liquid/errors.h"

namespace liquid
{

json::Json ArrayFilters::applyAny(const std::string& name, const json::Array& vec, const std::vector<json::Json>& args)
{
  json::Serializer serializer;

  if (name == "join")
    return filters::apply(&join, vec, args, serializer);
  else if (name == "concat")
    return concat(vec, args.front().toArray());
  else if (name == "first")
    return first(vec);
  else if (name == "last")
    return last(vec);
  else if (name == "map")
    return map(vec, args.front().toString());
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

json::Array ArrayFilters::concat(const json::Array& a, const json::Array& b)
{
  json::Array result;

  for (int i(0); i < a.length(); ++i)
    result.push(a.at(i));

  for (int i(0); i < b.length(); ++i)
    result.push(b.at(i));

  return result;
}

json::Json ArrayFilters::first(const json::Array& a)
{
  return a.at(0);
}

json::Json ArrayFilters::last(const json::Array& a)
{
  return a.at(a.length() - 1);
}

json::Array ArrayFilters::map(const json::Array& a, const std::string& field)
{
  json::Array result;

  for (int i(0); i < a.length(); ++i)
    result.push(a.at(i).toObject().data().at(field));

  return result;
}

} // namespace liquid

