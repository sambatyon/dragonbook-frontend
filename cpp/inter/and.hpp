#pragma once

#include "cpp/inter/logical.hpp"

namespace inter {
class And : public Logical {
 public:
  static std::shared_ptr<And> create(
      std::shared_ptr<lexer::Token> token,
      std::shared_ptr<Expression> left,
      std::shared_ptr<Expression> right
  );

  And(std::shared_ptr<lexer::Token> token, std::shared_ptr<Expression> left, std::shared_ptr<Expression> right);
  ~And();

  void jumping(std::stringstream &ss, std::uint32_t to, std::uint32_t from) override;
};

inline std::shared_ptr<And> And::create(
    std::shared_ptr<lexer::Token> token,
    std::shared_ptr<Expression> left,
    std::shared_ptr<Expression> right
) {
  auto res = std::make_shared<And>(token, left, right);
  res->init();
  return res;
}

inline And::And(
    std::shared_ptr<lexer::Token> token,
    std::shared_ptr<Expression> left,
    std::shared_ptr<Expression> right
)
    : Logical(token, left, right) {
}

inline And::~And() {
}
}  // namespace inter
