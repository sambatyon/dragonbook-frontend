#include "cpp/inter/logical.hpp"
#include "cpp/inter/temporary.hpp"

namespace inter {
std::shared_ptr<Expression> Logical::gen() {
    auto f = new_label();
    auto a = new_label();
    auto temp = Temporary::create(type());
    jumping(0, f);
    emit(temp->to_string() + " = true");
    std::stringstream ss;
    ss << a;
    emit("goto L" + ss.str());
    emit_label(f);
    emit(temp->to_string() + " = false");
    emit_label(a);
    return temp;
}
} // namespace inter
