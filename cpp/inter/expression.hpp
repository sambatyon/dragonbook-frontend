#pragma once

#include "cpp/inter/node.hpp"

#include "cpp/lexer/token.hpp"
#include "cpp/symbols/type.hpp"

#include <cstdint>
#include <memory>

namespace inter {
class Expression : public Node, public std::enable_shared_from_this<Expression> {
  public:
    static std::shared_ptr<Expression> create(std::shared_ptr<lexer::Token> oper,
                                              std::shared_ptr<symbols::Type> type);
    explicit Expression(std::shared_ptr<lexer::Token> oper, std::shared_ptr<symbols::Type> type);
    virtual ~Expression();

    virtual std::shared_ptr<Expression> get_ptr();
    virtual std::shared_ptr<Expression> gen();
    virtual std::shared_ptr<Expression> reduce();

    virtual void jumping(const std::uint32_t &to, const std::uint32_t &from);
    void emit_jumps(const std::string &test, const std::uint32_t &to, const std::uint32_t &from);

    virtual std::string to_string() const;

    std::shared_ptr<lexer::Token> oper() const;
    std::shared_ptr<symbols::Type> type() const;

  protected:
    std::shared_ptr<symbols::Type> type_;
    std::shared_ptr<lexer::Token> oper_;
};

inline
std::shared_ptr<Expression> Expression::create(std::shared_ptr<lexer::Token> oper,
                                               std::shared_ptr<symbols::Type> type) {
    return std::make_shared<Expression>(oper, type);
}

inline
Expression::Expression(std::shared_ptr<lexer::Token> oper, std::shared_ptr<symbols::Type> type)
        : type_(type), oper_(oper) {
}

inline
Expression::~Expression() {
}

inline
std::shared_ptr<Expression> Expression::get_ptr() {
    return shared_from_this();
}

inline
std::shared_ptr<Expression> Expression::gen() {
    return get_ptr();
}

inline
std::shared_ptr<Expression> Expression::reduce() {
    return get_ptr();
}

inline
void Expression::jumping(const std::uint32_t &to, const std::uint32_t &from) {
    emit_jumps(oper_->to_string(), to, from);
}

inline
std::string Expression::to_string() const {
    return oper_->to_string();
}

inline
std::shared_ptr<lexer::Token> Expression::oper() const {
    return oper_;
}

inline
std::shared_ptr<symbols::Type> Expression::type() const {
    return type_;
}
} // namespace inter
