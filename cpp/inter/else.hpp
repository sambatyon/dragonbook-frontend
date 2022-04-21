#pragma once

#include "cpp/inter/statement.hpp"
#include "cpp/inter/expression.hpp"

namespace inter {
class Else : public Statement {
  public:
    static std::shared_ptr<Else> create(std::shared_ptr<Expression> expr, std::shared_ptr<Statement> statement_if,
                                        std::shared_ptr<Statement> statement_else);
    Else(std::shared_ptr<Expression> expr, std::shared_ptr<Statement> statement_if,
         std::shared_ptr<Statement> statement_else);
    ~Else();

    void gen(const std::uint32_t &b, const std::uint32_t &a) override;

    std::shared_ptr<Expression> expr() const;
    std::shared_ptr<Statement> statement_if() const;
    std::shared_ptr<Statement> statement_else() const;

  private:
    std::shared_ptr<Expression> expr_;
    std::shared_ptr<Statement> statement_if_;
    std::shared_ptr<Statement> statement_else_;
};

inline
std::shared_ptr<Else> Else::create(std::shared_ptr<Expression> expr, std::shared_ptr<Statement> statement_if,
                                   std::shared_ptr<Statement> statement_else) {
    return std::make_shared<Else>(expr, statement_if, statement_else);
}

inline
Else::Else(std::shared_ptr<Expression> expr, std::shared_ptr<Statement> statement_if,
           std::shared_ptr<Statement> statement_else)
        : expr_(expr), statement_if_(statement_if), statement_else_(statement_else) {
    if (expr_->type() != symbols::Type::boolean)
        expr->error("Boolean required in if");
}

inline
Else::~Else() {
}

inline
void Else::gen(const std::uint32_t &b, const std::uint32_t &a) {
    auto label_statement_if = new_label();
    auto label_statement_else = new_label();
    expr_->jumping(0, label_statement_else);
    emit_label(label_statement_if);
    statement_if_->gen(label_statement_if, a);
    std::stringstream ss;
    ss << a;
    emit("goto L" + ss.str());
    emit_label(label_statement_else);
    statement_else_->gen(label_statement_else, a);
}

inline
std::shared_ptr<Expression> Else::expr() const {
    return expr_;
}

inline
std::shared_ptr<Statement> Else::statement_if() const {
    return statement_if_;
}

inline
std::shared_ptr<Statement> Else::statement_else() const {
    return statement_else_;
}
} // namespace inter
