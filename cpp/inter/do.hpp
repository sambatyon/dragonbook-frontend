#pragma once

#include "cpp/inter/expression.hpp"
#include "cpp/inter/statement.hpp"


namespace inter {
class Do : public Statement {
 public:
  static std::shared_ptr<Do> create();
  static std::shared_ptr<Do> create(std::shared_ptr<Statement> stmt, std::shared_ptr<Expression> expr);

  Do();
  ~Do();

  void init(std::shared_ptr<Statement> stmt, std::shared_ptr<Expression> expr);

  void gen(std::stringstream &ss, std::uint32_t b, std::uint32_t a) override;

  std::shared_ptr<Expression> expr() const;
  std::shared_ptr<Statement> stmt() const;

 private:
  std::shared_ptr<Expression> expr_;
  std::shared_ptr<Statement> stmt_;
};

inline std::shared_ptr<Do> Do::create() {
  return std::make_shared<Do>();
}

inline std::shared_ptr<Do> Do::create(std::shared_ptr<Statement> stmt, std::shared_ptr<Expression> expr) {
  auto res = std::make_shared<Do>();
  res->init(stmt, expr);
  return res;
}

inline Do::Do() : expr_(), stmt_() {
}

inline Do::~Do() {
}

inline void Do::init(std::shared_ptr<Statement> stmt, std::shared_ptr<Expression> expr) {
  stmt_ = stmt;
  expr_ = expr;
  if (expr->type() != symbols::Type::boolean) {
    expr->error("Boolean required in do");
  }
}

inline void Do::gen(std::stringstream &ss, std::uint32_t b, std::uint32_t a) {
  after_ = a;
  auto label = new_label();
  stmt_->gen(ss, b, label);
  emit_label(ss, label);
  expr_->jumping(ss, b, 0);
}

inline std::shared_ptr<Expression> Do::expr() const {
  return expr_;
}

inline std::shared_ptr<Statement> Do::stmt() const {
  return stmt_;
}
}  // namespace inter
