#pragma once

#include <cstdint>
#include <memory>

#include "cpp/inter/expression.hpp"
#include "cpp/lexer/type.hpp"
#include "cpp/lexer/word.hpp"

namespace inter {
class Identifier : public Expression {
 public:
  static std::shared_ptr<Identifier> create(
      std::shared_ptr<lexer::Word> identifier,
      std::shared_ptr<symbols::Type> type,
      std::uint32_t offset
  );

  Identifier(std::shared_ptr<lexer::Word> identifier, std::shared_ptr<symbols::Type> type, std::uint32_t offset);
  ~Identifier();

  std::uint32_t offset() const;

 private:
  std::uint32_t offset_;
};

inline std::shared_ptr<Identifier> Identifier::create(
    std::shared_ptr<lexer::Word> identifier,
    std::shared_ptr<symbols::Type> type,
    std::uint32_t offset
) {
  return std::make_shared<Identifier>(identifier, type, offset);
}

inline Identifier::Identifier(
    std::shared_ptr<lexer::Word> identifier,
    std::shared_ptr<symbols::Type> type,
    std::uint32_t offset
)
    : Expression(identifier, type), offset_(offset) {
}

inline Identifier::~Identifier() {
}

inline std::uint32_t Identifier::offset() const {
  return offset_;
}
}  // namespace inter
