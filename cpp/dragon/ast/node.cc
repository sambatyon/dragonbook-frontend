#include "cpp/dragon/ast/node.hh"

#include <atomic>

namespace dragon {
namespace ast {
static std::atomic<int> label_count(0);

void reset_label() {
  label_count = 0;
}

int new_label() {
  return ++label_count;
}
} // namespace ast
} // namespace dragon
