#pragma once

#include "cpp/inter/expression.hpp"
#include "cpp/inter/statement.hpp"


namespace inter {
class Else : public Statement {
 public:
  static std::shared_ptr<Else> create(
      std::shared_ptr<Expression> expr,
      std::shared_ptr<Statement> statement_if,
      std::shared_ptr<Statement> statement_else
  );

  Else(
      std::shared_ptr<Expression> expr,
      std::shared_ptr<Statement> statement_if,
      std::shared_ptr<Statement> statement_else
  );
  ~Else();

  void gen(std::stringstream &ss, std::uint32_t b, std::uint32_t a) override;

  std::shared_ptr<Expression> expr() const;
  std::shared_ptr<Statement> statement_if() const;
  std::shared_ptr<Statement> statement_else() const;

 private:
  std::shared_ptr<Expression> expr_;
  std::shared_ptr<Statement> statement_if_;
  std::shared_ptr<Statement> statement_else_;
};

inline std::shared_ptr<Else> Else::create(
    std::shared_ptr<Expression> expr,
    std::shared_ptr<Statement> statement_if,
    std::shared_ptr<Statement> statement_else
) {
  return std::make_shared<Else>(expr, statement_if, statement_else);
}

inline Else::Else(
    std::shared_ptr<Expression> expr,
    std::shared_ptr<Statement> statement_if,
    std::shared_ptr<Statement> statement_else
)
    : expr_(expr), statement_if_(statement_if), statement_else_(statement_else) {
  if (expr_->type() != symbols::Type::boolean)
    expr->error("Boolean required in if");
}

inline Else::~Else() {
}

inline void Else::gen(std::stringstream &ss, std::uint32_t b, std::uint32_t a) {
  auto label_statement_if = new_label();
  auto label_statement_else = new_label();
  expr_->jumping(ss, 0, label_statement_else);
  emit_label(ss, label_statement_if);
  statement_if_->gen(ss, label_statement_if, a);
  std::stringstream lbl;
  lbl << a;
  emit(ss, "goto L" + lbl.str());
  emit_label(ss, label_statement_else);
  statement_else_->gen(ss, label_statement_else, a);
}

inline std::shared_ptr<Expression> Else::expr() const {
  return expr_;
}

inline std::shared_ptr<Statement> Else::statement_if() const {
  return statement_if_;
}

inline std::shared_ptr<Statement> Else::statement_else() const {
  return statement_else_;
}
}  // namespace inter
