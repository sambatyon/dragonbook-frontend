#pragma once

#include <inter/Expression>

#include <memory>

namespace inter {
class Logical : public Expression {
  public:
    static std::shared_ptr<Logical> create(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> expr1,
                                           std::shared_ptr<Expression> expr2);
    Logical(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> expr1, std::shared_ptr<Expression> expr2);
    ~Logical();

    std::shared_ptr<Expression> expr1() const;
    std::shared_ptr<Expression> expr2() const;

    std::shared_ptr<Expression> gen();

    std::string to_string();

  private:
    std::shared_ptr<Expression> expr1_;
    std::shared_ptr<Expression> expr2_;
};

inline
std::shared_ptr<Logical> Logical::create(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> expr1,
                                         std::shared_ptr<Expression> expr2) {
    return std::make_shared<Logical>(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> expr1,
                                     std::shared_ptr<Expression> expr2);
}

inline
Logical::Logical(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> expr1,
                 std::shared_ptr<Expression> expr2) {

}

inline
Logical::~Logical() {
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
    //return Arithmetic::create(oper_, expr1_->reduce(), expr2_->reduce());
}

inline
std::string Arithmetic::to_string() {
    return expr1_->to_string() + " " + oper_->to_string() + " " + expr2_->to_string();
}
} // namespace inter
