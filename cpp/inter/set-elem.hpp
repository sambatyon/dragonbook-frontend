#pragma once

#include "cpp/inter/access.hpp"
#include "cpp/inter/statement.hpp"
#include "cpp/inter/identifier.hpp"

#include "cpp/symbols/array.hpp"

namespace inter {
class SetElem : public Statement {
  public:
    static std::shared_ptr<SetElem> create(std::shared_ptr<Access> access, std::shared_ptr<Expression> expr);
    SetElem(std::shared_ptr<Access> access, std::shared_ptr<Expression> expr);
    ~SetElem();

    std::shared_ptr<Identifier> array() const;
    std::shared_ptr<Expression> index() const;
    std::shared_ptr<Expression> expr() const;

    std::shared_ptr<symbols::Type> check(std::shared_ptr<symbols::Type> left, std::shared_ptr<symbols::Type> right);
    void gen(const std::uint32_t &b, const std::uint32_t &a) override;

  private:
    std::shared_ptr<Identifier> array_;
    std::shared_ptr<Expression> index_;
    std::shared_ptr<Expression> expr_;
};

inline
std::shared_ptr<SetElem> SetElem::create(std::shared_ptr<Access> access, std::shared_ptr<Expression> expr) {
    return std::make_shared<SetElem>(access, expr);
}

inline
SetElem::SetElem(std::shared_ptr<Access> access, std::shared_ptr<Expression> expr)
        : array_(access->array()), index_(access->index()), expr_(expr) {
    if (!check(access->type(), expr_->type()))
        error("type error");
}

inline
SetElem::~SetElem() {
}

inline
void SetElem::gen(const std::uint32_t &b, const std::uint32_t &a) {
    auto index = index_->reduce()->to_string();
    auto expression = expr_->reduce()->to_string();
    emit(array_->to_string() + "[ " + index + " ] = " + expression);
}

inline
std::shared_ptr<symbols::Type> SetElem::check(std::shared_ptr<symbols::Type> left,
                                              std::shared_ptr<symbols::Type> right) {
    auto left_array = dynamic_cast<symbols::Array*>(left.get());
    auto right_array = dynamic_cast<symbols::Array*>(right.get());
    if (left_array || right_array)
        return std::shared_ptr<symbols::Type>();
    else if (left->is_numeric() && right->is_numeric())
        return right;
    else
        return std::shared_ptr<symbols::Type>();
}

inline
std::shared_ptr<Identifier> SetElem::array() const {
    return array_;
}

inline
std::shared_ptr<Expression> SetElem::index() const {
    return index_;
}

inline
std::shared_ptr<Expression> SetElem::expr() const {
    return expr_;
}
} // namespace inter
