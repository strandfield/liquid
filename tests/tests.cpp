// Copyright (C) 2019 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "liquid/liquid.h"

#include "liquid/renderer.h"

#include <gtest/gtest.h>

TEST(Liquid, hello) {

  std::string str = "Hello {{ name }}!";

  liquid::Template tmplt = liquid::parse(str);

  json::Object data = {};
  data["name"] = "Alice";
  std::string result = tmplt.render(data);

  ASSERT_EQ(result, "Hello Alice!");
}

TEST(Liquid, greetings) {

  std::string str = "Hi! My name is {{ name }} and I am {{ age }} years old.";

  liquid::Template tmplt = liquid::parse(str);

  json::Object data = {};
  data["name"] = "Bob";
  data["age"] = 18;
  std::string result = tmplt.render(data);

  ASSERT_EQ(result, "Hi! My name is Bob and I am 18 years old.");
}

TEST(Liquid, fruits) {

  std::string str = "I love {% for fruit in fruits %}{{ fruit }}{% if forloop.last == false %}, {% endif %}{% endfor %}!";

  liquid::Template tmplt = liquid::parse(str);

  json::Array fruits;
  fruits.push("apples");
  fruits.push("strawberries");
  fruits.push("bananas");
  json::Object data = {};
  data["fruits"] = fruits;
  std::string result = tmplt.render(data);

  ASSERT_EQ(result, "I love apples, strawberries, bananas!");
}

TEST(Liquid, controlflow) {

  std::string str = "{% for n in numbers %}{% if n > 10 %}{% break %}{% elsif n <= 3 %}{% continue %}{% endif %}{{ n }}{% endfor %}";

  liquid::Template tmplt = liquid::parse(str);

  json::Array numbers;
  numbers.push(1);
  numbers.push(2);
  numbers.push(5);
  numbers.push(4);
  numbers.push(12);
  numbers.push(10);
  json::Object data = {};
  data["numbers"] = numbers;
  std::string result = tmplt.render(data);

  ASSERT_EQ(result, "54");
}

TEST(Liquid, assign_global) {

  std::string str = "{% assign foo = ['bar', 0] global %}";

  liquid::Template tmplt = liquid::parse(str);

  liquid::Renderer renderer;
  json::Object data;
  renderer.render(tmplt, data);

  ASSERT_EQ(data["foo"].at(0).toString(), "bar");
  ASSERT_EQ(data["foo"].at(1).toInt(), 0);
}

TEST(Liquid, logic) {

  std::string str = "{% if x or y %}1{% endif %}"
    "{% if a >= b %}2{% endif %}"
    "{% if a and b %}3{% endif %}"
    "{% if a != b %}4{% endif %}";

  liquid::Template tmplt = liquid::parse(str);

  json::Object data = {};
  data["x"] = true;
  data["y"] = false;
  data["a"] = 5;
  data["b"] = 10;
  std::string result = tmplt.render(data);

  ASSERT_EQ(result, "134");
}

TEST(Liquid, arrayaccess) {

  std::string str = "{% assign index = 1 %}{{ numbers[index] }}";

  liquid::Template tmplt = liquid::parse(str);

  json::Array numbers;
  numbers.push(1);
  numbers.push(2);
  numbers.push(3);
  json::Object data = {};
  data["numbers"] = numbers;
  std::string result = tmplt.render(data);

  ASSERT_EQ(result, "2");
}

static json::Json createContact(const std::string& name, int age, bool restricted = false)
{
  json::Object obj = {};
  obj["name"] = name;
  obj["age"] = age;

  if (restricted)
    obj["private"] = restricted;

  return obj;
}

