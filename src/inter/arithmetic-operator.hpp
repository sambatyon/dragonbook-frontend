#pragma once

#include <inter/operator.hpp>
#include <inter/expression.hpp>

#include <lexer/word.hpp>

#include <memory>

namespace inter {
class Arithmetic : public Operator {
  public:
    static std::shared_ptr<Arithmetic> create(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> expr1,
                                              std::shared_ptr<Expression> expr2);
    Arithmetic(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> expr1,
               std::shared_ptr<Expression> expr2);
    ~Arithmetic();

    std::shared_ptr<Expression> expr1() const;
    std::shared_ptr<Expression> expr2() const;

    std::shared_ptr<Expression> gen();

    std::string to_string();

  private:
    std::shared_ptr<Expression> expr1_;
    std::shared_ptr<Expression> expr2_;
};

inline
std::shared_ptr<Arithmetic> Arithmetic::create(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> expr1,
                                               std::shared_ptr<Expression> expr2) {
    return std::make_shared<Arithmetic>(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> expr1,
                                        std::shared_ptr<Expression> expr2);
}

inline
Arithmetic::Arithmetic(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> expr1,
                       std::shared_ptr<Expression> expr2) : Operator(token, std::shared_ptr())
                                                          , expr1_(expr1), expr2_(expr2) {
    symbols::Type type = Type::max(expr1->type().get(), expr2->type().get());
    if (type == nullptr)
        throw std::runtime_error("Arithmentic expression has no valid types");
    type_ = type->get_ptr();
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
std::string Arithmetic::to_string() {
    return expr1_->to_string() + " " + oper_->to_string() + " " + expr2_->to_string();
}
} // namespace inter
