#pragma once

#include <inter/logical.hpp>

namespace inter {
class Or : public Logical {
  public:
    static std::shared_ptr<Or> create(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> left,
                                      std::shared_ptr<Expression> right);
    Or(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> left, std::shared_ptr<Expression> right);
    ~Or();

    void jumping(const std::uint32_t &to, const std::uint32_t &from) override;
};

inline
std::shared_ptr<Or> Or::create(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> left,
                               std::shared_ptr<Expression> right) {
    auto res = std::make_shared<Or>(token, left, right);
    res->init();
    return res;
}

inline
Or::Or(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> left, std::shared_ptr<Expression> right)
        : Logical(token, left, right) {
}

inline
Or::~Or() {
}
} // namespace inter
