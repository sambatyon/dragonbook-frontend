#pragma once

#include <lexer/word.hpp>
#include <lexer/token.hpp>

#include <string>
#include <memory>
#include <cstdint>
#include <utility>
#include <iostream>
#include <unordered_map>

namespace lexer {
class Lexer {
  public:
    static std::uint32_t current_line(); // Eventually, current_line should stop being static
                                         // in order to make the scaner reentrant
    static std::shared_ptr<Lexer> create();

    Lexer();
    ~Lexer();

    std::shared_ptr<Token> scan();

  protected:
    void readch();
    bool readch(char c);

    void reserve(std::shared_ptr<Word> word);

  private:
    std::unordered_map<std::string, std::shared_ptr<Word>> words_;
    char peek_;
    static std::uint32_t current_line_;
};

inline
std::uint32_t Lexer::current_line() {
    return current_line_;
}

inline
std::shared_ptr<Lexer> Lexer::create() {
    return std::make_shared<Lexer>();
}

inline
void Lexer::readch() {
    peek_ = std::cin.get();
}

inline
void Lexer::reserve(std::shared_ptr<Word> word) {
    words_.insert(std::make_pair(word->lexeme(), word));
}
} // namespace lexer
