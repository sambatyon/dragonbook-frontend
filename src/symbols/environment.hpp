#pragma once

#include <lexer/token.hpp>
#include <inter/identifier.hpp>

#include <map>
#include <memory>
#include <utility>

namespace symbols {
class Environment : std::enable_shared_from_this<Environment> {
  public:
    static std::shared_ptr<Environment> create(std::shared_ptr<Environment> previous);
    explicit Environment(std::shared_ptr<Environment> previous);

    void put(std::shared_ptr<lexer::Token> token, std::shared_ptr<inter::Identifier> id);

    std::shared_ptr<inter::Identifier> get(std::shared_ptr<lexer::Token> token) const;

  protected:
    std::shared_ptr<Environment> previous_;

  private:
    std::map<std::shared_ptr<lexer::Token>, std::shared_ptr<inter::Identifier>> table_;
};

inline
std::shared_ptr<Environment> Environment::create(std::shared_ptr<Environment> previous) {
    return std::make_shared<Environment>(previous);
}

inline
Environment::Environment(std::shared_ptr<Environment> previous) : previous_(previous), table_() {
}

inline
void Environment::put(std::shared_ptr<lexer::Token> token, std::shared_ptr<inter::Identifier> id) {
    table_.insert(std::make_pair(token, id));
}

inline
std::shared_ptr<inter::Identifier> Environment::get(std::shared_ptr<lexer::Token> token) const {
    for (auto env = this->shared_from_this(); env.get() != nullptr; env = env->previous_) {
        auto found = env->table_.find(token);
        if (found != env->table_.end())
            return found->second;
    }
    return std::shared_ptr<inter::Identifier>();
}
} // namespace symbols
