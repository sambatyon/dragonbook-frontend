#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>

#include "cpp/dragon/lexer/token.hh"

namespace dragon {
namespace lexer {
class Lexer {
 public:
  Lexer(std::istream &source);
  ~Lexer();

  Token scan();

 protected:
  void readch();
  bool readch(char c);

  void reserve(const Token &word);
 private:
  std::istream &source_;
  std::unordered_map<std::string, Token> words_;
  char peek_;
  std::uint32_t current_line_;
};

inline void Lexer::readch() {
  source_.get(peek_);
  if (source_.eof()) {
    peek_ = '\0';
  }
}

inline void Lexer::reserve(const Token &word) {
  words_.emplace(std::make_pair(word.str(), std::move(word)));
}
} // namespace lexer
} // namespace dragon
