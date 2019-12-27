// Copyright (C) 2019 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef LIQUID_STRINGBACKEND_H
#define LIQUID_STRINGBACKEND_H


#include <iterator>
#include <string>

namespace liquid
{

class StringBackend
{
public:
  typedef std::string string_type;
  typedef std::string string_view_type;
  typedef char char_type;

  static int to_integer(const string_type& str) { return std::stoi(str); }
  static string_type from_integer(int n) { return std::to_string(n); }
  static string_type from_number(double x) { return std::to_string(x); }

  static int compare(const string_type& lhs, const string_type& rhs)
  {
    return lhs.compare(rhs);
  }

  static string_type normalize(const string_type& str)
  {
    std::string result = str;
    size_t pos = result.find("\r\n");
    while (pos != std::string::npos)
    {
      result.replace(pos, 2, "\n");
      pos = result.find("\r\n", pos);
    }
    return result;
  }

  static bool is_space(char_type c) { return c == ' ' || c == '\t'; }
  static bool is_newline(char_type c) { return c == '\n'; }
  static bool is_digit(char_type c) { return c >= '0' && c <= '9'; }
  static bool is_letter(char_type c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
  static bool is_letter_or_number(char_type c) { return is_digit(c) || is_letter(c); }
};

} // namespace liquid

#endif // !LIQUID_STRINGBACKEND_H
