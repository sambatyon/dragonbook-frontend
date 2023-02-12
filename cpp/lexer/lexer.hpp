#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include "cpp/lexer/token.hpp"
#include "cpp/lexer/word.hpp"

namespace lexer {
class Lexer {
 public:
  static std::uint32_t current_line();  // Eventually, current_line should stop being static
                                        // in order to make the scaner reentrant
  static std::shared_ptr<Lexer> create(std::istream &source);

  Lexer(std::istream &source);
  ~Lexer();

  std::shared_ptr<Token> scan();

 protected:
  void readch();
  bool readch(char c);

  void reserve(std::shared_ptr<Word> word);

 private:
  std::istream &source_;
  std::unordered_map<std::string, std::shared_ptr<Word>> words_;
  char peek_;
  static std::uint32_t current_line_;
};

inline std::uint32_t Lexer::current_line() {
  return current_line_;
}

inline std::shared_ptr<Lexer> Lexer::create(std::istream &source) {
  return std::make_shared<Lexer>(source);
}

inline void Lexer::readch() {
  source_.get(peek_);
  if (source_.eof())
    peek_ = '\0';
}

inline void Lexer::reserve(std::shared_ptr<Word> word) {
  words_.insert(std::make_pair(word->lexeme(), word));
}
}  // namespace lexer
