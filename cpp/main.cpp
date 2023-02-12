#include "cpp/lexer/lexer.hpp"
#include "cpp/parser/parser.hpp"

#include <iostream>

int main(int argc, char **argv) {
    auto lexer = lexer::Lexer::create(std::cin);
    parser::Parser pars(lexer);
    auto program = pars.program();
    std::cout << program << '\n';
}
