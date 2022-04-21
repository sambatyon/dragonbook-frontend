#include "cpp/lexer/lexer.hpp"
#include "cpp/parser/parser.hpp"

#include <iostream>

int main(int argc, char **argv) {
    auto lexer = lexer::Lexer::create();
    parser::Parser pars(lexer);
    pars.program();
    std::cout << '\n';
}
