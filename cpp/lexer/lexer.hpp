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
  static std::shared_ptr<Lexer> create();

  Lexer();
  ~Lexer();

  std::shared_ptr<Token> scan();

 protected:
  void readch();
  bool readch(char c);

  void reserve(std::shared_ptr<Word> word);

 private:
  std::unordered_map<std::string, std::shared_ptr<Word>> words_;
  char peek_;
  static std::uint32_t current_line_;
};

inline std::uint32_t Lexer::current_line() {
  return current_line_;
}

inline std::shared_ptr<Lexer> Lexer::create() {
  return std::make_shared<Lexer>();
}

inline void Lexer::readch() {
  std::cin.get(peek_);
  if (std::cin.eof())
    peek_ = '\0';
}

inline void Lexer::reserve(std::shared_ptr<Word> word) {
  words_.insert(std::make_pair(word->lexeme(), word));
}
}  // namespace lexer
