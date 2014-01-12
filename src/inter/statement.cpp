#include <inter/statement.hpp>

namespace inter {
const std::shared_ptr<Statement> Statement::kNullStatement = Statement::create();
std::shared_ptr<Statement> Statement::enclosing_statement = Statement::kNullStatement;
} // namespace inter
