#pragma once

#include <lexer/lexer.hpp>

#include <exception>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cstdint>
#include <cstddef>
#include <string>

namespace inter {
class Node {
  public:
    void error(const std::string &message);
    static std::uint32_t new_label();

    void emit_label(const std::uint32_t &i);
    void emit(const std::string &label);

  protected:
    Node();
    virtual ~Node();

    std::size_t lexline_;

  private:
    static std::uint32_t labels_;
};

inline
Node::Node() : lexline_(lexer::Lexer::current_line()) {
}

inline
Node::~Node() {
    lexline_ = 0;
}

inline
void Node::error(const std::string &message) {
    std::stringstream ss;
    ss << "near line " << lexline_ << ": " << message;
    throw std::runtime_error(ss.str());
}

inline
std::uint32_t Node::new_label() {
    return ++labels_;
}

inline
void Node::emit_label(const std::uint32_t &i) {
    std::cout << "L" << i << ":";
}

inline
void Node::emit(const std::string &message) {
    std::cout << '\t' << message << '\n';
}
} // namespace inter
