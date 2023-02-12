#pragma once

#include "cpp/inter/expression.hpp"
#include "cpp/inter/statement.hpp"


namespace inter {
class If : public Statement {
 public:
  static std::shared_ptr<If> create(std::shared_ptr<Expression> expr, std::shared_ptr<Statement> statement);

  If(std::shared_ptr<Expression> expr, std::shared_ptr<Statement> statement);
  ~If();

  void gen(std::stringstream &ss, std::uint32_t b, std::uint32_t a) override;

  std::shared_ptr<Expression> expr() const;
  std::shared_ptr<Statement> statement() const;

 private:
  std::shared_ptr<Expression> expr_;
  std::shared_ptr<Statement> statement_;
};

inline std::shared_ptr<If> If::create(std::shared_ptr<Expression> expr, std::shared_ptr<Statement> statement) {
  return std::make_shared<If>(expr, statement);
}

inline If::If(std::shared_ptr<Expression> expr, std::shared_ptr<Statement> statement)
    : expr_(expr), statement_(statement) {
  if (expr_->type() != symbols::Type::boolean)
    expr->error("Boolean required in if");
}

inline If::~If() {
}

inline void If::gen(std::stringstream &ss, std::uint32_t b, std::uint32_t a) {
  auto label = new_label();
  expr_->jumping(ss, 0, a);
  emit_label(ss, label);
  statement_->gen(ss, label, a);
}

inline std::shared_ptr<Expression> If::expr() const {
  return expr_;
}

inline std::shared_ptr<Statement> If::statement() const {
  return statement_;
}
}  // namespace inter
