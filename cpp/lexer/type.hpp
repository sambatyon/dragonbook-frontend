#pragma once

#include <memory>
#include <string>

#include "cpp/lexer/word.hpp"

namespace symbols {
class Type : public lexer::Word, public std::enable_shared_from_this<Type> {
 public:
  static std::shared_ptr<Type> integer;
  static std::shared_ptr<Type> real;
  static std::shared_ptr<Type> character;
  static std::shared_ptr<Type> boolean;

  static std::shared_ptr<Type> create(const std::string &lexeme, std::uint32_t tag, const std::size_t &width);
  static Type *max(Type *type_left, Type *type_right);

  Type(const std::string &lexeme, std::uint32_t tag, const std::size_t &width);
  virtual ~Type();

  std::shared_ptr<Type> getptr();
  bool operator==(const Type &other) const;

  std::size_t width() const;
  bool is_numeric() const;

 private:
  std::size_t width_;
};

inline std::shared_ptr<Type> Type::create(
    const std::string &lexeme,
    std::uint32_t tag,
    const std::size_t &width
) {
  return std::make_shared<Type>(lexeme, tag, width);
}

inline Type::Type(const std::string &lexeme, std::uint32_t tag, const std::size_t &width)
    : lexer::Word(lexeme, tag), width_(width) {
}

inline Type::~Type() {
  width_ = 0;
}

inline std::shared_ptr<Type> Type::getptr() {
  return shared_from_this();
}

inline std::size_t Type::width() const {
  return width_;
}
}  // namespace symbols
