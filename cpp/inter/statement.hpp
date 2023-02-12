#pragma once

#include "cpp/inter/node.hpp"

namespace inter {
class Statement : public Node {
 public:
  static const std::shared_ptr<Statement> kNullStatement;
  static std::shared_ptr<Statement> enclosing_statement;
  static std::shared_ptr<Statement> create();

  Statement();
  virtual ~Statement();

  std::uint32_t after() const;

  virtual void gen(std::stringstream &ss, std::uint32_t b, std::uint32_t a);

 protected:
  std::uint32_t after_;
};

inline std::shared_ptr<Statement> Statement::create() {
  return std::make_shared<Statement>();
}

inline Statement::Statement() : after_(0) {
}

inline Statement::~Statement() {
}

inline void Statement::gen(std::stringstream &ss, std::uint32_t b, std::uint32_t a) {
}

inline std::uint32_t Statement::after() const {
  return after_;
}
}  // namespace inter
