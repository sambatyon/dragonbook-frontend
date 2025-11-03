#pragma once

#include <cstdint>
#include <string>
#include <variant>

namespace dragon {
namespace lexer {
class Token {
 public:
  Token(const Token &other);
  Token(Token &&other);
  explicit Token(std::uint32_t tag);
  Token(std::uint32_t tag, std::int64_t val);
  Token(std::uint32_t tag, double val);
  Token(std::uint32_t tag, const std::string &val);

  std::uint32_t tag() const { return tag_; }
  std::int64_t num() const { return std::get<std::int64_t>(value_); }
  std::int64_t real() const { return std::get<double>(value_); }
  const std::string &str() const { return std::get<std::string>(value_); }

  bool is_word() const { return std::holds_alternative<std::string>(value_); }
  bool is_num() const { return std::holds_alternative<std::int64_t>(value_); }
  bool is_real() const { return std::holds_alternative<double>(value_); }

  std::string to_string() const;

  const Token &operator=(const Token &other);
  bool operator==(const Token &other) const;
  bool operator!=(const Token &other) const;

 private:
  std::uint32_t tag_;
  std::variant<std::monostate, std::int64_t, double, std::string> value_;
};

namespace operators {
extern const Token kAndWord;
extern const Token kOrWord;
extern const Token kEqual;
extern const Token kNotEqual;
extern const Token kLessEqual;
extern const Token kGreaterEqual;
extern const Token kMinus;
extern const Token kTrueWord;
extern const Token kFalseWord;
extern const Token kTemp;
extern const Token kAccess;
} // namespace operators

namespace types {
extern const Token kInt;
extern const Token kFloat;
extern const Token kBool;
extern const Token kChar;
} // namespace types

namespace tag {
const std::uint32_t kAnd = 256;
const std::uint32_t kBasic = 257;
const std::uint32_t kBreak = 258;
const std::uint32_t kDo = 259;
const std::uint32_t kElse = 260;
const std::uint32_t kEqual = 261;
const std::uint32_t kFalse = 262;
const std::uint32_t kGreaterEqual = 263;
const std::uint32_t kIdentifier = 264;
const std::uint32_t kIf = 265;
const std::uint32_t kIndex = 266;
const std::uint32_t kLessEqual = 267;
const std::uint32_t kMinus = 268;
const std::uint32_t kNotEqual = 269;
const std::uint32_t kInteger = 270;
const std::uint32_t kOr = 271;
const std::uint32_t kReal = 272;
const std::uint32_t kTemp = 273;
const std::uint32_t kTrue = 274;
const std::uint32_t kWhile = 275;
} // namespace tag
} // namespace lexer
} // namespace dragon
