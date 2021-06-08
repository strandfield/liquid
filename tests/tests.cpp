// Copyright (C) 2019-2021 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "liquid/liquid.h"

#include "liquid/renderer.h"

#include <gtest/gtest.h>

TEST(Liquid, hello) {

  std::string str = "Hello {{ name }}!";

  liquid::Template tmplt = liquid::parse(str);

  liquid::Map data = { {"name", "Alice"} };
  std::string result = tmplt.render(data);

  ASSERT_EQ(result, "Hello Alice!");
}

TEST(Liquid, greetings) {

  std::string str = "Hi! My name is {{ name }} and I am {{ age }} years old.";

  liquid::Template tmplt = liquid::parse(str);

  liquid::Map data = { {"name", "Bob"}, {"age", 18} };
  std::string result = tmplt.render(data);

  ASSERT_EQ(result, "Hi! My name is Bob and I am 18 years old.");
}

TEST(Liquid, fruits) {

  std::string str = "I love {% for fruit in fruits %}{{ fruit }}{% if forloop.last == false %}, {% endif %}{% endfor %}!";

  liquid::Template tmplt = liquid::parse(str);

  liquid::Array fruits;
  fruits.push("apples");
  fruits.push("strawberries");
  fruits.push("bananas");
  liquid::Map data = { {"fruits", fruits} };
  std::string result = tmplt.render(data);

  ASSERT_EQ(result, "I love apples, strawberries, bananas!");
}

TEST(Liquid, controlflow) {

  std::string str = "{% for n in numbers %}{% if n > 10 %}{% break %}{% elsif n <= 3 %}{% continue %}{% endif %}{{ n }}{% endfor %}";

  liquid::Template tmplt = liquid::parse(str);

  liquid::Array numbers;
  numbers.push(1);
  numbers.push(2);
  numbers.push(5);
  numbers.push(4);
  numbers.push(12);
  numbers.push(10);
  liquid::Map data = { {"numbers", numbers} };
  std::string result = tmplt.render(data);

  ASSERT_EQ(result, "54");
}

TEST(Liquid, assign_global) {

  std::string str = "{% assign foo = ['bar', 0] global %}";

  liquid::Template tmplt = liquid::parse(str);

  liquid::Renderer renderer;
  liquid::Map data;
  renderer.render(tmplt, data);

  ASSERT_EQ(data["foo"].at(0).as<std::string>(), "bar");
  ASSERT_EQ(data["foo"].at(1).as<int>(), 0);
}

TEST(Liquid, logic) {

  std::string str = "{% if x or y %}1{% endif %}"
    "{% if a >= b %}2{% endif %}"
    "{% if a and b %}3{% endif %}"
    "{% if a != b and not w %}4{% endif %}"
    "{% if a xor y %}5{% endif %}";

  liquid::Template tmplt = liquid::parse(str);

  liquid::Map data = {};
  data["x"] = true;
  data["y"] = false;
  data["a"] = 5;
  data["b"] = 10;
  std::string result = tmplt.render(data);

  ASSERT_EQ(result, "1345");
}

TEST(Liquid, math) {

  std::string str = "{% if 4 + 4 == 8 xor 2 - 2 %}1{% endif %}"
    "{% if 0 == 9 - 9 / 2 * 2 %}2{% endif %}"
    "{% if 8 - 8 / 2 * 2 < 1 %}3{% endif %}";

  liquid::Template tmplt = liquid::parse(str);

  liquid::Map data = {};
  std::string result = tmplt.render(data);

  ASSERT_EQ(result, "13");
}

TEST(Liquid, arrayaccess) {

  std::string str = "{% assign index = 1 %}{{ numbers[index] }}";

  liquid::Template tmplt = liquid::parse(str);

  liquid::Array numbers;
  numbers.push(1);
  numbers.push(2);
  numbers.push(3);
  liquid::Map data = {};
  data["numbers"] = numbers;
  std::string result = tmplt.render(data);

  ASSERT_EQ(result, "2");
}

static liquid::Map createContact(const std::string& name, int age, bool restricted = false)
{
  liquid::Map obj = {};
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

  liquid::Array contacts;
  contacts.push(createContact("Bob", 19));
  contacts.push(createContact("Alice", 18));
  contacts.push(createContact("Eve", 22, true));

  liquid::Map data = {};
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

  std::string result = tmplt.render(liquid::Map{});

  ASSERT_EQ(result, "Hello there!\n  You're a bold 1.");
}

TEST(Liquid, eject) {

  std::string str = "{% for n in numbers %}{% if n == 5 %}{% eject %}{% endif %}{{ n }}{% endfor %}bye";

  liquid::Template tmplt = liquid::parse(str);

  liquid::Array numbers;
  numbers.push(1);
  numbers.push(2);
  numbers.push(3);
  numbers.push(4);
  numbers.push(5);
  numbers.push(6);
  numbers.push(7);
  liquid::Map data = {};
  data["numbers"] = numbers;
  std::string result = tmplt.render(data);

  ASSERT_EQ(result, "1234");
}

