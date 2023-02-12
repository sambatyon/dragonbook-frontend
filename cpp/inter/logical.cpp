#include "cpp/inter/logical.hpp"

#include "cpp/inter/temporary.hpp"

namespace inter {
std::shared_ptr<Expression> Logical::gen(std::stringstream &ss) {
  auto f = new_label();
  auto a = new_label();
  auto temp = Temporary::create(type());
  jumping(ss, 0, f);
  emit(ss, temp->to_string() + " = true");
  std::stringstream lbl;
  lbl << a;
  emit(ss, "goto L" + lbl.str());
  emit_label(ss, f);
  emit(ss, temp->to_string() + " = false");
  emit_label(ss, a);
  return temp;
}
}  // namespace inter
