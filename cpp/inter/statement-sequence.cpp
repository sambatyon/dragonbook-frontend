#include "cpp/inter/statement-sequence.hpp"

namespace inter {
void StatementSequence::gen(std::stringstream &ss, std::uint32_t b, std::uint32_t a) {
  if (first_ == Statement::kNullStatement) {
    second_->gen(ss, b, a);
  } else if (second_ == Statement::kNullStatement) {
    first_->gen(ss, b, a);
  } else {
    auto label = new_label();
    first_->gen(ss, b, label);
    emit_label(ss, label);
    second_->gen(ss, label, a);
  }
}
}  // namespace inter