TEST(Liquid, discard) {

  std::string str = "{% for n in numbers %}{% if n == 5 %}{% discard %}{% endif %}{{ n }}{% endfor %}bye";

  liquid::Template tmplt = liquid::parse(str);

  liquid::Array numbers;
  numbers.push(1);
  numbers.push(2);
  numbers.push(3);
  numbers.push(4);
  numbers.push(5);
  numbers.push(6);
  numbers.push(7);
  liquid::Map data = {};
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

  liquid::Value applyFilter(const std::string& name, const liquid::Value& object, const std::vector<liquid::Value>& args) override;
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

liquid::Value CustomRenderer::applyFilter(const std::string& name, const liquid::Value& object, const std::vector<liquid::Value>& args)
{
  if(name == "uppercase")
    return liquid::filters::apply(filter_uppercase, object, args);
  else if(name == "mul")
    return liquid::filters::apply(filter_mul, object, args);
  else if (name == "substr")
    return liquid::filters::apply(filter_substr, object, args);

  return Renderer::applyFilter(name, object, args);
}

TEST(Liquid, filters) {

  std::string str = "Hello {{ 'Bob2' | substr: 0, 3 | uppercase }}, your account now contains {{ money | mul: 2 }} dollars.";

  liquid::Template tmplt = liquid::parse(str);

  liquid::Map data = {};
  data["money"] = 5;
  std::string result = tmplt.render<CustomRenderer>(data);

  ASSERT_EQ(result, "Hello BOB, your account now contains 10 dollars.");
}

TEST(Liquid, array_push_pop) {

  std::string str = 
    "{% assign list = [] %}"
    "{% assign list = list | push: '1' %}"
    "{% assign list = list | push: 3 %}"
    "{% assign list = list | pop | push: '2' | push: '3' %}"
    "{{ list | join: ',' }}";
   
  liquid::Template tmplt = liquid::parse(str);

  liquid::Map data = {};
  std::string result = tmplt.render(data);

  ASSERT_EQ(result, "1,2,3");
}

TEST(Liquid, array_filters) {

  std::string str = "{% assign names = persons | map: 'name' %}{{ names | first }} {{ names | last }} {{ ['Plankton'] | concat: names | join: '|' }}";

  liquid::Template tmplt = liquid::parse(str);

  liquid::Array persons;

  {
    liquid::Map spongebob;
    spongebob["name"] = "SpongeBob";
    spongebob["surname"] = "SquarePants";
    persons.push(spongebob);
  }

  {
    liquid::Map patrick;
    patrick["name"] = "Patrick";
    patrick["surname"] = "Star";
    persons.push(patrick);
  }

  {
    liquid::Map squidward;
    squidward["name"] = "Squidward";
    squidward["surname"] = "Tentacles";
    persons.push(squidward);
  }

  liquid::Map data = {};
  data["persons"] = persons;
  std::string result = tmplt.render(data);

  ASSERT_EQ(result, "SpongeBob Squidward Plankton|SpongeBob|Patrick|Squidward");
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

    liquid::Map data = {};
    std::string result = tmplt.render(data);

    ASSERT_EQ(result, "\n\n  Wow, John G.Chalmers - Smith, you have a long name!\n");

    tmplt.stripWhitespacesAtTag();
    result = tmplt.render(data);

    ASSERT_EQ(result, "Wow, John G.Chalmers - Smith, you have a long name!\n");
  }

  {
    std::string str =
      "{% for p in people %}\n"
      "  - {{ p }}{% newline %}\n"
      "{% endfor %}\n"
      "{% for p in people %}\n"
      "{{''}}  - {{ p }}{% comment %}\n"
      "{% endfor %}\n";

    liquid::Template tmplt = liquid::parse(str);

    liquid::Map data = {};
    liquid::Array people;
    data["people"] = liquid::Array({"Bob", "Alice"});

    tmplt.skipWhitespacesAfterTag();
    std::string result = tmplt.render(data);

    ASSERT_EQ(result, "- Bob\n- Alice\n  - Bob  - Alice");
  }
}

TEST(Liquid, include) {

  liquid::Renderer renderer;

  {
    std::string str = "{% if include %}{{ include.text }}{% endif %}";

    liquid::Template tmplt = liquid::parse(str);

    liquid::Map data = {};
    std::string result = tmplt.render(data);

    ASSERT_EQ(result, "");

    renderer.templates()["print_name"] = tmplt;
  }

  {
    std::string str =
      "Hello {% include print_name with text=name %}!";

    liquid::Template tmplt = liquid::parse(str);

    liquid::Map data = {};
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

    liquid::Map data = {};
    std::string result = renderer.render(tmplt, data);

    ASSERT_EQ(result, "falsetrue");
  }
}

TEST(Liquid, capture) {

  std::string str = "{% assign n = 35 %}{% capture text %}You owe me {{n}}$!{% endcapture %}{% assign text = text global %}";

  liquid::Template tmplt = liquid::parse(str);

  liquid::Map data = {};
  std::string result = tmplt.render(data);

  ASSERT_TRUE(result.empty());
  ASSERT_EQ(data["text"].as<std::string>(), "You owe me 35$!");
}

TEST(Liquid, error) {

  std::string str = "{% assign age = 20 %}{{ age.bad_property }}";

  liquid::Template tmplt = liquid::parse(str);

  liquid::Renderer renderer;
  liquid::Map data = {};
  std::string result = renderer.render(tmplt, data);

  ASSERT_TRUE(result.find("{!") == 0);

  ASSERT_TRUE(!renderer.errors().empty());

  ASSERT_EQ(tmplt.getLine(renderer.errors().front().offset), "{% assign age = 20 %}{{ age.bad_property }}");
}
