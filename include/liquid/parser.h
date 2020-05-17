// Copyright (C) 2019 Vincent Chambrin
// This file is part of the liquid project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef LIQUID_PARSER_H
#define LIQUID_PARSER_H

#include "liquid/errors.h"
#include "liquid/objects.h"
#include "liquid/template.h"

#include <set>
#include <vector>

namespace liquid
{

class LIQUID_API ParserException : public Exception
{
public:
  size_t offset_;
  std::string message_;

public:
  ParserException(size_t off, const std::string& mssg);

  const char* what() const noexcept override;
};

class LIQUID_API StringView
{
public:
  const std::string* text_;
  size_t offset_;
  size_t length_;

public:
  StringView(const StringView&) = default;
  StringView(StringView&&) noexcept = default;
  
  StringView();
  StringView(const std::string* str, size_t off, size_t len);

  std::string::const_iterator begin() const;
  std::string::const_iterator end() const;

  StringView mid(size_t off, size_t len = std::numeric_limits<size_t>::max()) const;

  StringView& operator=(const StringView&) = default;

  char operator[](size_t off) const;
  bool operator==(const char* str) const;
  inline bool operator!=(const char* str) const { return !(*this == str); }
};

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
  StringView text;

  std::string toString() const;

  bool operator==(const char *str) const;
  inline bool operator!=(const char* str) const { return !(*this == str); }
};

class LIQUID_API Tokenizer
{
public:
  Tokenizer();

  std::vector<Token> tokenize(StringView str);

protected:
  Token read();
  inline char nextChar() const { return peekChar(); }
  bool atEnd() const;

  size_t position() const { return mPosition; }

protected:
  friend struct TokenProducer;

  char readChar();
  char peekChar() const;
  void seek(size_t pos);
  bool isPunctuator(char c) const;
  bool readSpaces();
  Token produce(Token::Kind k);

  Token readIdentifier();
  Token readIntegerLiteral();
  Token readStringLiteral();
  Token readOperator();

private:
  size_t mPosition;
  size_t mStartPos;
  StringView mInput;
  std::set<char> mPunctuators;
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
  inline size_t position() const { return mPosition; }
  inline const std::string& document() const { return mDocument; }

protected:
  void process_tag_comment();
  void process_tag_eject();
  void process_tag_discard();
  void process_tag_assign(const Token& keyword, std::vector<Token>& tokens);
  void process_tag_if(const Token& keyword, std::vector<Token>& tokens);
  void process_tag_elsif(const Token& keyword, std::vector<Token>& tokens);
  void process_tag_else(const Token& keyword, std::vector<Token>& tokens);
  void process_tag_endif(const Token& keyword, std::vector<Token>& tokens);
  void process_tag_for(const Token& keyword, std::vector<Token>& tokens);
  void process_tag_break(const Token& keyword, std::vector<Token>& tokens);
  void process_tag_continue(const Token& keyword, std::vector<Token>& tokens);
  void process_tag_endfor(const Token& keyword, std::vector<Token>& tokens);
  void process_tag_include(const Token& keyword, std::vector<Token>& tokens);

protected:
  const std::vector<std::shared_ptr<liquid::templates::Node>>& stack() const { return mStack; }

private:
  size_t mPosition;
  std::string mDocument;
  Tokenizer mTokenizer;
  std::vector<std::shared_ptr<liquid::templates::Node>> mNodes;
  std::vector<std::shared_ptr<liquid::templates::Node>> mStack;
};

} // namespace liquid

#endif // LIQUID_PARSER_H
