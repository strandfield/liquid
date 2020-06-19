// Copyright (C) 2019 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef LIQUID_ERRORS_H
#define LIQUID_ERRORS_H

#include "liquid/liquid-defs.h"

#include <stdexcept>

namespace liquid
{

#pragma warning(push)
#pragma warning(disable: 4275)

class LIQUID_API Exception : public std::exception
{
public:
  Exception() = default;
};

#pragma warning(pop)


class Template;

class LIQUID_API EvaluationException : public Exception
{
public:
  std::string message_;
  const Template* template_;
  size_t offset_;

public:
  explicit EvaluationException(std::string mssg);
  EvaluationException(std::string mssg, const Template& tmplt, size_t off);

  const char* what() const noexcept override;
};


} // namespace liquid

#endif // LIQUID_ERRORS_H
