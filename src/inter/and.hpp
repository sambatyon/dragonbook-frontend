#pragma once

#include <inter/logical.hpp>

namespace inter {
class And : public Logical {
  public:
    static std::shared_ptr<And> create(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> left,
                                       std::shared_ptr<Expression> right);
    And(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> left, std::shared_ptr<Expression> right);
    ~And();

    void jumping(const std::uint32_t &to, const std::uint32_t &from) override;
};

inline
std::shared_ptr<And> And::create(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> left,
                                 std::shared_ptr<Expression> right) {
    return std::make_shared<And>(token, left, right);
}

inline
And::And(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> left, std::shared_ptr<Expression> right)
        : Logical(token, left, right) {
}

inline
And::~And() {
}
} // namespace inter
