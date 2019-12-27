// Copyright (C) 2019 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "liquid/parser.h"

#include "liquid/tags.h"

#include <algorithm>
#include <map>

namespace vec
{

template<typename T>
T take_first(std::vector<T>& list)
{
  T result = list.front();
  list.erase(list.begin());
  return result;
}

template<typename T>
T take_last(std::vector<T>& list)
{
  T result = list.back();
  list.pop_back();
  return result;
}

template<typename T>
std::vector<T> mid(const std::vector<T>& list, size_t offset, size_t n = std::numeric_limits<size_t>::max())
{
  if (n == std::numeric_limits<size_t>::max())
    n = list.size() - offset;

  return std::vector<T>(list.begin() + offset, list.begin() + offset + n);
}


} // namespace vec

namespace liquid
{

StringView::StringView()
  : text_(nullptr),
  offset_(0),
  length_(0)
{

}

StringView::StringView(const std::string* str, size_t off, size_t len)
  : text_(str),
  offset_(off),
  length_(len)
{

}

std::string::const_iterator StringView::begin() const
{
  return text_->cbegin() + offset_;
}

std::string::const_iterator StringView::end() const
{
  return text_->cbegin() + offset_ + length_;
}

StringView StringView::mid(size_t off, size_t len) const
{
  return StringView(text_, offset_ + off, std::min(len, length_ - off));
}

char StringView::operator[](size_t off) const
{
  return text_->at(offset_ + off);
}

bool StringView::operator==(const char* str) const
{
  size_t i = 0;

  while (str[i] != '\0' && i < length_)
  {
    if (str[i] != text_->at(offset_ + i))
      return false;
    else
      ++i;
  }

  return str[i] == '\0' && i == length_;
}

std::string Token::toString() const
{
  return std::string(text.begin(), text.end());
}

bool Token::operator==(const char *str) const
{
  return text == str;
}

Tokenizer::Tokenizer()
  : mPosition(0), mStartPos(0)
{
  mPunctuators = std::set<char>{ '!', '<', '>', '=' };
}

std::vector<Token> Tokenizer::tokenize(StringView str)
{
  std::vector<Token> result;
  
  mInput = str;
  mPosition = 0;

  readSpaces();

  while (!atEnd())
    result.push_back(read());

  return result;
}

Token Tokenizer::read()
{
  if (atEnd())
  {
    throw std::runtime_error{ "Unexpected end of input" };
  }

  mStartPos = position();

  char c = peekChar();

  if (c == '|')
    return readChar(), produce(Token::Pipe);
  else   if (c == ':')
    return readChar(), produce(Token::Colon);
  else if (c == '.')
    return readChar(), produce(Token::Dot);
  else if (c == ',')
    return readChar(), produce(Token::Comma);
  else if (c == '[')
    return readChar(), produce(Token::LeftBracket);
  else if (c == ']')
    return readChar(), produce(Token::RightBracket);
  else if (StringBackend::is_digit(c))
    return readIntegerLiteral();
  else if (c == '\'' || c == '"')
    return readStringLiteral();
  else if (StringBackend::is_letter(c) || c == '_')
    return readIdentifier();
  else if (isPunctuator(c))
    return readOperator();

  throw std::runtime_error{ "Tokenizer::read() : error" };
}

char Tokenizer::readChar()
{
  return mInput[mPosition++];
}

char Tokenizer::peekChar() const
{
  return mInput[mPosition];
}

void Tokenizer::seek(size_t pos)
{
  mPosition = std::min(pos, mInput.length_);
}

bool Tokenizer::isPunctuator(char c) const
{
  return mPunctuators.find(c) != mPunctuators.end();
}

bool Tokenizer::readSpaces()
{
  const size_t offset = position();

  while (!atEnd() && (StringBackend::is_space(peekChar()) || StringBackend::is_newline(peekChar())))
    readChar();

  return position() != offset;
}

Token Tokenizer::produce(Token::Kind k)
{
  const size_t pos = position();
  readSpaces();
  return Token{ k, mInput.mid(mStartPos, pos-mStartPos) };
}

Token Tokenizer::readIdentifier()
{
  auto is_valid = [](char c) -> bool {
    return StringBackend::is_letter_or_number(c) || c == '_';
  };

  while (!atEnd() && is_valid(peekChar()))
    readChar();

  Token ret = produce(Token::Identifier);

  if (ret == "or" || ret == "and")
    ret.kind = Token::Operator;
  else if (ret == "true" || ret == "false")
    ret.kind = Token::BooleanLiteral;

  return ret;
}

Token Tokenizer::readIntegerLiteral()
{
  while (!atEnd() && StringBackend::is_digit(peekChar()))
    readChar();

  return produce(Token::IntegerLiteral);
}

Token Tokenizer::readStringLiteral()
{
  const char quote = readChar();
  const size_t index = mInput.text_->find(quote, position() + mInput.offset_);
  if (index == std::string::npos)
    throw std::runtime_error{ "Malformed string literal" };
  seek(index - mInput.offset_);
  readChar();
  return produce(Token::StringLiteral);
}

Token Tokenizer::readOperator()
{
  if (peekChar() == '<' || peekChar() == '>' || peekChar() == '=')
  {
    char first_char = readChar();
    if (atEnd())
      return produce(Token::Operator);
    else if (peekChar() == '=')
      return readChar(), produce(Token::Operator);
    else if (first_char == '<' && peekChar() == '>')
      return readChar(), produce(Token::Operator);
  }
  else if (peekChar() == '!')
  {
    readChar();
    if (atEnd())
      return produce(Token::Operator);
    else if (peekChar() == '=')
      return readChar(), produce(Token::Operator);
  }
  else
  {
    readChar();
  }

  return produce(Token::Operator);
}

bool Tokenizer::atEnd() const
{
  return mInput.length_ == mPosition;
}

static std::shared_ptr<liquid::Object> parse_object(std::vector<Token>& tokens);

static std::string parse_object_read_operator(std::vector<Token>& tokens)
{
  Token tok = vec::take_first(tokens);
  if (tok.kind != Token::Operator)
    throw std::runtime_error{ "Bad expression" };
  return tok.toString();
}

static json::Json parse_object_create_literal(const Token& tok)
{
  if (tok.kind == Token::BooleanLiteral)
  {
    return json::Json{ tok == "true" };
  }
  else if (tok.kind == Token::IntegerLiteral)
  {
    return json::Json{ std::stoi(tok.toString()) };
  }
  else if (tok.kind == Token::StringLiteral)
  {
    return json::Json(std::string(tok.text.begin() + 1, tok.text.end() - 1));
  }
  else
  {
    throw std::runtime_error{ "Bad literal" };
  }
}

static json::Json parse_object_read_literal(std::vector<Token>& tokens)
{
  return parse_object_create_literal(vec::take_first(tokens));
}

static std::shared_ptr<liquid::Object> parse_object_read_operand(std::vector<Token>& tokens)
{
  std::shared_ptr<liquid::Object> obj;

  Token tok = vec::take_first(tokens);
  if (tok.kind == Token::Identifier)
    obj = std::make_shared<objects::Variable>(tok.toString());
  else if (tok.kind == Token::BooleanLiteral || tok.kind == Token::IntegerLiteral || tok.kind == Token::StringLiteral)
    obj = std::make_shared<objects::Value>(parse_object_create_literal(tok));
  else
    throw std::runtime_error{ "Bad object" };

  while (!tokens.empty())
  {
    if (tokens.front().kind == Token::Dot)
    {
      vec::take_first(tokens);

      if (tokens.empty() || tokens.front().kind != Token::Identifier)
        throw std::runtime_error{ "Bad object" };

      tok = vec::take_first(tokens);
      obj = std::make_shared<objects::MemberAccess>(obj, tok.toString());
    }
    else if (tokens.front().kind == Token::LeftBracket)
    {
      vec::take_first(tokens);
      std::vector<Token> subtokens;
      while (!tokens.empty() && tokens.front().kind != Token::RightBracket)
      {
        subtokens.push_back(vec::take_first(tokens));
      }

      if (tokens.empty())
        throw std::runtime_error{ "Bad array" };

      vec::take_first(tokens);

      std::shared_ptr<liquid::Object> index = parse_object(subtokens);
      obj = std::make_shared<objects::ArrayAccess>(obj, index);
    }
    else
    {
      break;
    }
  }

  return obj;
}

static std::shared_ptr<liquid::Object> parse_object_build_expr(std::vector<std::shared_ptr<liquid::Object>> operands, std::vector<std::string> operators)
{
  struct OpInfo { objects::BinOp::Operation name; int precedence; };

  static std::map<std::string, OpInfo> map{
    { "or", { objects::BinOp::Or, 4 } },
    { "and", { objects::BinOp::And, 3 } },
    { "!=", { objects::BinOp::Inequal, 2 } },
    { "<>", { objects::BinOp::Inequal, 2 } },
    { "==", { objects::BinOp::Equal, 2 } },
    { "<", { objects::BinOp::Less, 1 } },
    { "<=", { objects::BinOp::Leq, 1 } },
    { ">", { objects::BinOp::Greater, 1 } },
    { ">=", { objects::BinOp::Geq,1 } },
  };

  if (operators.size() == 0)
    return operands.front();

  size_t op_index = operators.size() - 1;
  OpInfo op_info = map.find(operators.back())->second;

  for (size_t i(operators.size() - 1); i-- > 0;)
  {
    auto it = map.find(operators.at(i));
    if (it->second.precedence > op_info.precedence)
      op_index = i, op_info = it->second;
  }

  auto lhs = parse_object_build_expr(vec::mid(operands, 0, op_index + 1), vec::mid(operators, 0, op_index));
  auto rhs = parse_object_build_expr(vec::mid(operands, op_index + 1), vec::mid(operators, op_index + 1));
  return std::make_shared<objects::BinOp>(op_info.name, lhs, rhs);
}

static std::shared_ptr<liquid::Object> parse_object_apply_filter(std::shared_ptr<liquid::Object> obj, std::vector<Token>& tokens)
{
  Token tok = vec::take_first(tokens);
  tok = vec::take_first(tokens);

  std::string name = tok.toString();

  auto ret = std::make_shared<objects::Pipe>(obj, name);

  if (tokens.empty() || tokens.front().kind == Token::Pipe)
    return ret;

  if (tokens.front().kind != Token::Colon)
    throw std::runtime_error{ "Bad filter" };

  vec::take_first(tokens);

  while (!tokens.empty() && tokens.front().kind != Token::Pipe)
  {
    ret->arguments.push_back(parse_object_read_literal(tokens));

    if (tokens.empty() || tokens.front().kind == Token::Pipe)
      break;

    if (tokens.front().kind != Token::Comma)
      throw std::runtime_error{ "Bad filter, expected comma" };

    // read the comma
    vec::take_first(tokens);
  }

  return ret;
}

static std::shared_ptr<liquid::Object> parse_object(std::vector<Token>& tokens)
{
  if (tokens.size() == 1 && tokens.front().kind == Token::Identifier)
    return std::make_shared<objects::Variable>(tokens.front().toString());

  auto obj = parse_object_read_operand(tokens);

  if (tokens.empty())
    return obj;

  std::vector<std::shared_ptr<liquid::Object>> operands;
  operands.push_back(obj);
  std::vector<std::string> operators;

  while (!tokens.empty() && tokens.front().kind != Token::Pipe)
  {
    operators.push_back(parse_object_read_operator(tokens));
    operands.push_back(parse_object_read_operand(tokens));
  }

  obj = parse_object_build_expr(operands, operators);

  /* Apply filters */
  while (!tokens.empty() && tokens.front().kind == Token::Pipe)
  {
    obj = parse_object_apply_filter(obj, tokens);
  }

  return obj;
}

Parser::Parser()
  : mPosition(0)
{

}

Parser::~Parser()
{

}

std::vector<std::shared_ptr<liquid::templates::Node>> Parser::parse(const std::string & document)
{
  mDocument = StringBackend::normalize(document);
  mPosition = 0;
  mNodes.clear();
  mStack.clear();

  while (!atEnd())
    readNode();

  return mNodes;
}

void Parser::readNode()
{
  size_t pos = document().find('{', position());
  
  if (pos == std::string::npos || pos == document().length() - 1)
  {
    auto text = std::string(document().begin() + position(), document().end());
    auto ret = std::make_shared<templates::TextNode>(std::move(text));
    mPosition = document().length();
    dispatchNode(ret);
    return;
  }

  if (pos == position())
  {
    if (document().at(pos + 1) == '{')
    {
      pos = pos + 2;
      const size_t endpos = document().find("}}", pos);

      if(endpos == std::string::npos)
        throw std::runtime_error{ "liquid::Parser::dispatchNode() error" };

      auto tokens = tokenizer().tokenize(StringView(&document(), pos, endpos - pos));
      auto obj = parseObject(tokens);
      dispatchNode(obj);

      mPosition = endpos + 2;
    }
    else if (document().at(static_cast<size_t>(pos) + 1) == '%')
    {
      pos = pos + 2;
      size_t endpos = document().find("%}", pos);

      if (endpos == std::string::npos)
        throw std::runtime_error{ "liquid::Parser::dispatchNode() error" };

      auto tokens = tokenizer().tokenize(StringView(&document(), pos, endpos - pos));
      processTag(tokens);

      mPosition = endpos + 2;

      // Strips new-line after a tag
      //if (!atEnd() && document().at(position()) == '\n')
      //  mPosition += 1;
    }
    else
    {
      auto text = std::string(document().begin() + position(), document().begin() + (pos + 1));
      auto ret = std::make_shared<templates::TextNode>(std::move(text));
      mPosition = pos + 1;
      dispatchNode(ret);
    }
  }
  else
  {
    auto text = std::string(document().begin() + position(), document().begin() + pos);
    auto ret = std::make_shared<templates::TextNode>(std::move(text));
    mPosition = pos;
    dispatchNode(ret);
    return;
  }
}

void Parser::dispatchNode(std::shared_ptr<liquid::templates::Node> n)
{
  if (mStack.empty())
  {
    mNodes.push_back(n);
  }
  else
  {
    auto top = mStack.back();
    if (top->is<tags::For>())
      top->as<tags::For>().body.push_back(n);
    else if (top->is<tags::If>())
      top->as<tags::If>().blocks.back().body.push_back(n);
    else
      throw std::runtime_error{ "liquid::Parser::dispatchNode() error" };
  }
}

void Parser::processTag(std::vector<Token> & tokens)
{
  Token tok = vec::take_first(tokens);

  if (tok == "assign")
    process_tag_assign(tokens);
  else if (tok == "if")
    process_tag_if(tokens);
  else if (tok == "elsif")
    process_tag_elsif(tokens);
  else if (tok == "else")
    process_tag_else(tokens);
  else if (tok == "endif")
    process_tag_endif(tokens);
  else if (tok == "for")
    process_tag_for(tokens);
  else if (tok == "break")
    process_tag_break(tokens);
  else if (tok == "continue")
    process_tag_continue(tokens);
  else if (tok == "endfor")
    process_tag_endfor(tokens);
  else
    throw std::runtime_error{ "Liquid template: unknown tag" };
}

std::shared_ptr<liquid::Object> Parser::parseObject(std::vector<Token> & tokens)
{
  return parse_object(tokens);
}

void Parser::process_tag_assign(std::vector<Token> & tokens)
{
  Token name = vec::take_first(tokens);
  Token eq = vec::take_first(tokens);

  auto expr = parseObject(tokens);

  auto node = std::make_shared<tags::Assign>(name.toString(), expr);
  dispatchNode(node);
}

void Parser::process_tag_if(std::vector<Token> & tokens)
{
  auto cond = parseObject(tokens);
  auto tag = std::make_shared<tags::If>(cond);
  mStack.push_back(tag);
}

void Parser::process_tag_elsif(std::vector<Token> & tokens)
{
  if (mStack.empty() || !mStack.back()->is<tags::If>())
    throw std::runtime_error{ "Bad elsif" };

  tags::If::Block block;
  block.condition = parseObject(tokens);

  mStack.back()->as<tags::If>().blocks.push_back(block);
}

void Parser::process_tag_else(std::vector<Token> & tokens)
{
  if (mStack.empty() || !mStack.back()->is<tags::If>())
    throw std::runtime_error{ "Bad else" };

  tags::If::Block block;
  block.condition = std::make_shared<objects::Value>(json::Json(true));

  mStack.back()->as<tags::If>().blocks.push_back(block);
}

void Parser::process_tag_endif(std::vector<Token> & tokens)
{
  if (mStack.empty() || !mStack.back()->is<tags::If>())
    throw std::runtime_error{ "Bad endif" };

  auto node = vec::take_last(mStack);
  assert(node->is<tags::If>());
  dispatchNode(node);
}

void Parser::process_tag_for(std::vector<Token> & tokens)
{
  std::string name = vec::take_first(tokens).toString();

  std::string in = vec::take_first(tokens).toString();
  if (in != "in")
    throw std::runtime_error{ "Bad for" };

  auto container = parseObject(tokens);

  auto tag = std::make_shared<tags::For>(name, container);
  mStack.push_back(tag);
}

void Parser::process_tag_break(std::vector<Token> & tokens)
{
  dispatchNode(std::make_shared<tags::Break>());
}

void Parser::process_tag_continue(std::vector<Token> & tokens)
{
  dispatchNode(std::make_shared<tags::Continue>());
}

void Parser::process_tag_endfor(std::vector<Token> & tokens)
{
  if (mStack.empty() || !mStack.back()->is<tags::For>())
    throw std::runtime_error{ "Bad endfor" };

  auto node = vec::take_last(mStack);
  assert(node->is<tags::For>());
  dispatchNode(node);
}

} // namespace liquid
