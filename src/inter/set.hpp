#pragma once

#include <inter/statement.hpp>
#include <inter/identifier.hpp>

namespace inter {
class Set : public Statement {
  public:
    static std::shared_ptr<Set> create(std::shared_ptr<Identifier> id, std::shared_ptr<Expression> expr);
    Set(std::shared_ptr<Identifier> id, std::shared_ptr<Expression> expr);
    ~Set();

    std::shared_ptr<Identifier> id() const;
    std::shared_ptr<Expression> expr() const;

    std::shared_ptr<symbols::Type> check(std::shared_ptr<symbols::Type> left, std::shared_ptr<symbols::Type> right);
    void gen(const std::uint32_t &b, const std::uint32_t &a) override;

  private:
    std::shared_ptr<Identifier> id_;
    std::shared_ptr<Expression> expr_;
};

inline
std::shared_ptr<Set> Set::create(std::shared_ptr<Identifier> id, std::shared_ptr<Expression> expr) {
    return std::make_shared<Set>(id, expr);
}

inline
Set::Set(std::shared_ptr<Identifier> id, std::shared_ptr<Expression> expr)
        : id_(id), expr_(expr) {
    if (!check(id_->type(), expr_->type()))
        error("type error");
}

inline
Set::~Set() {
}

inline
void Set::gen(const std::uint32_t &b, const std::uint32_t &a) {
    emit(id_->to_string() + " = " + expr_->gen()->to_string());
}

inline
std::shared_ptr<symbols::Type> Set::check(std::shared_ptr<symbols::Type> left, std::shared_ptr<symbols::Type> right) {
    if (left->is_numeric() && right->is_numeric())
        return right;
    else if (left == symbols::Type::boolean && right == symbols::Type::boolean)
        return right;
    else
        return std::shared_ptr<symbols::Type>();
}

inline
std::shared_ptr<Identifier> Set::id() const {
    return id_;
}

inline
std::shared_ptr<Expression> Set::expr() const {
    return expr_;
}
} // namespace inter
