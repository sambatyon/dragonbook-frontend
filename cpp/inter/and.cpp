#include "cpp/inter/and.hpp"

namespace inter {
void And::jumping(std::stringstream &ss, std::uint32_t to, std::uint32_t from) {
  int label = from != 0 ? from : new_label();
  expr1()->jumping(ss, 0, label);
  expr2()->jumping(ss, to, from);
  if (!from) {
    emit_label(ss, label);
  }
}
}  // namespace inter
