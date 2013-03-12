#pragma once

#include <lexer/numeric.hpp>

#include <memory>
#include <sstream>
#include <cstdint>

namespace lexer {
typedef NumericType<double, Token::kReal> Real;
} // namespace lexer
