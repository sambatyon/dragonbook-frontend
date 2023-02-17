#include "cpp/inter/temporary.hpp"

namespace inter {
thread_local std::uint32_t Temporary::count_ = 0;
}  // namespace inter
