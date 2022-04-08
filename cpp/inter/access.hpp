#pragma once

#include <inter/operator.hpp>
#include <inter/expression.hpp>
#include <inter/identifier.hpp>

#include <lexer/word.hpp>
#include <lexer/token.hpp>

namespace inter {
class Access : public Operator {
  public:
    static std::shared_ptr<Access> create(std::shared_ptr<Identifier> id, std::shared_ptr<Expression> index,
                                          std::shared_ptr<symbols::Type> type);
    Access(std::shared_ptr<Identifier> id, std::shared_ptr<Expression> index, std::shared_ptr<symbols::Type> type);
    ~Access();

    std::shared_ptr<Identifier> array() const;
    std::shared_ptr<Expression> index() const;

    std::shared_ptr<Expression> gen() override;

    void jumping(const std::uint32_t &to, const std::uint32_t &from) override;

    std::string to_string() const override;

  private:
    std::shared_ptr<Identifier> array_;
    std::shared_ptr<Expression> index_;
};

inline
std::shared_ptr<Access> Access::create(std::shared_ptr<Identifier> id, std::shared_ptr<Expression> index,
                                       std::shared_ptr<symbols::Type> type) {
    return std::make_shared<Access>(id, index, type);
}

inline
Access::Access(std::shared_ptr<Identifier> id, std::shared_ptr<Expression> index, std::shared_ptr<symbols::Type> type)
        : Operator(lexer::Word::create("[]", lexer::Token::kIndex), type), array_(id), index_(index) {
}

inline
Access::~Access() {
}

inline
std::shared_ptr<Identifier> Access::array() const {
    return array_;
}

inline
std::shared_ptr<Expression> Access::index() const {
    return index_;
}

inline
std::shared_ptr<Expression> Access::gen() {
    return Access::create(array_, index_->reduce(), type_);
}

inline
void Access::jumping(const std::uint32_t &to, const std::uint32_t &from) {
    emit_jumps(reduce()->to_string(), to, from);
}

inline
std::string Access::to_string() const {
    return array_->to_string() + "[ " + index_->to_string() + " ]";
}
} // namespace inter
