#include "cpp/inter/expression.hpp"

#include <sstream>

namespace inter {
void Expression::emit_jumps(std::stringstream &ss, const std::string &test, std::uint32_t to, std::uint32_t from) {
  std::stringstream tst;
  if (to && from) {
    tst << "if " << test << " goto L" << to;
    emit(ss, tst.str());
    tst.str("");
    tst << "goto L" << from;
    emit(ss, tst.str());
  } else if (to) {
    tst << "if " << test << " goto L" << to;
    emit(ss, tst.str());
  } else if (from) {
    tst << "iffalse " << test << " goto L" << from;
    emit(ss, tst.str());
  }
}
}  // namespace inter
