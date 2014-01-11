#pragma once

#include <inter/expression.hpp>

#include <inter/temporary.hpp>

namespace inter {
class Operator : public Expression {
  public:
    static std::shared_ptr<Operator> create(std::shared_ptr<lexer::Token> token, std::shared_ptr<symbols::Type> type);
    Operator(std::shared_ptr<lexer::Token> token, std::shared_ptr<symbols::Type> type);
    ~Operator();

    std::shared_ptr<Expression> reduce() override;
};

inline
std::shared_ptr<Operator> Operator::create(std::shared_ptr<lexer::Token> token, std::shared_ptr<symbols::Type> type) {
    return std::make_shared<Operator>(token, type);
}

inline
Operator::Operator(std::shared_ptr<lexer::Token> token, std::shared_ptr<symbols::Type> type)
  : Expression(token, type) {
}

inline
Operator::~Operator() {
}

inline
std::shared_ptr<Expression> Operator::reduce() {
    std::shared_ptr<Expression> expr = gen();
    std::shared_ptr<Temporary> temp = Temporary::create(type());
    emit(temp->to_string() + " = " + expr->to_string());
    return temp;
}
} // namespace inter
