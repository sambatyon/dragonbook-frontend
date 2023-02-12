#include "cpp/inter/or.hpp"

#include "cpp/inter/node.hpp"

namespace inter {
void Or::jumping(std::stringstream &ss, std::uint32_t to, std::uint32_t from) {
  int label = to != 0 ? to : new_label();
  expr1()->jumping(ss, label, 0);
  expr2()->jumping(ss, to, from);
  if (!to) {
    emit_label(ss, label);
  }
}
}  // namespace inter
