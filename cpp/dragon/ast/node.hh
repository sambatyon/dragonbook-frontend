#pragma once

#include <iostream>
#include <string>

namespace dragon {
namespace ast {

inline  void emit_label(std::ostream &os, int lbl) {
  os << 'L' << lbl << ':';
}
inline void emit(std::ostream &os, std::string s) {
  os << '\t' << s << '\n';
}

void reset_label();
int new_label();

class Node {
 public:
  Node() {}
  virtual ~Node() {}

  virtual const std::string to_string() const = 0;
};
} // namespace ast
} // namespace dragon
