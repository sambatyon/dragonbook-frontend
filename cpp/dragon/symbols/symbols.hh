#pragma once

#include "cpp/dragon/lexer/token.hh"

#include <cstdint>
#include <memory>

namespace dragon {
namespace symbols {
class Type {
 public:
  static const Type kNumber;
  static const Type kReal;
  static const Type kCharacter;
  static const Type kBoolean;

  static bool is_numeric(const Type *const typ);
  static std::unique_ptr<Type> max(const Type *const left, const Type *const right);

  Type(const lexer::Token &token, std::uint8_t width);
  virtual ~Type() {}

  bool operator==(const Type &other) const;
  bool operator!=(const Type &other) const;

  virtual std::string to_string() const { return token_.to_string(); }
  virtual std::unique_ptr<Type> clone() const;

  std::uint8_t width() const { return width_; }
  lexer::Token token() const { return token_; }

 protected:
  lexer::Token token_;
  std::uint8_t width_;

 private:
  Type() = delete;
  Type(const Type &) = delete;
  Type(Type &&) = delete;
  const Type &operator=(const Type &) = delete;
};

class Array: public Type {
 public:
  Array(std::unique_ptr<Type> of, std::uint32_t size);

  std::uint32_t size() const { return size_; }
  const Type *const of() const { return of_.get(); }

  virtual std::string to_string() const override;
  virtual std::unique_ptr<Type> clone() const override;

 private:
  std::unique_ptr<Type> of_;
  std::uint32_t size_;

  Array() = delete;
  Array(const Type &) = delete;
  Array(Type &&) = delete;
  const Array &operator=(const Type &) = delete;
};
} // namespace symbols
} // namespace dragon
