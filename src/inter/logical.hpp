#pragma once

#include <inter/expression.hpp>

#include <memory>

namespace inter {
class Logical : public Expression {
  public:
    static std::shared_ptr<Logical> create(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> expr1,
                                           std::shared_ptr<Expression> expr2);
    Logical(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> expr1, std::shared_ptr<Expression> expr2);
    virtual ~Logical();

    std::shared_ptr<Expression> expr1() const;
    std::shared_ptr<Expression> expr2() const;

    virtual std::shared_ptr<symbols::Type> check(std::shared_ptr<symbols::Type> left,
                                                 std::shared_ptr<symbols::Type> right);
    virtual std::shared_ptr<Expression> gen() override;

    virtual std::string to_string() const override;

  private:
    std::shared_ptr<Expression> expr1_;
    std::shared_ptr<Expression> expr2_;
};

inline
std::shared_ptr<Logical> Logical::create(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> expr1,
                                         std::shared_ptr<Expression> expr2) {
    return std::make_shared<Logical>(token, expr1, expr2);
}

inline
Logical::Logical(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> expr1,
                 std::shared_ptr<Expression> expr2)
        : Expression(token, std::shared_ptr<symbols::Type>())
        , expr1_(expr1)
        , expr2_(expr2) {
    auto type = check(expr1_->type(), expr2_->type());
    if (!type)
        error("type error");
    type_ = type;
}

inline
Logical::~Logical() {
}

inline
std::shared_ptr<Expression> Logical::expr1() const {
    return expr1_;
}

inline
std::shared_ptr<Expression> Logical::expr2() const {
    return expr2_;
}

inline
std::shared_ptr<symbols::Type> Logical::check(std::shared_ptr<symbols::Type> left,
                                              std::shared_ptr<symbols::Type> right) {
    if (left == symbols::Type::boolean && right == symbols::Type::boolean)
        return symbols::Type::boolean;
    else
        return std::shared_ptr<symbols::Type>();
}

inline
std::string Logical::to_string() const {
    return expr1_->to_string() + " " + oper_->to_string() + " " + expr2_->to_string();
}
} // namespace inter
