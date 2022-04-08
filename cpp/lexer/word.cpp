#include <lexer/word.hpp>

namespace lexer {
std::shared_ptr<Word> Word::and_word = Word::create("&&", Token::kAnd);
std::shared_ptr<Word> Word::or_word = Word::create("||", Token::kOr);
std::shared_ptr<Word> Word::equal = Word::create("==", Token::kEqual);
std::shared_ptr<Word> Word::not_equal = Word::create("!=", Token::kNotEqual);
std::shared_ptr<Word> Word::less_equal = Word::create("<=", Token::kLessEqual);
std::shared_ptr<Word> Word::greater_equal = Word::create(">=", Token::kGreaterEqual);
std::shared_ptr<Word> Word::minus = Word::create("minus", Token::kMinus);
std::shared_ptr<Word> Word::true_word = Word::create("true", Token::kTrue);
std::shared_ptr<Word> Word::false_word = Word::create("false", Token::kFalse);
std::shared_ptr<Word> Word::temp = Word::create("t", Token::kTemp);
} // namespace lexer
