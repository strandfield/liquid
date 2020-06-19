// Copyright (C) 2020 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "liquid/errors.h"

namespace liquid
{

EvaluationException::EvaluationException(std::string mssg)
  : message_(std::move(mssg)),
  template_(nullptr),
  offset_(std::numeric_limits<size_t>::max())
{

}

EvaluationException::EvaluationException(std::string mssg, const Template& tmplt, size_t off)
  : message_(std::move(mssg)),
  template_(&tmplt),
  offset_(off)
{

}

const char* EvaluationException::what() const noexcept
{
  return "liquid::Renderer evaluation error";
}

} // namespace liquid
