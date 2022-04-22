#include "cpp/inter/or.hpp"

#include "cpp/inter/node.hpp"

namespace inter {
void Or::jumping(const std::uint32_t &to, const std::uint32_t &from) {
  int label = to != 0 ? to : new_label();
  expr1()->jumping(label, 0);
  expr2()->jumping(to, from);
  if (!to) {
    emit_label(label);
  }
}
}  // namespace inter
