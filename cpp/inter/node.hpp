#pragma once

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

  void emit_label(std::stringstream &ss, std::uint32_t i);
  void emit(std::stringstream &ss, const std::string &label);

 protected:
  Node();
  virtual ~Node();

  std::size_t lexline_;

 private:
  static thread_local std::uint32_t labels_;
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

inline void Node::emit_label(std::stringstream &ss, std::uint32_t i) {
  ss << "L" << i << ":";
}

inline void Node::emit(std::stringstream &ss, const std::string &message) {
  ss << '\t' << message << '\n';
}
}  // namespace inter