TEST(Liquid, contacts) {

  std::string str = 
    " There are {{ contacts.length }} contacts."
    " {% for c in contacts %}                  "
    "   {% if c.private %}                     "
    " This contact is private.                 "
    "   {% else %}                             "
    " Contact {{ c['name'] }} ({{ c.age }}).   "
    "   {% endif %}                            "
    " {% endfor %}                             ";

  liquid::Template tmplt = liquid::parse(str);

  json::Array contacts;
  contacts.push(createContact("Bob", 19));
  contacts.push(createContact("Alice", 18));
  contacts.push(createContact("Eve", 22, true));

  json::Object data = {};
  data["contacts"] = contacts;
  std::string result = tmplt.render(data);

  {
    size_t pos = result.find("Eve");
    ASSERT_EQ(pos, std::string::npos);
  }

  {
    size_t pos = result.find("Alice");
    ASSERT_NE(pos, std::string::npos);
  }

  {
    size_t pos = result.find("19");
    ASSERT_NE(pos, std::string::npos);
  }
}

TEST(Liquid, comments) {

  std::string str =
    "Hello there!{% comment General Kenobi ! %}\n"
    "  You're a bold 1.";

  liquid::Template tmplt = liquid::parse(str);

  std::string result = tmplt.render(json::Object{});

  ASSERT_EQ(result, "Hello there!\n  You're a bold 1.");
}

TEST(Liquid, eject) {

  std::string str = "{% for n in numbers %}{% if n == 5 %}{% eject %}{% endif %}{{ n }}{% endfor %}bye";

  liquid::Template tmplt = liquid::parse(str);

  json::Array numbers;
  numbers.push(1);
  numbers.push(2);
  numbers.push(3);
  numbers.push(4);
  numbers.push(5);
  numbers.push(6);
  numbers.push(7);
  json::Object data = {};
  data["numbers"] = numbers;
  std::string result = tmplt.render(data);

  ASSERT_EQ(result, "1234");
}

TEST(Liquid, discard) {

  std::string str = "{% for n in numbers %}{% if n == 5 %}{% discard %}{% endif %}{{ n }}{% endfor %}bye";

  liquid::Template tmplt = liquid::parse(str);

  json::Array numbers;
  numbers.push(1);
  numbers.push(2);
  numbers.push(3);
  numbers.push(4);
  numbers.push(5);
  numbers.push(6);
  numbers.push(7);
  json::Object data = {};
  data["numbers"] = numbers;
  std::string result = tmplt.render(data);

  ASSERT_EQ(result, "");
}

#include "liquid/renderer.h"
#include "liquid/filter.h"

#include <cctype>

class CustomRenderer : public liquid::Renderer
{
public:

  json::Json applyFilter(const std::string& name, const json::Json& object, const std::vector<json::Json>& args) override;

  json::Serializer serializer;
};

static std::string filter_uppercase(const std::string& str)
{
  std::string result = str;
  for (char& c : result)
    c = std::toupper(c);
  return result;
}

static int filter_mul(int x, int y)
{
  return x * y;
}

static std::string filter_substr(std::string str, int pos, int count)
{
  return str.substr(pos, count);
}

json::Json CustomRenderer::applyFilter(const std::string& name, const json::Json& object, const std::vector<json::Json>& args)
{
  if(name == "uppercase")
    return liquid::filters::apply(filter_uppercase, object, args, serializer);
  else if(name == "mul")
    return liquid::filters::apply(filter_mul, object, args, serializer);
  else if (name == "substr")
    return liquid::filters::apply(filter_substr, object, args, serializer);

  return Renderer::applyFilter(name, object, args);
}

TEST(Liquid, filters) {

  std::string str = "Hello {{ 'Bob2' | substr: 0, 3 | uppercase }}, your account now contains {{ money | mul: 2 }} dollars.";

  liquid::Template tmplt = liquid::parse(str);

  json::Object data = {};
  data["money"] = 5;
  std::string result = tmplt.render<CustomRenderer>(data);

  ASSERT_EQ(result, "Hello BOB, your account now contains 10 dollars.");
}

