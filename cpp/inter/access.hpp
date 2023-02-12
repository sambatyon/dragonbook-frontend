#pragma once

#include "cpp/inter/expression.hpp"
#include "cpp/inter/identifier.hpp"
#include "cpp/inter/operator.hpp"

#include "cpp/lexer/token.hpp"
#include "cpp/lexer/word.hpp"

namespace inter {
class Access : public Operator {
 public:
  static std::shared_ptr<Access> create(
      std::shared_ptr<Identifier> id,
      std::shared_ptr<Expression> index,
      std::shared_ptr<symbols::Type> type
  );

  Access(std::shared_ptr<Identifier> id, std::shared_ptr<Expression> index, std::shared_ptr<symbols::Type> type);
  ~Access();

  std::shared_ptr<Identifier> array() const;
  std::shared_ptr<Expression> index() const;

  std::shared_ptr<Expression> gen(std::stringstream &ss) override;

  void jumping(std::stringstream &ss, std::uint32_t to, std::uint32_t from) override;

  std::string to_string() const override;

 private:
  std::shared_ptr<Identifier> array_;
  std::shared_ptr<Expression> index_;
};

inline std::shared_ptr<Access> Access::create(
    std::shared_ptr<Identifier> id,
    std::shared_ptr<Expression> index,
    std::shared_ptr<symbols::Type> type
) {
  return std::make_shared<Access>(id, index, type);
}

inline Access::Access(
    std::shared_ptr<Identifier> id,
    std::shared_ptr<Expression> index,
    std::shared_ptr<symbols::Type> type
)
    : Operator(lexer::Word::create("[]", lexer::Token::kIndex), type), array_(id), index_(index) {
}

inline Access::~Access() {
}

inline std::shared_ptr<Identifier> Access::array() const {
  return array_;
}

inline std::shared_ptr<Expression> Access::index() const {
  return index_;
}

inline std::shared_ptr<Expression> Access::gen(std::stringstream &ss) {
  return Access::create(array_, index_->reduce(ss), type_);
}

inline void Access::jumping(std::stringstream &ss, std::uint32_t to, std::uint32_t from) {
  emit_jumps(ss, reduce(ss)->to_string(), to, from);
}

inline std::string Access::to_string() const {
  return array_->to_string() + "[ " + index_->to_string() + " ]";
}
}  // namespace inter
