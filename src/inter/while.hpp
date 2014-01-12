#pragma once

#include <inter/statement.hpp>
#include <inter/expression.hpp>

namespace inter {
class While : public Statement {
  public:
    static std::shared_ptr<While> create();
    While();
    ~While();

    void init(std::shared_ptr<Statement> stmt, std::shared_ptr<Expression> expr);

    void gen(const std::uint32_t &b, const std::uint32_t &a) override;

    std::shared_ptr<Expression> expr() const;
    std::shared_ptr<Statement> stmt() const;

  private:
    std::shared_ptr<Expression> expr_;
    std::shared_ptr<Statement> stmt_;
};

inline
std::shared_ptr<While> While::create() {
    return std::make_shared<While>();
}

inline
While::While() : expr_(), stmt_() {
}

inline
While::~While() {
}

inline
void While::init(std::shared_ptr<Statement> stmt, std::shared_ptr<Expression> expr) {
    stmt_ = stmt;
    expr_ = expr;
    if (expr->type() != symbols::Type::boolean)
        expr->error("Boolean required in do");
}

inline
void While::gen(const std::uint32_t &b, const std::uint32_t &a) {
    after_ = a;
    expr_->jumping(b, 0);
    auto label = new_label();
    emit_label(label);
    stmt_->gen(label, b);
    std::stringstream ss;
    ss << b;
    emit("goto L" + ss.str());
}

inline
std::shared_ptr<Expression> While::expr() const {
    return expr_;
}

inline
std::shared_ptr<Statement> While::stmt() const {
    return stmt_;
}
} // namespace inter