TEST(Liquid, array_filters) {

  std::string str = "{% assign names = persons | map: 'name' %}{{ names | first }} {{ names | last }} {{ names | join: '|' }}";

  liquid::Template tmplt = liquid::parse(str);

  json::Array persons;

  {
    json::Json spongebob;
    spongebob["name"] = "SpongeBob";
    spongebob["surname"] = "SquarePants";
    persons.push(spongebob);
  }

  {
    json::Json patrick;
    patrick["name"] = "Patrick";
    patrick["surname"] = "Star";
    persons.push(patrick);
  }

  {
    json::Json squidward;
    squidward["name"] = "Squidward";
    squidward["surname"] = "Tentacles";
    persons.push(squidward);
  }

  json::Object data = {};
  data["persons"] = persons;
  std::string result = tmplt.render(data);

  ASSERT_EQ(result, "SpongeBob Squidward SpongeBob|Patrick|Squidward");
}

TEST(Liquid, manual_whitespace_control) {

  {
    std::string str =
      "{% assign username = 'John G.Chalmers - Smith' %}\n"
      "{% if username and username.size > 10 %}\n"
      "  Wow, {{ username }}, you have a long name!\n"
      "{% else %}\n"
      "  Hello there!\n"
      "{% endif %}";

    liquid::Template tmplt = liquid::parse(str);

    json::Object data = {};
    std::string result = tmplt.render(data);

    ASSERT_EQ(result, "\n\n  Wow, John G.Chalmers - Smith, you have a long name!\n");

    tmplt.stripWhitespacesAtTag();
    result = tmplt.render(data);

    ASSERT_EQ(result, "Wow, John G.Chalmers - Smith, you have a long name!\n");
  }

  {
    std::string str =
      "{% for p in people %}\n"
      "  - {{ p }}\n"
      "{% endfor %}\n"
      "{% for p in people %}\n"
      "{{''}}  - {{ p }}\n"
      "{% endfor %}\n";

    liquid::Template tmplt = liquid::parse(str);

    json::Object data = {};
    data["people"] = json::Array();
    data["people"].push("Bob");
    data["people"].push("Alice");

    tmplt.stripWhitespacesAtTag();
    std::string result = tmplt.render(data);

    ASSERT_EQ(result, "- Bob\n- Alice\n  - Bob\n  - Alice\n");
  }
}

TEST(Liquid, include) {

  liquid::Renderer renderer;

  {
    std::string str = "{% if include %}{{ include.text }}{% endif %}";

    liquid::Template tmplt = liquid::parse(str);

    json::Object data = {};
    std::string result = tmplt.render(data);

    ASSERT_EQ(result, "");

    renderer.templates()["print_name"] = tmplt;
  }

  {
    std::string str =
      "Hello {% include print_name with text=name %}!";

    liquid::Template tmplt = liquid::parse(str);

    json::Object data = {};
    data["name"] = "World";
    std::string result = renderer.render(tmplt, data);

    ASSERT_EQ(result, "Hello World!");
  }

  {
    std::string str = "{% if include.number > 9 %}{% assign result = false parent_scope %}{% else %}{% assign result = true parent_scope %}{% endif %}";

    liquid::Template tmplt = liquid::parse(str);

    renderer.templates()["is_digit"] = tmplt;
  }

  {
    std::string str =
      "{% include is_digit with number = 10 %}{{result}}{% include is_digit with number = 9 %}{{result}}";

    liquid::Template tmplt = liquid::parse(str);

    json::Object data = {};
    std::string result = renderer.render(tmplt, data);

    ASSERT_EQ(result, "falsetrue");
  }
}

TEST(Liquid, error) {

  std::string str = "{% assign age = 20 %}{{ age.bad_property }}";

  liquid::Template tmplt = liquid::parse(str);

  liquid::Renderer renderer;
  json::Object data = {};
  std::string result = renderer.render(tmplt, data);

  ASSERT_TRUE(result.find("{!") == 0);

  ASSERT_TRUE(!renderer.errors().empty());

  ASSERT_EQ(tmplt.getLine(renderer.errors().front().offset), "{% assign age = 20 %}{{ age.bad_property }}");
}
