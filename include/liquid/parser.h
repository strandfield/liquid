// Copyright (C) 2019 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef LIQUID_PARSER_H
#define LIQUID_PARSER_H

#include "liquid/objects.h"
#include "liquid/template.h"

#include <set>
#include <vector>

namespace liquid
{

using Char = StringBackend::char_type;
using StringRef = StringBackend::string_view_type;

struct LIQUID_API Token
{
  enum Kind {
    Identifier,
    Dot,
    LeftBracket,
    RightBracket,
    Operator,
    Pipe,
    Colon,
    Comma,
    BooleanLiteral,
    IntegerLiteral,
    StringLiteral,
    Nil,
  };

  Kind kind;
  std::string* text_;
  int offset_;
  int length_;

  StringRef toStringRef() const;
  std::string toString() const;

  bool operator==(const char *str) const;
};

class LIQUID_API Tokenizer
{
public:
  Tokenizer();

  std::vector<Token> tokenize(const std::string& str);

protected:
  Token read();
  inline Char nextChar() const { return peekChar(); }
  bool atEnd() const;

  int position() const { return mPosition; }
  const std::string& input() const { return mInput; }

protected:
  friend struct TokenProducer;

  Char readChar();
  Char peekChar() const;
  void seek(int pos);
  bool isPunctuator(const Char& c) const;
  bool readSpaces();
  Token produce(Token::Kind k);

  Token readIdentifier();
  Token readIntegerLiteral();
  Token readStringLiteral();
  Token readOperator();

private:
  int mPosition;
  int mStartPos;
  std::string mInput;
  std::set<Char> mPunctuators;
};


class LIQUID_API Parser
{
public:
  Parser();
  ~Parser();

  std::vector<std::shared_ptr<liquid::templates::Node>> parse(const std::string& document);

protected:
  void readNode();
  virtual void dispatchNode(std::shared_ptr<liquid::templates::Node> n);
  inline bool atEnd() const { return mPosition == mDocument.length(); }

  virtual void processTag(std::vector<Token> & tokens);
  std::shared_ptr<liquid::Object> parseObject(std::vector<Token> & tokens);

  inline Tokenizer & tokenizer() { return mTokenizer;  }
  inline int position() const { return mPosition; }
  inline const std::string& document() const { return mDocument; }

protected:
  void process_tag_assign(std::vector<Token> & tokens);
  void process_tag_if(std::vector<Token> & tokens);
  void process_tag_elsif(std::vector<Token> & tokens);
  void process_tag_else(std::vector<Token> & tokens);
  void process_tag_endif(std::vector<Token> & tokens);
  void process_tag_for(std::vector<Token> & tokens);
  void process_tag_break(std::vector<Token> & tokens);
  void process_tag_continue(std::vector<Token> & tokens);
  void process_tag_endfor(std::vector<Token> & tokens);

protected:
  const std::vector<std::shared_ptr<liquid::templates::Node>>& stack() const { return mStack; }

private:
  int mPosition;
  std::string mDocument;
  Tokenizer mTokenizer;
  std::vector<std::shared_ptr<liquid::templates::Node>> mNodes;
  std::vector<std::shared_ptr<liquid::templates::Node>> mStack;
};

} // namespace liquid

#endif // LIQUID_PARSER_H
