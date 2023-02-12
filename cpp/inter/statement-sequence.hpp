#pragma once

#include "cpp/inter/statement.hpp"

namespace inter {
class StatementSequence : public Statement {
 public:
  static std::shared_ptr<StatementSequence> create(std::shared_ptr<Statement> first, std::shared_ptr<Statement> second);

  StatementSequence(std::shared_ptr<Statement> first, std::shared_ptr<Statement> second);
  ~StatementSequence();

  std::shared_ptr<Statement> first() const;
  std::shared_ptr<Statement> second() const;

  void gen(std::stringstream &ss, std::uint32_t b, std::uint32_t a) override;

 private:
  std::shared_ptr<Statement> first_;
  std::shared_ptr<Statement> second_;
};

inline std::shared_ptr<StatementSequence> StatementSequence::create(
    std::shared_ptr<Statement> first,
    std::shared_ptr<Statement> second
) {
  return std::make_shared<StatementSequence>(first, second);
}

inline StatementSequence::StatementSequence(std::shared_ptr<Statement> first, std::shared_ptr<Statement> second)
    : first_(first), second_(second) {
}

inline StatementSequence::~StatementSequence() {
}

inline std::shared_ptr<Statement> StatementSequence::first() const {
  return first_;
}

inline std::shared_ptr<Statement> StatementSequence::second() const {
  return second_;
}
}  // namespace inter
