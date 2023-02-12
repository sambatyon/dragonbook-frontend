#pragma once

#include "cpp/inter/statement.hpp"

namespace inter {
class Break : public Statement {
 public:
  static std::shared_ptr<Break> create();

  Break();
  virtual ~Break();

  virtual void gen(std::stringstream &ss, std::uint32_t b, std::uint32_t a) override;

  std::shared_ptr<Statement> statement() const;

 private:
  std::shared_ptr<Statement> statement_;
};

inline std::shared_ptr<Break> Break::create() {
  return std::make_shared<Break>();
}

inline Break::Break() : statement_(Statement::enclosing_statement) {
  if (statement_ == Statement::kNullStatement)
    error("Unenclosed break");
}

inline Break::~Break() {
}

inline void Break::gen(std::stringstream &ss,std::uint32_t b, std::uint32_t a) {
  std::stringstream after;
  after << statement_->after();
  emit(ss, "goto L" + after.str());
}
}  // namespace inter
