#include <lexer/lexer.hpp>

#include <lexer/numeric.hpp>

#include <symbols/type.hpp>

#include <cctype>
#include <memory>

namespace lexer {
std::uint32_t Lexer::current_line_ = 1;

Lexer::Lexer() : words_(), peek_(' ') {
    reserve(std::make_shared<Word>("if", Token::kIf));
    reserve(std::make_shared<Word>("else", Token::kElse));
    reserve(std::make_shared<Word>("while", Token::kWhile));
    reserve(std::make_shared<Word>("do", Token::kDo));
    reserve(std::make_shared<Word>("break", Token::kBreak));

    reserve(Word::true_word);
    reserve(Word::false_word);

    reserve(symbols::Type::integer);
    reserve(symbols::Type::real);
    reserve(symbols::Type::boolean);
    reserve(symbols::Type::character);
}

Lexer::~Lexer() {
}

bool Lexer::readch(char c) {
    readch();
    if (peek_ != c)
        return false;
    peek_ = ' ';
    return true;
}

std::shared_ptr<Token> Lexer::scan() {
    for (int i = 1; i > 0; this->readch()) {
        if (peek_ == ' ' || peek_ == '\t')
            continue;
        else if (peek_ == '\n')
            ++current_line_;
        else
            break;
    }
    switch (peek_) {
        case '&':
            if (readch('&'))
                return Word::and_word;
            else
                return Token::create(static_cast<std::uint32_t>('&')); // consider pooling this
            break;
        case '|':
            if (readch('|'))
                return Word::or_word;
            else
                return Token::create(static_cast<std::uint32_t>('|'));
            break;
        case '=':
            if (readch('='))
                return Word::equal;
            else
                return Token::create(static_cast<std::uint32_t>('='));
            break;
        case '!':
            if (readch('='))
                return Word::not_equal;
            else
                return Token::create(static_cast<std::uint32_t>('!'));
            break;
        case '<':
            if (readch('='))
                return Word::less_equal;
            else
                return Token::create(static_cast<std::uint32_t>('<'));
            break;
        case '>':
            if (readch('='))
                return Word::greater_equal;
            else
                return Token::create(static_cast<std::uint32_t>('>'));
            break;
    }
    if (std::isdigit(peek_)) {
        std::int64_t value = 0;
        do {
            value = 10ull * value + static_cast<std::int64_t>(peek_ - '0');
            readch();
        } while (std::isdigit(peek_));
        if (peek_ != '.')
            return std::make_shared<Number>(value);
        auto dvalue = static_cast<double>(value);
        auto power = 10.0;
        for (;;) {
            readch();
            if (!std::isdigit(peek_))
                break;
            dvalue = dvalue + static_cast<double>(peek_ - '0') / power;
            power *= 10.0;
        }
        return std::make_shared<Real>(dvalue);
    }
    if (std::isalpha(peek_)) {
        std::string str;
        do {
            str += peek_;
            readch();
        } while (std::isalnum(peek_));
        auto word_it = words_.find(str);
        if (word_it != words_.end())
            return word_it->second;
        auto word = Word::create(str, Token::kIdentifier);
        words_.insert(std::make_pair(str, word));
        return word;
    }
    auto token = Token::create(static_cast<std::uint32_t>(peek_));
    peek_ = ' ';
    return token;
}
} // namespace lexer
