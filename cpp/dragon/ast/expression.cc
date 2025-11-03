#include "cpp/dragon/ast/expression.hh"

namespace dragon {
namespace ast {
Expr::Expr(lexer::Token op, std::unique_ptr<symbols::Type> &type)
  : op_(op), type_(std::move(type)) {}

Expr::Expr(lexer::Token op, std::unique_ptr<symbols::Type> &&type)
: op_(op), type_(std::move(type)) {}

void Expr::jumping(std::ostream &o, int to, int from) {
  emit_jumps(o, to_string(), to, from);
}

void Expr::emit_jumps(std::ostream &o, const std::string &test, int to, int from) {
  std::stringstream ss;
  if (to != 0 && from != 0) {
    emit(o, "if " + test + " goto L" + std::to_string(to));
    emit(o, "goto L" + std::to_string(from));
  } else if (to != 0) {
    emit(o, "if " + test + " goto L" + std::to_string(to));
  } else if (from != 0) {
    emit(o, "iffalse " + test + " goto L" + std::to_string(from));
  }
}

ID::ID(lexer::Token id, std::unique_ptr<symbols::Type> &type, int offset)
  : Expr(id, type), offset_(offset) {}

ID::ID(lexer::Token id, std::unique_ptr<symbols::Type> &&type, int offset)
  : Expr(id, std::move(type)), offset_(offset) {}


Const::Const(lexer::Token val, std::unique_ptr<symbols::Type> &type)
  : Expr(val, type) {}
Const::Const(lexer::Token val, std::unique_ptr<symbols::Type> &&type)
  : Expr(val, std::move(type)) {}

void Const::jumping(std::ostream &o, int to, int from) {
  if (is_true(*this) && to != 0) {
    ast::emit(o, "goto L" + std::to_string(to));
  }
  if (is_false(*this) && from != 0) {
    ast::emit(o, "goto L" + std::to_string(from));
  }
}

bool Const::operator==(const Const &other) const {
  return op_ == other.op_ && *type_.get() == *other.type_.get();
}

bool Const::is_true(const Const &c) const {
  static const Const tr(lexer::operators::kTrueWord, symbols::Type::kBoolean.clone());
  return c == tr;
}

std::unique_ptr<Const> Const::true_const() {
  return std::make_unique<Const>(lexer::operators::kTrueWord, symbols::Type::kBoolean.clone());
}

bool Const::is_false(const Const &c) const {
  static const Const fl(lexer::operators::kFalseWord, symbols::Type::kBoolean.clone());
  return c == fl;
}

std::unique_ptr<Const> Const::false_const() {
  return std::make_unique<Const>(lexer::operators::kFalseWord, symbols::Type::kBoolean.clone());
}
std::unique_ptr<Const> Const::num_const(std::int64_t val) {
  return std::make_unique<Const>(lexer::Token(lexer::tag::kInteger, val), symbols::Type::kNumber.clone());
}
std::unique_ptr<Const> Const::real_const(double val) {
  return std::make_unique<Const>(lexer::Token(lexer::tag::kReal, val), symbols::Type::kNumber.clone());
}

std::atomic<std::uint32_t> Temp::count_{0};

Temp::Temp(std::unique_ptr<symbols::Type> &type)
  : Expr(lexer::operators::kTemp, type), number_(++Temp::count_) {}
Temp::Temp(std::unique_ptr<symbols::Type> &&type)
  : Expr(lexer::operators::kTemp, std::move(type)), number_(++Temp::count_) {}

const std::string Temp::to_string() const {
  return "t" + std::to_string(number_);
}

void Temp::reset_temp_count() {
  Temp::count_ = 0;
}

Operator::Operator(lexer::Token op, std::unique_ptr<symbols::Type> &type)
  : Expr(op, type) {}
Operator::Operator(lexer::Token op, std::unique_ptr<symbols::Type> &&type)
  : Expr(op, std::move(type)) {}

std::unique_ptr<Expr> Operator::reduce(std::ostream &os) {
  auto expr = gen(os);
  auto tmp = std::make_unique<Temp>(type()->clone());
  emit(os, tmp->to_string() + " = " + expr->to_string());
  return tmp;
}

Arithmetic::Arithmetic(lexer::Token op, std::unique_ptr<Expr> &left, std::unique_ptr<Expr> &right)
  : Operator(op, symbols::Type::max(left->type(), right->type())), left_(std::move(left)), right_(std::move(right)) {
  if (type() == nullptr) {
    throw std::runtime_error("Arithmetic: type is null");
  }
}
Arithmetic::Arithmetic(lexer::Token op, std::unique_ptr<Expr> &&left, std::unique_ptr<Expr> &&right)
  : Operator(op, symbols::Type::max(left->type(), right->type())), left_(std::move(left)), right_(std::move(right)) {
  if (type() == nullptr) {
    throw std::runtime_error("Arithmetic: type is null");
  }
}

const std::string Arithmetic::to_string() const {
 return left_->to_string() + " " + op_.to_string() + " " + right_->to_string();
}

std::unique_ptr<Expr> Arithmetic::gen(std::ostream &os) {
  return std::make_unique<Arithmetic>(op_, left_->reduce(os), right_->reduce(os));
}

Unary::Unary(lexer::Token op, std::unique_ptr<Expr> &expr)
  : Operator(op, symbols::Type::max(&symbols::Type::kNumber, expr->type())), expr_(std::move(expr)) {
  if (type() == nullptr) {
    throw std::runtime_error("Unary: type is null");
  }
}
Unary::Unary(lexer::Token op, std::unique_ptr<Expr> &&expr)
  : Operator(op, symbols::Type::max(&symbols::Type::kNumber, expr->type())), expr_(std::move(expr)) {
  if (type() == nullptr) {
    throw std::runtime_error("Unary: type is null");
  }
}

const std::string Unary::to_string() const {
  return op_.to_string() + " " + expr_->to_string();
}

std::unique_ptr<Expr> Unary::gen(std::ostream &os) {
  return std::make_unique<Unary>(op_, expr_->reduce(os));
}

Access::Access(std::unique_ptr<ID> &array, std::unique_ptr<Expr> &index, std::unique_ptr<symbols::Type> &type)
  : Operator(lexer::operators::kAccess, std::move(type)), array_(std::move(array)), index_(std::move(index)) {}
Access::Access(std::unique_ptr<ID> &&array, std::unique_ptr<Expr> &&index, std::unique_ptr<symbols::Type> &&type)
  : Operator(lexer::operators::kAccess, std::move(type)), array_(std::move(array)), index_(std::move(index)) {}

const std::string Access::to_string() const {
  return array_->to_string() + "[ " + index_->to_string() + " ]";
}
void Access::jumping(std::ostream &os, int to, int from) {
  emit_jumps(os, reduce(os)->to_string(), to, from);
}
std::unique_ptr<Expr> Access::gen(std::ostream &os) {
  return std::make_unique<Access>(
    std::make_unique<ID>(
      array_->op(),
      array_->type()->clone(),
      array_->offset()
    ),
    index_->reduce(os),
    type()->clone()
  );
}


Logical::Logical(lexer::Token op, std::unique_ptr<Expr> &left, std::unique_ptr<Expr> &right)
  : Expr(op, Logical::check(left->type(), right->type())), left_(std::move(left)), right_(std::move(right)) {
  if (type() == nullptr) {
    throw std::runtime_error("Logical: type is null");
  }
}
Logical::Logical(lexer::Token op, std::unique_ptr<Expr> &&left, std::unique_ptr<Expr> &&right)
  : Expr(op, Logical::check(left->type(), right->type())), left_(std::move(left)), right_(std::move(right)) {
  if (type() == nullptr) {
    throw std::runtime_error("Logical: type is null");
  }
}

const std::string Logical::to_string() const {
  return left_->to_string() + " " + op_.to_string() + " " + right_->to_string();
}

std::unique_ptr<Expr> Logical::gen(std::ostream &os) {
  auto f = new_label();
  auto a = new_label();
  auto tmp = std::make_unique<Temp>(type()->clone());
  jumping(os, 0, f);
  emit(os, tmp->to_string() + " = true");
  emit(os, "goto L" + std::to_string(a));
  emit_label(os, f);
  emit(os, tmp->to_string() + " = false");
  emit_label(os, a);
  return tmp;
}

std::unique_ptr<symbols::Type> Logical::check(const symbols::Type *const left, const symbols::Type *const right) {
  if (symbols::Type::kBoolean == *left && symbols::Type::kBoolean == *right) {
    return symbols::Type::kBoolean.clone();
  }
  return std::unique_ptr<symbols::Type>();
}

Not::Not(lexer::Token op, std::unique_ptr<Expr> &expr)
  : Expr(op, expr->type()->clone()), expr_(std::move(expr)) {
  if (symbols::Type::kBoolean != *expr_->type()) {
    throw std::runtime_error("Not: type is not boolean");
  }
}
Not::Not(lexer::Token op, std::unique_ptr<Expr> &&expr)
  : Expr(op, expr->type()->clone()), expr_(std::move(expr)) {
  if (symbols::Type::kBoolean != *expr_->type()) {
    throw std::runtime_error("Not: type is not boolean");
  }
}

const std::string Not::to_string() const {
  return op_.to_string() + " " + expr_->to_string();
}
void Not::jumping(std::ostream &os, int to, int from) {
  return expr_->jumping(os, from, to);
}
std::unique_ptr<Expr> Not::gen(std::ostream &os) {
  auto f = new_label();
  auto a = new_label();
  auto tmp = std::make_unique<Temp>(type()->clone());
  jumping(os, 0, f);
  emit(os, tmp->to_string() + " = true");
  emit(os, "goto L" + std::to_string(a));
  emit_label(os, f);
  emit(os, tmp->to_string() + " = false");
  emit_label(os, a);
  return tmp;
}

Or::Or(std::unique_ptr<Expr> &left, std::unique_ptr<Expr> &right)
  : Logical(lexer::operators::kOrWord, std::move(left), std::move(right)) {}
Or::Or(std::unique_ptr<Expr> &&left, std::unique_ptr<Expr> &&right)
  : Logical(lexer::operators::kOrWord, std::move(left), std::move(right)) {}

void Or::jumping(std::ostream &o, int to, int from) {
  auto label = to != 0 ? to : new_label();
  left_->jumping(o, label, 0);
  right_->jumping(o, to, from);
  if (to == 0) {
    emit_label(o, label);
  }
}

And::And(std::unique_ptr<Expr> &left, std::unique_ptr<Expr> &right)
  : Logical(lexer::operators::kAndWord, std::move(left), std::move(right)) {}
And::And(std::unique_ptr<Expr> &&left, std::unique_ptr<Expr> &&right)
  : Logical(lexer::operators::kAndWord, std::move(left), std::move(right)) {}

void And::jumping(std::ostream &o, int to, int from) {
  auto label = from != 0 ? from : new_label();
  left_->jumping(o, 0, label);
  right_->jumping(o, to, from);
  if (from == 0) {
    emit_label(o, label);
  }
}
} // namespace ast
} // namespace dragon
