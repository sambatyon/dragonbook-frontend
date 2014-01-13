#pragma once

#include <inter/logical.hpp>

namespace inter {
class Not : public Logical {
  public:
    static std::shared_ptr<Not> create(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> expr);
    Not(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> expr);
    ~Not();

    void jumping(const std::uint32_t &to, const std::uint32_t &from) override;
    std::string to_string() const override;
};

inline
std::shared_ptr<Not> Not::create(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> expr) {
    return std::make_shared<Not>(token, expr);
}

inline
Not::Not(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> expr)
        : Logical(token, expr, expr) {
}

inline
Not::~Not() {
}

inline
void Not::jumping(const std::uint32_t &to, const std::uint32_t &from) {
    expr1()->jumping(from, to);
}

inline
std::string Not::to_string() const {
    return oper()->to_string() + " " + expr1()->to_string();
}
} // namespace inter
