#pragma once

#include "cpp/lexer/type.hpp"
#include "cpp/lexer/token.hpp"

#include <memory>
#include <cstddef>
#include <sstream>

namespace symbols {
class Array : public Type {
  public:
    static std::shared_ptr<Array> create(const std::size_t &element_count, std::shared_ptr<Type> type);
    Array(const std::size_t &element_count, std::shared_ptr<Type> type);
    ~Array();

    std::shared_ptr<Type> type() const;
    std::size_t element_count() const;

    std::string to_string() const override;

  private:
    std::shared_ptr<Type> type_;
    std::size_t element_count_;
};

inline
std::shared_ptr<Array> Array::create(const std::size_t &element_count, std::shared_ptr<Type> type) {
    return std::make_shared<Array>(element_count, type);
}

inline
Array::Array(const std::size_t &element_count, std::shared_ptr<Type> type)
        : Type("[]", Token::kIndex, type->width() * element_count), type_(type), element_count_(element_count) {
}

inline
Array::~Array() {
}

inline
std::shared_ptr<Type> Array::type() const {
    return type_;
}

inline
std::size_t Array::element_count() const {
    return element_count_;
}

inline
std::string Array::to_string() const {
    std::stringstream ss;
    ss << "[" << element_count_ << "] " << type_->to_string();
    return ss.str();
}
} // namespace symbols
