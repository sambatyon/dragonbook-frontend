#include <inter/relational.hpp>

namespace inter {
std::shared_ptr<symbols::Type> Relational::check(std::shared_ptr<symbols::Type> left,
                                                 std::shared_ptr<symbols::Type> right) {
    symbols::Array *arr_left = dynamic_cast<symbols::Array*>(left.get());
    symbols::Array *arr_right = dynamic_cast<symbols::Array*>(right.get());
    if (arr_left || arr_right)
        return std::shared_ptr<symbols::Type>();
    else if (left == right)
        return symbols::Type::boolean;
    else
        return std::shared_ptr<symbols::Type>();
}

void Relational::jumping(const std::uint32_t &to, const std::uint32_t &from) {
    auto a = expr1()->reduce();
    auto b = expr2()->reduce();
    emit_jumps(a->to_string() + " " + oper()->to_string() + " " + b->to_string(), to, from);
}
} // namespace inter
