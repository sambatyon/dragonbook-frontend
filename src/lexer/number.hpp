#pragma once

#include <lexer/numeric.hpp>

#include <memory>
#include <sstream>
#include <cstdint>

namespace lexer {
typedef NumericType<std::int64_t, Token::kInteger> Number;
} // namespace lexer
