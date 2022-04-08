#include <symbols/type.hpp>

#include <lexer/token.hpp>

namespace symbols {
std::shared_ptr<Type> Type::integer = Type::create("int", Token::kBasic, 4);
std::shared_ptr<Type> Type::real = Type::create("float", Token::kBasic, 8);
std::shared_ptr<Type> Type::character = Type::create("char", Token::kBasic, 1);
std::shared_ptr<Type> Type::boolean = Type::create("bool", Token::kBasic, 1);

Type *Type::max(Type *type_left, Type *type_right) {
    if (!type_left)
        return type_right;
    else if (!type_right)
        return type_left;
    else if (!type_left->is_numeric() || !type_right->is_numeric())
        return nullptr;
    else if (*type_left == *real || *type_right == *real)
        return real.get();
    else if (*type_left == *integer || *type_right == *integer)
        return integer.get();
    else
        return character.get();
}

bool Type::operator==(const Type &other) const {
    if (this == &other)
        return true;
    return width_ == other.width_ && this->lexeme() == other.lexeme() && this->tag() == other.tag();
}

bool Type::is_numeric() const {
    return (*real == *this) || (*integer == *this) || (*this == *character);
}
} // namespace symbols
