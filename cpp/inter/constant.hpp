#pragma once

#include "cpp/inter/expression.hpp"
#include "cpp/lexer/numeric.hpp"

namespace inter {
class Constant : public Expression {
public:
    static std::shared_ptr<Constant> kTrue;
    static std::shared_ptr<Constant> kFalse;

    static std::shared_ptr<Constant> create(std::shared_ptr<lexer::Token> token, std::shared_ptr<symbols::Type> type);
    static std::shared_ptr<Constant> create(std::int64_t value);
    Constant(std::shared_ptr<lexer::Token> token, std::shared_ptr<symbols::Type> type);
    explicit Constant(std::int64_t value);
    ~Constant();

    void jumping(const std::uint32_t &to, const std::uint32_t &from) override;
};

inline
std::shared_ptr<Constant> Constant::create(std::shared_ptr<lexer::Token> token, std::shared_ptr<symbols::Type> type) {
    return std::make_shared<Constant>(token, type);
}

inline
std::shared_ptr<Constant> Constant::create(std::int64_t value) {
    return std::make_shared<Constant>(value);
}

inline
Constant::Constant(std::shared_ptr<lexer::Token> token, std::shared_ptr<symbols::Type> type)
        : Expression(token, type) {
}

inline
Constant::Constant(std::int64_t value) : Expression(std::make_shared<lexer::Number>(value), symbols::Type::integer) {
}

inline
Constant::~Constant() {
}

inline
void Constant::jumping(const std::uint32_t &to, const std::uint32_t &from) {
    std::stringstream ss;
    if (this == kTrue.get() && to != 0) {
        ss << to;
        emit("goto L" + ss.str());
    } else if (this == kFalse.get() && from != 0) {
        ss << from;
        emit("goto L" + ss.str());
    }
}
} // namespace inter
