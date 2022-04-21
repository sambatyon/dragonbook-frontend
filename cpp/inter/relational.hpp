#pragma once

#include "cpp/inter/logical.hpp"

#include "cpp/symbols/array.hpp"

namespace inter {
class Relational : public Logical {
public:
    static std::shared_ptr<Relational> create(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> left,
                                              std::shared_ptr<Expression> right);
    Relational(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> left,
               std::shared_ptr<Expression> right);
    ~Relational();

    std::shared_ptr<symbols::Type> check(std::shared_ptr<symbols::Type> left,
                                         std::shared_ptr<symbols::Type> right) override;
    void jumping(const std::uint32_t &to, const std::uint32_t &from) override;
};

inline
std::shared_ptr<Relational> Relational::create(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> left,
                                               std::shared_ptr<Expression> right) {
    auto res = std::make_shared<Relational>(token, left, right);
    res->init();
    return res;
}

inline
Relational::Relational(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> left,
                       std::shared_ptr<Expression> right) : Logical(token, left, right) {
}

inline
Relational::~Relational() {
}
} // namespace inter
