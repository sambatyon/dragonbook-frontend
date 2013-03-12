#include <inter/expression.hpp>

#include <sstream>

namespace inter {
void Expression::emit_jumps(const std::string &test, const std::uint32_t &to, const std::uint32_t &from) {
    std::stringstream ss;
    if (to && from) {
        ss << "if " << test << " goto L" << to;
        emit(ss.str());
        ss.str("");
        ss << "goto L" << from;
        emit(ss.str());
    } else if (to) {
        ss << "if " << test << " goto L" << to;
        emit(ss.str());
    } else if (from) {
        ss << "iffalse " << test << " goto L" << from;
        emit(ss.str());
    }
}
} // namespace inter
