#include "cpp/dragon/lexer/token.hh"

#include <sstream>

namespace dragon {
namespace lexer {
Token::Token(const Token &other) : tag_(other.tag_), value_(other.value_) {}
Token::Token(Token &&other) : tag_(other.tag_), value_(std::move(other.value_)) {}
Token::Token(std::uint32_t tag) : tag_(tag), value_() {}
Token::Token(std::uint32_t tag, std::int64_t val) : tag_(tag), value_(val) {}
Token::Token(std::uint32_t tag, double val) : tag_(tag), value_(val) {}
Token::Token(std::uint32_t tag, const std::string &val) : tag_(tag), value_(std::move(val)) {}

const Token &Token::operator=(const Token &other) {
  if (this == &other) {
    return *this;
  }
  tag_ = other.tag_;
  value_ = other.value_;
  return *this;
}

bool Token::operator==(const Token &other) const {
  return tag_ == other.tag_ && value_ == other.value_;
}

bool Token::operator!=(const Token &other) const {
  return !(*this == other);
}

std::string Token::to_string() const {
  std::stringstream ss;
  if (std::holds_alternative<std::string>(value_)) {
    ss << str();
  } else if (std::holds_alternative<std::int64_t>(value_)) {
    ss << num();
  } else if (std::holds_alternative<double>(value_)) {
    ss << real();
  } else {
    ss << static_cast<char>(tag_);
  }
  return ss.str();
}

namespace operators {
const Token kAndWord(::dragon::lexer::tag::kAnd, "&&");
const Token kOrWord(::dragon::lexer::tag::kOr, "||");
const Token kEqual(::dragon::lexer::tag::kEqual, "==");
const Token kNotEqual(::dragon::lexer::tag::kNotEqual, "!=");
const Token kLessEqual(::dragon::lexer::tag::kLessEqual, "<=");
const Token kGreaterEqual(::dragon::lexer::tag::kGreaterEqual, ">=");
const Token kMinus(::dragon::lexer::tag::kMinus, "-");
const Token kTrueWord(::dragon::lexer::tag::kTrue, "true");
const Token kFalseWord(::dragon::lexer::tag::kFalse, "false");
const Token kTemp(::dragon::lexer::tag::kTemp, "t");
const Token kAccess(::dragon::lexer::tag::kIndex, "[]");
} // namespace operators

namespace types {
const Token kInt(::dragon::lexer::tag::kBasic, "int");
const Token kFloat(::dragon::lexer::tag::kBasic, "float");
const Token kBool(::dragon::lexer::tag::kBasic, "bool");
const Token kChar(::dragon::lexer::tag::kBasic, "char");
} // namespace types

} // namespace lexer
} // namespace dragon
