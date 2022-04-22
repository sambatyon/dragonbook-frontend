#pragma once

#include "cpp/inter/expression.hpp"
#include "cpp/inter/statement.hpp"


namespace inter {
class While : public Statement {
 public:
  static std::shared_ptr<While> create();

  While();
  ~While();

  void init(std::shared_ptr<Expression> expr, std::shared_ptr<Statement> stmt);

  void gen(const std::uint32_t &b, const std::uint32_t &a) override;

  std::shared_ptr<Expression> expr() const;
  std::shared_ptr<Statement> stmt() const;

 private:
  std::shared_ptr<Expression> expr_;
  std::shared_ptr<Statement> stmt_;
};

inline std::shared_ptr<While> While::create() {
  return std::make_shared<While>();
}

inline While::While() : expr_(), stmt_() {
}

inline While::~While() {
}

inline void While::init(std::shared_ptr<Expression> expr, std::shared_ptr<Statement> stmt) {
  stmt_ = stmt;
  expr_ = expr;
  if (expr->type() != symbols::Type::boolean)
    expr->error("Boolean required in do");
}

inline void While::gen(const std::uint32_t &b, const std::uint32_t &a) {
  after_ = a;
  expr_->jumping(0, a);
  auto label = new_label();
  emit_label(label);
  stmt_->gen(label, b);
  std::stringstream ss;
  ss << b;
  emit("goto L" + ss.str());
}

inline std::shared_ptr<Expression> While::expr() const {
  return expr_;
}

inline std::shared_ptr<Statement> While::stmt() const {
  return stmt_;
}
}  // namespace inter
