#pragma once

#include <lexer/token.hpp>

#include <memory>
#include <sstream>
#include <cstdint>

namespace lexer {
template <typename T, std::uint32_t TAG>
class NumericType : public Token {
  public:
    typedef T value_type;
    static std::shared_ptr<NumericType<T, TAG>> create(const T &value);
    explicit NumericType(const T &value);
    ~NumericType();

    value_type value() const;
    std::string to_string() const override;

  private:
    T value_;
};

typedef NumericType<double, Token::kReal> Real;
typedef NumericType<std::int64_t, Token::kInteger> Number;

template<typename T, std::uint32_t TAG>
std::shared_ptr<NumericType<T, TAG>> create(const T &value) {
    return std::make_shared<NumericType<T, TAG>>(value);
}

template<typename T, std::uint32_t TAG>
NumericType<T, TAG>::NumericType(const T &value) : Token(TAG), value_(value) {
}

template<typename T, std::uint32_t TAG>
NumericType<T, TAG>::~NumericType() {
}

template<typename T, std::uint32_t TAG>
T NumericType<T, TAG>::value() const {
    return value_;
}

template<typename T, std::uint32_t TAG>
std::string NumericType<T, TAG>::to_string() const {
    std::stringstream ss;
    ss << value_;
    return ss.str();
}
} // namespace lexer
