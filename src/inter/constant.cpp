#include <inter/constant.hpp>

#include <lexer/word.hpp>

namespace inter {
std::shared_ptr<Constant> Constant::kTrue = Constant::create(lexer::Token::create(lexer::Word::kTrue),
                                                             symbols::Type::boolean);
std::shared_ptr<Constant> Constant::kFalse = Constant::create(lexer::Token::create(lexer::Word::kFalse),
                                                              symbols::Type::boolean);
} // namespace inter
