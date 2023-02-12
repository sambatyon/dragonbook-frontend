#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <sstream>
#include <string>

#include "cpp/inter/expression.hpp"

#include "cpp/lexer/type.hpp"
#include "cpp/lexer/word.hpp"

namespace inter {
class Temporary : public Expression {
 public:
  static std::shared_ptr<Temporary> create(std::shared_ptr<symbols::Type> type);
  Temporary(std::shared_ptr<symbols::Type> type);
  ~Temporary();

  std::uint32_t number() const;

  std::string to_string() const override;

 private:
  static std::atomic_uint32_t count_;
  std::uint32_t number_;
};

inline std::shared_ptr<Temporary> Temporary::create(std::shared_ptr<symbols::Type> type) {
  return std::make_shared<Temporary>(type);
}

inline Temporary::Temporary(std::shared_ptr<symbols::Type> type)
    : Expression(lexer::Word::temp, type), number_(++count_) {
}

inline Temporary::~Temporary() {
}

inline std::uint32_t Temporary::number() const {
  return number_;
}

inline std::string Temporary::to_string() const {
  std::stringstream ss;
  ss << "t" << number_;
  return ss.str();
}
}  // namespace inter
