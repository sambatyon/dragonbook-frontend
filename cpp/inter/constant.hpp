#pragma once

#include "cpp/inter/expression.hpp"

#include "cpp/lexer/numeric.hpp"

namespace inter {
class Constant : public Expression {
 public:
  static std::shared_ptr<Constant> kTrue;
  static std::shared_ptr<Constant> kFalse;

  static std::shared_ptr<Constant> create(std::shared_ptr<lexer::Token> token, std::shared_ptr<symbols::Type> type);
  static std::shared_ptr<Constant> create(std::int64_t value);

  Constant(std::shared_ptr<lexer::Token> token, std::shared_ptr<symbols::Type> type);
  explicit Constant(std::int64_t value);
  ~Constant();

  void jumping(std::stringstream &ss, std::uint32_t to, std::uint32_t from) override;
};

inline std::shared_ptr<Constant> Constant::create(
    std::shared_ptr<lexer::Token> token,
    std::shared_ptr<symbols::Type> type
) {
  return std::make_shared<Constant>(token, type);
}

inline std::shared_ptr<Constant> Constant::create(std::int64_t value) {
  return std::make_shared<Constant>(value);
}

inline Constant::Constant(std::shared_ptr<lexer::Token> token, std::shared_ptr<symbols::Type> type)
    : Expression(token, type) {
}

inline Constant::Constant(std::int64_t value)
    : Expression(std::make_shared<lexer::Number>(value), symbols::Type::integer) {
}

inline Constant::~Constant() {
}

inline void Constant::jumping(std::stringstream &ss, std::uint32_t to, std::uint32_t from) {
  std::stringstream lbl;
  if (this == kTrue.get() && to != 0) {
    lbl << to;
    emit(ss, "goto L" + lbl.str());
  } else if (this == kFalse.get() && from != 0) {
    lbl << from;
    emit(ss, "goto L" + lbl.str());
  }
}
}  // namespace inter
