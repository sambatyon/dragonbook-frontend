#pragma once

#include "cpp/dragon/ast/node.hh"
#include "cpp/dragon/lexer/token.hh"
#include "cpp/dragon/symbols/symbols.hh"

#include <atomic>
#include <cstdint>
#include <iostream>
#include <memory>
#include <sstream>

namespace dragon {
namespace ast {
class Expr: public Node {
 public:
  Expr(lexer::Token op, std::unique_ptr<symbols::Type> &type);
  Expr(lexer::Token op, std::unique_ptr<symbols::Type> &&type);

  lexer::Token op() const { return op_; }
  const symbols::Type *const type() const { return type_.get(); }

  virtual std::unique_ptr<Expr> gen(std::ostream &) {
    return std::make_unique<Expr>(op_, type_->clone());
  }
  virtual std::unique_ptr<Expr> reduce(std::ostream &) {
    return std::make_unique<Expr>(op_, type_->clone());
  }

  virtual const std::string to_string() const { return op_.to_string(); }

  virtual void jumping(std::ostream &o, int to, int from);
  virtual void emit_jumps(std::ostream &o, const std::string &test, int to, int from);

 protected:
  const lexer::Token op_;
  const std::unique_ptr<symbols::Type> type_;
};

class ID: public Expr {
 public:
  ID(lexer::Token id, std::unique_ptr<symbols::Type> &type, int offset);
  ID(lexer::Token id, std::unique_ptr<symbols::Type> &&type, int offset);

  int offset() const { return offset_; }

 private:
  int offset_;
};

class Const: public Expr {
 public:
  Const(lexer::Token val, std::unique_ptr<symbols::Type> &type);
  Const(lexer::Token val, std::unique_ptr<symbols::Type> &&type);

  bool is_true(const Const &c) const;
  static std::unique_ptr<Const> true_const();
  bool is_false(const Const &c) const;
  static std::unique_ptr<Const> false_const();

  static std::unique_ptr<Const> num_const(std::int64_t val);
  static std::unique_ptr<Const> real_const(double val);

  virtual void jumping(std::ostream &o, int to, int from) override;

  bool operator==(const Const &other) const;
  bool operator!=(const Const &other) const {
    return !(*this == other);
  }
};

class Temp: public Expr {
 public:
  Temp(std::unique_ptr<symbols::Type> &type);
  Temp(std::unique_ptr<symbols::Type> &&type);

  virtual const std::string to_string() const override;

  static void reset_temp_count();

 private:
  static std::atomic<std::uint32_t> count_;
  std::uint32_t number_;
};

class Operator: public Expr {
 public:
  Operator(lexer::Token op, std::unique_ptr<symbols::Type> &type);
  Operator(lexer::Token op, std::unique_ptr<symbols::Type> &&type);
  virtual ~Operator() = default;

  virtual std::unique_ptr<Expr> reduce(std::ostream &) override;
};

class Arithmetic: public Operator {
 public:
  Arithmetic(lexer::Token op, std::unique_ptr<Expr> &left, std::unique_ptr<Expr> &right);
  Arithmetic(lexer::Token op, std::unique_ptr<Expr> &&left, std::unique_ptr<Expr> &&right);

  virtual const std::string to_string() const override;

  virtual std::unique_ptr<Expr> gen(std::ostream &) override;

 private:
  std::unique_ptr<Expr> left_;
  std::unique_ptr<Expr> right_;
};

class Unary: public Operator {
 public:
  Unary(lexer::Token op, std::unique_ptr<Expr> &expr);
  Unary(lexer::Token op, std::unique_ptr<Expr> &&expr);

  virtual const std::string to_string() const override;

  virtual std::unique_ptr<Expr> gen(std::ostream &) override;

 private:
  std::unique_ptr<Expr> expr_;
};

class Access: public Operator {
public:
  Access(std::unique_ptr<ID> &array, std::unique_ptr<Expr> &index, std::unique_ptr<symbols::Type> &type);
  Access(std::unique_ptr<ID> &&array, std::unique_ptr<Expr> &&index, std::unique_ptr<symbols::Type> &&type);

  const ID *const array() const { return array_.get(); }
  const Expr *const index() const { return index_.get(); }

  virtual const std::string to_string() const override;
  virtual void jumping(std::ostream &o, int to, int from) override;
  virtual std::unique_ptr<Expr> gen(std::ostream &) override;
private:
  std::unique_ptr<ID> array_;
  std::unique_ptr<Expr> index_;
};

class Logical: public Expr {
public:
  Logical(lexer::Token op, std::unique_ptr<Expr> &left, std::unique_ptr<Expr> &right);
  Logical(lexer::Token op, std::unique_ptr<Expr> &&left, std::unique_ptr<Expr> &&right);

  virtual const std::string to_string() const override;
  virtual std::unique_ptr<Expr> gen(std::ostream &) override;

  static std::unique_ptr<symbols::Type> check(const symbols::Type *const left, const symbols::Type *const right);

protected:
  std::unique_ptr<Expr> left_;
  std::unique_ptr<Expr> right_;
};

class Not: public Expr {
 public:
  Not(lexer::Token op, std::unique_ptr<Expr> &expr);
  Not(lexer::Token op, std::unique_ptr<Expr> &&expr);

  virtual const std::string to_string() const override;
  virtual void jumping(std::ostream &o, int to, int from) override;
  virtual std::unique_ptr<Expr> gen(std::ostream &) override;
 private:
  std::unique_ptr<Expr> expr_;
};

class Or: public Logical {
 public:
  Or(std::unique_ptr<Expr> &left, std::unique_ptr<Expr> &right);
  Or(std::unique_ptr<Expr> &&left, std::unique_ptr<Expr> &&right);

  virtual void jumping(std::ostream &o, int to, int from) override;
};

class And: public Logical {
 public:
  And(std::unique_ptr<Expr> &left, std::unique_ptr<Expr> &right);
  And(std::unique_ptr<Expr> &&left, std::unique_ptr<Expr> &&right);

  virtual void jumping(std::ostream &o, int to, int from) override;
};

class Relational: public Logical {
 public:
  Relational(lexer::Token op, std::unique_ptr<Expr> &left, std::unique_ptr<Expr> &right);
  Relational(lexer::Token op, std::unique_ptr<Expr> &&left, std::unique_ptr<Expr> &&right);

  virtual void jumping(std::ostream &o, int to, int from) override;
};
} // namespace ast
} // namespace dragon
