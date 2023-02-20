#include "cpp/inter/constant.hpp"

#include "cpp/lexer/word.hpp"

namespace inter {
std::shared_ptr<Constant> Constant::kTrue =
    Constant::create(lexer::Word::create("true", lexer::Token::kTrue), symbols::Type::create("bool", lexer::Token::kBasic, 1));
std::shared_ptr<Constant> Constant::kFalse =
    Constant::create(lexer::Word::create("false", lexer::Token::kFalse), symbols::Type::create("bool", lexer::Token::kBasic, 1));
}  // namespace inter
