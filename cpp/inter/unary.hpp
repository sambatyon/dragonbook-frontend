#pragma once

#include "cpp/inter/operator.hpp"

namespace inter {
class UnaryOperator : public Operator {
public:
    static std::shared_ptr<UnaryOperator> create(std::shared_ptr<lexer::Token> token,
                                                 std::shared_ptr<Expression> expression);
    UnaryOperator(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> expression);
    ~UnaryOperator();

    std::shared_ptr<Expression> expression() const;
    std::shared_ptr<Expression> gen() override;

    std::string to_string() const override;

private:
    std::shared_ptr<Expression> expression_;
};

inline
std::shared_ptr<UnaryOperator> UnaryOperator::create(std::shared_ptr<lexer::Token> token,
                                                     std::shared_ptr<Expression> expression) {
    return std::make_shared<UnaryOperator>(token, expression);
}

inline
UnaryOperator::UnaryOperator(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> expression)
        : Operator(token, std::shared_ptr<symbols::Type>())
        , expression_(expression) {
    auto type = symbols::Type::max(symbols::Type::integer.get(), expression_->type().get());
    if (!type)
        error("Type error");
    else
        type_ = type->getptr();
}

inline
UnaryOperator::~UnaryOperator() {
}

inline
std::shared_ptr<Expression> UnaryOperator::expression() const {
    return expression_;
}

inline
std::shared_ptr<Expression> UnaryOperator::gen() {
    return UnaryOperator::create(oper(), expression_->reduce());
}

inline
std::string UnaryOperator::to_string() const {
    return oper()->to_string();
}
} // namepsace inter
