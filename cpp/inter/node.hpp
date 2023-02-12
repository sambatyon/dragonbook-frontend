#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "cpp/lexer/lexer.hpp"

namespace inter {
class Node {
 public:
  void error(const std::string &message);
  static std::uint32_t new_label();
  static void reset_labels();

  void emit_label(const std::uint32_t &i);
  void emit(const std::string &label);

 protected:
  Node();
  virtual ~Node();

  std::size_t lexline_;

 private:
  static std::atomic_uint32_t labels_;
};

inline Node::Node() : lexline_(lexer::Lexer::current_line()) {
}

inline Node::~Node() {
  lexline_ = 0;
}

inline void Node::error(const std::string &message) {
  std::stringstream ss;
  ss << "near line " << lexline_ << ": " << message;
  throw std::runtime_error(ss.str());
}

inline std::uint32_t Node::new_label() {
  return ++labels_;
}

inline void Node::reset_labels() {
  labels_ = 0;
}

inline void Node::emit_label(const std::uint32_t &i) {
  std::cout << "L" << i << ":";
}

inline void Node::emit(const std::string &message) {
  std::cout << '\t' << message << '\n';
}
}  // namespace inter
