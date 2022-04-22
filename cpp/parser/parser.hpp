#pragma once

#include <cstdint>
#include <memory>
#include <string>


namespace symbols {
class Type;
class Environment;
}  // namespace symbols

namespace lexer {
class Lexer;
class Token;
}  // namespace lexer

namespace inter {
class Expression;
class Identifier;
class Access;
class Statement;
}  // namespace inter

namespace parser {
class Parser {
 public:
  explicit Parser(std::shared_ptr<lexer::Lexer> lexer);
  void program();

 protected:
  void move();
  void error(std::string what);

  void match(const std::uint32_t &tag);
  std::shared_ptr<inter::Statement> block();
  void decls();

  std::shared_ptr<symbols::Type> type();
  std::shared_ptr<symbols::Type> dimension(std::shared_ptr<symbols::Type>);

  std::shared_ptr<inter::Statement> statements();
  std::shared_ptr<inter::Statement> statement();
  std::shared_ptr<inter::Statement> assign();

  std::shared_ptr<inter::Expression> boolean();
  std::shared_ptr<inter::Expression> join();
  std::shared_ptr<inter::Expression> equality();
  std::shared_ptr<inter::Expression> relational();
  std::shared_ptr<inter::Expression> expression();
  std::shared_ptr<inter::Expression> term();
  std::shared_ptr<inter::Expression> unary();
  std::shared_ptr<inter::Expression> factor();

  std::shared_ptr<inter::Access> offset(std::shared_ptr<inter::Identifier> id);

 private:
  std::shared_ptr<lexer::Lexer> lexer_;
  std::shared_ptr<lexer::Token> lookahead_;
  std::shared_ptr<symbols::Environment> top_;
  std::uint32_t used_;
};
}  // namespace parser
