#include "cpp/inter/and.hpp"

namespace inter {
void And::jumping(const std::uint32_t &to, const std::uint32_t &from) {
  int label = from != 0 ? from : new_label();
  expr1()->jumping(0, label);
  expr2()->jumping(to, from);
  if (!from)
    emit_label(label);
}
}  // namespace inter
