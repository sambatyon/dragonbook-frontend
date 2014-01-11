#pragma once

#include <inter/expression.hpp>

#include <lexer/word.hpp>
#include <symbols/type.hpp>

#include <cstdint>
#include <memory>

namespace inter {
class Identifier : public Expression {
  public:
    static std::shared_ptr<Identifier> create(std::shared_ptr<lexer::Word> identifier,
                                              std::shared_ptr<symbols::Type> type,
                                              std::uint32_t offset);
    Identifier(std::shared_ptr<lexer::Word> identifier, std::shared_ptr<symbols::Type> type, std::uint32_t offset);
    ~Identifier();

    std::uint32_t offset() const;

  private:
    std::uint32_t offset_;
};

inline
std::shared_ptr<Identifier> Identifier::create(std::shared_ptr<lexer::Word> identifier,
                                               std::shared_ptr<symbols::Type> type,
                                               std::uint32_t offset) {
    return std::make_shared<Identifier>(identifier, type, offset);
}

inline
Identifier::Identifier(std::shared_ptr<lexer::Word> identifier, std::shared_ptr<symbols::Type> type,
                       std::uint32_t offset) : Expression(identifier, type), offset_(offset) {
}

inline
Identifier::~Identifier() {
}

inline
std::uint32_t Identifier::offset() const {
    return offset_;
}
} // namespace inter
