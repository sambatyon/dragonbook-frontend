#pragma once

#include <cstdint>
#include <limits>
#include <memory>
#include <sstream>
#include <string>

namespace lexer {
class Token {
 public:
  static const std::uint32_t kAnd = 256;
  static const std::uint32_t kBasic = 257;
  static const std::uint32_t kBreak = 258;
  static const std::uint32_t kDo = 259;
  static const std::uint32_t kElse = 260;
  static const std::uint32_t kEqual = 261;
  static const std::uint32_t kFalse = 262;
  static const std::uint32_t kGreaterEqual = 263;
  static const std::uint32_t kIdentifier = 264;
  static const std::uint32_t kIf = 265;
  static const std::uint32_t kIndex = 266;
  static const std::uint32_t kLessEqual = 267;
  static const std::uint32_t kMinus = 268;
  static const std::uint32_t kNotEqual = 269;
  static const std::uint32_t kInteger = 270;
  static const std::uint32_t kOr = 271;
  static const std::uint32_t kReal = 272;
  static const std::uint32_t kTemp = 273;
  static const std::uint32_t kTrue = 274;
  static const std::uint32_t kWhile = 275;

  static std::shared_ptr<Token> create(std::uint32_t tag);
  explicit Token(std::uint32_t tag);
  virtual ~Token();

  std::uint32_t tag() const;
  virtual std::string to_string() const;

 private:
  std::uint32_t tag_;
};

inline std::shared_ptr<Token> Token::create(std::uint32_t tag) {
  return std::make_shared<Token>(tag);
}

inline Token::Token(std::uint32_t tag) : tag_(tag) {
}

inline std::uint32_t Token::tag() const {
  return tag_;
}

inline Token::~Token() {
  tag_ = std::numeric_limits<std::uint32_t>::max();
}

inline std::string Token::to_string() const {
  std::stringstream ss;
  ss << static_cast<char>(tag_);
  return ss.str();
}
}  // namespace lexer
