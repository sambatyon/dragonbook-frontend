#include "cpp/parser/parser.hpp"

#include <exception>
#include <sstream>

#include "cpp/inter/access.hpp"
#include "cpp/inter/and.hpp"
#include "cpp/inter/arithmetic-operator.hpp"
#include "cpp/inter/break.hpp"
#include "cpp/inter/constant.hpp"
#include "cpp/inter/do.hpp"
#include "cpp/inter/else.hpp"
#include "cpp/inter/expression.hpp"
#include "cpp/inter/identifier.hpp"
#include "cpp/inter/if.hpp"
#include "cpp/inter/not.hpp"
#include "cpp/inter/or.hpp"
#include "cpp/inter/relational.hpp"
#include "cpp/inter/set-elem.hpp"
#include "cpp/inter/set.hpp"
#include "cpp/inter/statement-sequence.hpp"
#include "cpp/inter/statement.hpp"
#include "cpp/inter/unary.hpp"
#include "cpp/inter/while.hpp"

#include "cpp/lexer/array.hpp"
#include "cpp/lexer/lexer.hpp"
#include "cpp/lexer/numeric.hpp"
#include "cpp/lexer/token.hpp"
#include "cpp/lexer/type.hpp"
#include "cpp/parser/environment.hpp"

namespace parser {
Parser::Parser(std::shared_ptr<lexer::Lexer> lexer) : lexer_(lexer), lookahead_(), top_(), used_(0) {
  move();
}

std::string Parser::program() {
  auto stmts = block();
  auto begin = stmts->new_label();
  auto after = stmts->new_label();
  std::stringstream program;
  stmts->emit_label(program, begin);
  stmts->gen(program, begin, after);
  stmts->emit_label(program, after);
  return program.str();
}

void Parser::move() {
  lookahead_ = lexer_->scan();
}

void Parser::error(std::string what) {
  std::stringstream ss;
  ss << "Near line " << lexer_->current_line() << ": " << what;
  std::cerr << ss.str() << '\n';
  throw std::runtime_error(ss.str().c_str());
}

void Parser::match(std::uint32_t tag) {
  if (lookahead_->tag() == tag) {
    move();
  } else {
    error("Syntax error");
  }
}

std::shared_ptr<inter::Statement> Parser::block() {
  match('{');
  std::shared_ptr<symbols::Environment> saved_environment = top_;
  top_ = std::make_shared<symbols::Environment>(top_);
  decls();
  auto stmts = statements();
  match('}');
  top_ = saved_environment;
  return stmts;
}

void Parser::decls() {
  while (lookahead_->tag() == lexer::Token::kBasic) {
    auto typ = type();
    auto token = lookahead_;
    match(lexer::Token::kIdentifier);
    match(';');
    auto id = inter::Identifier::create(std::dynamic_pointer_cast<lexer::Word>(token), typ, used_);
    top_->put(token, id);
    used_ += static_cast<std::uint32_t>(typ->width());
  }
}

std::shared_ptr<symbols::Type> Parser::type() {
  auto typ = std::dynamic_pointer_cast<symbols::Type>(lookahead_);  // Expect look.tag == Token::kBasic
  match(lexer::Token::kBasic);
  if (lookahead_->tag() != '[')
    return typ;
  else
    return dimension(typ);
}

std::shared_ptr<symbols::Type> Parser::dimension(std::shared_ptr<symbols::Type> typ) {
  match('[');
  auto token = lookahead_;
  match(lexer::Token::kInteger);
  match(']');
  if (lookahead_->tag() == '[')
    typ = dimension(typ);
  return symbols::Array::create(std::dynamic_pointer_cast<lexer::Number>(token)->value(), typ);
}

std::shared_ptr<inter::Statement> Parser::statements() {
  if (lookahead_->tag() == '}') {
    return inter::Statement::kNullStatement;
  } else {
    auto stmt1 = statement();
    auto stmt2 = statements();
    return inter::StatementSequence::create(stmt1, stmt2);
  }
}

std::shared_ptr<inter::Statement> Parser::statement() {
  std::shared_ptr<inter::Expression> expr;
  std::shared_ptr<inter::Statement> stmt, stmt1, stmt2;
  std::shared_ptr<inter::Statement> saved_statement;
  switch (lookahead_->tag()) {
    case ';':
      move();
      return inter::Statement::kNullStatement;

    case lexer::Token::kIf:
      match(lexer::Token::kIf);
      match('(');
      expr = boolean();
      match(')');
      stmt1 = statement();
      if (lookahead_->tag() != lexer::Token::kElse)
        return inter::If::create(expr, stmt1);
      match(lexer::Token::kElse);
      stmt2 = statement();
      return inter::Else::create(expr, stmt1, stmt2);

    case lexer::Token::kWhile: {
      auto while_node = inter::While::create();
      saved_statement = inter::Statement::enclosing_statement;
      inter::Statement::enclosing_statement = while_node;
      match(lexer::Token::kWhile);
      match('(');
      expr = boolean();
      match(')');
      stmt1 = statement();
      while_node->init(expr, stmt1);
      inter::Statement::enclosing_statement = saved_statement;
      return while_node;
    }
    case lexer::Token::kDo: {
      auto do_node = inter::Do::create();
      saved_statement = inter::Statement::enclosing_statement;
      inter::Statement::enclosing_statement = do_node;
      match(lexer::Token::kDo);
      stmt1 = statement();
      match(lexer::Token::kWhile);
      match('(');
      expr = boolean();
      match(')');
      match(';');
      do_node->init(stmt1, expr);
      inter::Statement::enclosing_statement = saved_statement;
      return do_node;
    }

    case lexer::Token::kBreak:
      match(lexer::Token::kBreak);
      match(';');
      return inter::Break::create();

    case '{':
      return block();

    default:
      return assign();
  }
}

std::shared_ptr<inter::Statement> Parser::assign() {
  std::shared_ptr<inter::Statement> stmt;
  std::shared_ptr<lexer::Token> token = lookahead_;
  match(lexer::Token::kIdentifier);
  auto id = top_->get(token);
  if (!id)
    error(token->to_string() + " undeclared");

  if (lookahead_->tag() == '=') {
    move();
    stmt = inter::Set::create(id, boolean());
  } else {
    auto access = offset(id);
    match('=');
    stmt = inter::SetElem::create(access, boolean());
  }
  match(';');
  return stmt;
}

std::shared_ptr<inter::Expression> Parser::boolean() {
  auto expr = join();
  while (lookahead_->tag() == lexer::Token::kOr) {
    auto token = lookahead_;
    move();
    expr = inter::Or::create(token, expr, relational());
  }
  return expr;
}

std::shared_ptr<inter::Expression> Parser::join() {
  auto expr = equality();
  while (lookahead_->tag() == lexer::Token::kAnd) {
    auto token = lookahead_;
    move();
    expr = inter::And::create(token, expr, relational());
  }
  return expr;
}

std::shared_ptr<inter::Expression> Parser::equality() {
  auto expr = relational();
  while (lookahead_->tag() == lexer::Token::kEqual || lookahead_->tag() == lexer::Token::kNotEqual) {
    auto token = lookahead_;
    move();
    expr = inter::Relational::create(token, expr, expression());
  }
  return expr;
}

std::shared_ptr<inter::Expression> Parser::relational() {
  auto expr = expression();
  switch (lookahead_->tag()) {
    case '<':
    case lexer::Token::kLessEqual:
    case lexer::Token::kGreaterEqual:
    case '>': {
      auto token = lookahead_;
      move();
      return inter::Relational::create(token, expr, expression());
    }
    default:
      return expr;
  }
}

std::shared_ptr<inter::Expression> Parser::expression() {
  auto expr = term();
  while (lookahead_->tag() == '+' || lookahead_->tag() == '-') {
    auto token = lookahead_;
    move();
    expr = inter::Arithmetic::create(token, expr, term());
  }
  return expr;
}

std::shared_ptr<inter::Expression> Parser::term() {
  auto expr = unary();
  while (lookahead_->tag() == '*' || lookahead_->tag() == '/') {
    auto token = lookahead_;
    move();
    expr = inter::Arithmetic::create(token, expr, unary());
  }
  return expr;
}

std::shared_ptr<inter::Expression> Parser::unary() {
  if (lookahead_->tag() == '-') {
    move();
    return inter::UnaryOperator::create(lexer::Word::minus, unary());
  } else if (lookahead_->tag() == '!') {
    auto token = lookahead_;
    move();
    return inter::Not::create(token, unary());
  } else {
    return factor();
  }
}

std::shared_ptr<inter::Expression> Parser::factor() {
  auto expr = std::shared_ptr<inter::Expression>();
  switch (lookahead_->tag()) {
    case '(':
      move();
      expr = boolean();
      match(')');
      return expr;
    case lexer::Token::kInteger:
      expr = inter::Constant::create(lookahead_, symbols::Type::integer);
      move();
      return expr;
    case lexer::Token::kReal:
      expr = inter::Constant::create(lookahead_, symbols::Type::real);
      move();
      return expr;
    case lexer::Token::kTrue:
      expr = inter::Constant::kTrue;
      move();
      return expr;
    case lexer::Token::kFalse:
      expr = inter::Constant::kFalse;
      move();
      return expr;
    case lexer::Token::kIdentifier: {
      auto s = lookahead_->to_string();
      auto id = top_->get(lookahead_);
      if (!id)
        error(lookahead_->to_string() + " undeclared");
      move();
      if (lookahead_->tag() != '[')
        return id;
      else
        return offset(id);
    }
    default:
      error("syntax error");
      return expr;
  }
}

std::shared_ptr<inter::Access> Parser::offset(std::shared_ptr<inter::Identifier> id) {
  std::shared_ptr<inter::Expression> width;
  std::shared_ptr<inter::Expression> term1, term2;
  std::shared_ptr<inter::Expression> location;

  auto typ = id->type();
  match('[');
  auto index = boolean();
  match(']');
  typ = std::dynamic_pointer_cast<symbols::Array>(typ)->type();
  width = inter::Constant::create(typ->width());
  term1 = inter::Arithmetic::create(lexer::Token::create('*'), index, width);
  location = term1;
  while (lookahead_->tag() == '[') {
    match('[');
    index = boolean();
    match(']');
    typ = std::dynamic_pointer_cast<symbols::Array>(typ)->type();
    width = inter::Constant::create(typ->width());
    term1 = inter::Arithmetic::create(lexer::Token::create('*'), index, width);
    term2 = inter::Arithmetic::create(lexer::Token::create('+'), location, term1);
    location = term2;
  }
  return inter::Access::create(id, location, typ);
}
}  // namespace parser
