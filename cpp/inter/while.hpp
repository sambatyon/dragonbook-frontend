#pragma once

#include "cpp/inter/expression.hpp"
#include "cpp/inter/statement.hpp"


namespace inter {
class While : public Statement {
 public:
  static std::shared_ptr<While> create();
  static std::shared_ptr<While> create(std::shared_ptr<Expression> expr, std::shared_ptr<Statement> stmt);

  While();
  ~While();

  void init(std::shared_ptr<Expression> expr, std::shared_ptr<Statement> stmt);

  void gen(std::stringstream &ss, std::uint32_t b, std::uint32_t a) override;

  std::shared_ptr<Expression> expr() const;
  std::shared_ptr<Statement> stmt() const;

 private:
  std::shared_ptr<Expression> expr_;
  std::shared_ptr<Statement> stmt_;
};

inline std::shared_ptr<While> While::create() {
  return std::make_shared<While>();
}

inline std::shared_ptr<While> While::create(std::shared_ptr<Expression> expr, std::shared_ptr<Statement> stmt) {
  auto res = std::make_shared<While>();
  res->init(expr, stmt);
  return res;
}

inline While::While() : expr_(), stmt_() {
}

inline While::~While() {
}

inline void While::init(std::shared_ptr<Expression> expr, std::shared_ptr<Statement> stmt) {
  stmt_ = stmt;
  expr_ = expr;
  if (expr->type() != symbols::Type::boolean) {
    expr->error("Boolean required in do");
  }
}

inline void While::gen(std::stringstream &ss, std::uint32_t b, std::uint32_t a) {
  after_ = a;
  expr_->jumping(ss, 0, a);
  auto label = new_label();
  emit_label(ss, label);
  stmt_->gen(ss, label, b);
  std::stringstream lbl;
  lbl << b;
  emit(ss, "goto L" + lbl.str());
}

inline std::shared_ptr<Expression> While::expr() const {
  return expr_;
}

inline std::shared_ptr<Statement> While::stmt() const {
  return stmt_;
}
}  // namespace inter
