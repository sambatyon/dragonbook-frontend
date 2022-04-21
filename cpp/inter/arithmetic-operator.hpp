#pragma once

#include "cpp/inter/operator.hpp"
#include "cpp/inter/expression.hpp"

#include "cpp/lexer/word.hpp"

#include <memory>

namespace inter {
class Arithmetic : public Operator {
  public:
    static std::shared_ptr<Arithmetic> create(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> expr1,
                                              std::shared_ptr<Expression> expr2);
    Arithmetic(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> expr1,
               std::shared_ptr<Expression> expr2);
    virtual ~Arithmetic();

    std::shared_ptr<Expression> expr1() const;
    std::shared_ptr<Expression> expr2() const;

    virtual std::shared_ptr<Expression> gen() override;

    virtual std::string to_string() const override;

  private:
    std::shared_ptr<Expression> expr1_;
    std::shared_ptr<Expression> expr2_;
};

inline
std::shared_ptr<Arithmetic> Arithmetic::create(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> expr1,
                                               std::shared_ptr<Expression> expr2) {
    return std::make_shared<Arithmetic>(token, expr1, expr2);
}

inline
Arithmetic::Arithmetic(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> expr1,
                       std::shared_ptr<Expression> expr2) : Operator(token, std::shared_ptr<symbols::Type>())
                                                          , expr1_(expr1), expr2_(expr2) {
    auto type = symbols::Type::max(expr1->type().get(), expr2->type().get());
    if (type == nullptr)
        error("Arithmentic expression has no valid types");
    else
        type_ = type->getptr();
}

inline
Arithmetic::~Arithmetic() {
}

inline
std::shared_ptr<Expression> Arithmetic::expr1() const {
    return expr1_;
}

inline
std::shared_ptr<Expression> Arithmetic::expr2() const {
    return expr2_;
}

inline
std::shared_ptr<Expression> Arithmetic::gen() {
    return Arithmetic::create(oper_, expr1_->reduce(), expr2_->reduce());
}

inline
std::string Arithmetic::to_string() const {
    return expr1_->to_string() + " " + oper_->to_string() + " " + expr2_->to_string();
}
} // namespace inter
