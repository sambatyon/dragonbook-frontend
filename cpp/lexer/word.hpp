#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "cpp/lexer/token.hpp"

namespace lexer {
class Word : public Token {
 public:
  static std::shared_ptr<Word> and_word;
  static std::shared_ptr<Word> or_word;
  static std::shared_ptr<Word> equal;
  static std::shared_ptr<Word> not_equal;
  static std::shared_ptr<Word> less_equal;
  static std::shared_ptr<Word> greater_equal;
  static std::shared_ptr<Word> minus;
  static std::shared_ptr<Word> true_word;
  static std::shared_ptr<Word> false_word;
  static std::shared_ptr<Word> temp;

  static std::shared_ptr<Word> create(const std::string &lexeme, const std::uint32_t &tag);
  Word(const std::string &lexeme, const std::uint32_t &tag);
  virtual ~Word();

  std::string lexeme() const;
  virtual std::string to_string() const override;

 private:
  std::string lexeme_;
};

inline std::shared_ptr<Word> Word::create(const std::string &lexeme, const std::uint32_t &tag) {
  return std::make_shared<Word>(lexeme, tag);
}

inline Word::Word(const std::string &lexeme, const std::uint32_t &tag) : Token(tag), lexeme_(lexeme) {
}

inline Word::~Word() {
}

inline std::string Word::lexeme() const {
  return lexeme_;
}

inline std::string Word::to_string() const {
  return lexeme();
}
}  // namespace lexer
