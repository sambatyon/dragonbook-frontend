#include "cpp/dragon/symbols/symbols.hh"

#include <sstream>

namespace dragon {
namespace symbols {
const Type Type::kNumber(lexer::types::kInt, 4);
const Type Type::kReal(lexer::types::kFloat, 8);
const Type Type::kCharacter(lexer::types::kChar, 1);
const Type Type::kBoolean(lexer::types::kBool, 1);

bool Type::is_numeric(const Type *const typ) {
  return *typ == Type::kNumber || *typ == Type::kReal || *typ == Type::kCharacter;
}
std::unique_ptr<Type> Type::max(const Type *const left, const Type *const right) {
  if (!is_numeric(left) && !is_numeric(right)) {
    return std::unique_ptr<Type>();
  }
  if (*left == kReal || *right == kReal) {
    return kReal.clone();
  }
  if (*left == kNumber || *right == kNumber) {
    return kReal.clone();
  }
  return kCharacter.clone();
}

Type::Type(const lexer::Token &token, std::uint8_t width)
  : token_(std::move(token)), width_(width) {}

bool Type::operator==(const Type &other) const {
  return token_ == other.token_ && width_ == other.width_;
}
bool Type::operator!=(const Type &other) const {
  return !(*this == other);
}
std::unique_ptr<Type> Type::clone() const {
  return std::make_unique<Type>(token_, width_);
}

Array::Array(std::unique_ptr<Type> of, std::uint32_t size)
: Type(lexer::Token(lexer::tag::kIndex, "[]"), size * of->width()), of_(std::move(of)), size_(size) {}

std::unique_ptr<Type> Array::clone() const {
  return std::make_unique<Array>(of_->clone(), size_);
}

std::string Array::to_string() const {
  std::stringstream ss;
  ss << '[' << size_ << ']' << of_->to_string();
  return ss.str();
}
} // namespace symbols
} // namespace dragon
