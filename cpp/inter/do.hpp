#pragma once

#include "cpp/inter/expression.hpp"
#include "cpp/inter/statement.hpp"


namespace inter {
class Do : public Statement {
 public:
  static std::shared_ptr<Do> create();

  Do();
  ~Do();

  void init(std::shared_ptr<Statement> stmt, std::shared_ptr<Expression> expr);

  void gen(const std::uint32_t &b, const std::uint32_t &a) override;

  std::shared_ptr<Expression> expr() const;
  std::shared_ptr<Statement> stmt() const;

 private:
  std::shared_ptr<Expression> expr_;
  std::shared_ptr<Statement> stmt_;
};

inline std::shared_ptr<Do> Do::create() {
  return std::make_shared<Do>();
}

inline Do::Do() : expr_(), stmt_() {
}

inline Do::~Do() {
}

inline void Do::init(std::shared_ptr<Statement> stmt, std::shared_ptr<Expression> expr) {
  stmt_ = stmt;
  expr_ = expr;
  if (expr->type() != symbols::Type::boolean)
    expr->error("Boolean required in do");
}

inline void Do::gen(const std::uint32_t &b, const std::uint32_t &a) {
  after_ = a;
  auto label = new_label();
  stmt_->gen(b, label);
  emit_label(label);
  expr_->jumping(b, 0);
}

inline std::shared_ptr<Expression> Do::expr() const {
  return expr_;
}

inline std::shared_ptr<Statement> Do::stmt() const {
  return stmt_;
}
}  // namespace inter
