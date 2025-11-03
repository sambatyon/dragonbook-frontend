#include "cpp/dragon/lexer/lexer.hh"

namespace dragon {
namespace lexer {
Lexer::Lexer(std::istream &source): source_(source), words_(), peek_(' ') {
  reserve(Token(tag::kIf, "if"));
  reserve(Token(tag::kElse, "else"));
  reserve(Token(tag::kWhile, "while"));
  reserve(Token(tag::kDo, "do"));
  reserve(Token(tag::kBreak, "break"));

  reserve(operators::kTrueWord);
  reserve(operators::kFalseWord);

  reserve(types::kInt);
  reserve(types::kFloat);
  reserve(types::kBool);
  reserve(types::kChar);
}

Lexer::~Lexer() {}

bool Lexer::readch(char c) {
  readch();
  if (peek_ != c) {
    return false;
  }
  peek_ = ' ';
  return true;
}

Token Lexer::scan() {
  for (;; readch()) {
    if (peek_ == ' ' || peek_ == '\t') {
      continue;
    } else if (peek_ == '\n') {
      ++current_line_;
    } else {
      break;
    }
  }
  switch (peek_) {
  case '&':
    if (readch('&')) {
      return operators::kAndWord;
    } else {
      return Token(static_cast<std::uint32_t>('&'));
    }
    break;
  case '|':
    if (readch('|')) {
      return operators::kOrWord;
    } else {
      return Token(static_cast<std::uint32_t>('|'));
    }
    break;
  case '=':
    if (readch('=')) {
      return operators::kEqual;
    } else {
      return Token(static_cast<std::uint32_t>('='));
    }
    break;
  case '!':
    if (readch('=')) {
      return operators::kNotEqual;
    } else {
      return Token(static_cast<std::uint32_t>('!'));
    }
    break;
  case '<':
    if (readch('=')) {
      return operators::kLessEqual;
    } else {
      return Token(static_cast<std::uint32_t>('<'));
    }
    break;
  case '>':
    if (readch('=')) {
      return operators::kGreaterEqual;
    } else {
      return Token(static_cast<std::uint32_t>('>'));
    }
    break;
  }
  if (std::isdigit(peek_)) {
    std::int64_t value = 0;
    do {
      value = 10ull * value + static_cast<std::int64_t>(peek_-'0');
      readch();
    } while (std::isdigit(peek_));
    if (peek_ != '.') {
      return Token(tag::kInteger, value);
    }
    auto dval = static_cast<double>(value);
    auto power = 10.0;
    for (;;) {
      readch();
      if (!std::isdigit(peek_)) {
        break;
      }
      dval += static_cast<double>(peek_-'0') / power;
      power *= 10.0;
    }
    return Token(tag::kReal, dval);
  }
  if (std::isalpha(peek_)) {
    std::string str;
    do {
      str += peek_;
      readch();
    } while (std::isalnum(peek_));
    auto word_it = words_.find(str);
    if (word_it != std::end(words_)) {
      return word_it->second;
    }
    auto word = Token(tag::kIdentifier, str);
    return word;
  }
  Token token(static_cast<uint32_t>(peek_));
  peek_ = ' ';
  return token;
}
} // namespace lexer
} // namespace dragon
