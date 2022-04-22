#include "cpp/inter/statement-sequence.hpp"

namespace inter {
void StatementSequence::gen(const std::uint32_t &b, const std::uint32_t &a) {
  if (first_ == Statement::kNullStatement) {
    second_->gen(b, a);
  } else if (second_ == Statement::kNullStatement) {
    first_->gen(b, a);
  } else {
    auto label = new_label();
    first_->gen(b, label);
    emit_label(label);
    second_->gen(label, a);
  }
}
}  // namespace inter
