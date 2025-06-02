#pragma once
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "value.h"

class Environment {
   public:
    Environment() : parent(nullptr) {}

    Environment(std::shared_ptr<Environment> parentEnv)
        : parent(std::move(parentEnv)) {}

    void set(const std::string& name, Value v) {
        if (vars_.find(name) != vars_.end()) {
            vars_[name] = std::move(v);
            return;
        }
        if (parent) {
            try {
                parent->get(name);
                parent->set(name, std::move(v));
                return;
            } catch (std::runtime_error&) {
            }
        }
        vars_[name] = std::move(v);
    }

    Value& get(const std::string& name) {
        auto it = vars_.find(name);
        if (it != vars_.end()) {
            return it->second;
        }
        if (parent) {
            return parent->get(name);
        }
        throw std::runtime_error("Undefined variable '" + name + "'");
    }

    const Value& get(const std::string& name) const {
        auto it = vars_.find(name);
        if (it != vars_.end()) {
            return it->second;
        }
        if (parent) {
            return parent->get(name);
        }
        throw std::runtime_error("Undefined variable '" + name + "'");
    }

   private:
    std::unordered_map<std::string, Value> vars_;
    std::shared_ptr<Environment> parent;
};
