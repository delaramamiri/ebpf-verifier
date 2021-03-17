// Copyright (c) Prevail Verifier contributors.
// SPDX-License-Identifier: MIT
#pragma once

#include "linear_expression.hpp"

// A linear constraint is of the form:
//    <linear expression> <operator> 0

enum class constraint_kind_t {
    EQUALS_ZERO,
    LESS_THAN_OR_EQUALS_ZERO,
    LESS_THAN_ZERO,
    NOT_ZERO
};

class linear_constraint_t final {
  private:
    linear_expression_t _expression = linear_expression_t(0);
    constraint_kind_t _constraint_kind;

  public:
    linear_constraint_t(const linear_expression_t expression, constraint_kind_t constraint_kind)
    {
        _expression = expression;
        _constraint_kind = constraint_kind;
    }

    [[nodiscard]] const linear_expression_t& expression() const { return _expression; }
    [[nodiscard]] constraint_kind_t kind() const { return _constraint_kind; }

    // Test whether the constraint is guaranteed to be false.
    [[nodiscard]] bool is_contradiction() const {
        if (!_expression.variable_terms().empty()) {
            return false;
        }
        number_t constant = _expression.constant_term();
        switch (_constraint_kind) {
        case constraint_kind_t::EQUALS_ZERO: return (constant == 0);
        case constraint_kind_t::LESS_THAN_OR_EQUALS_ZERO: return (constant <= 0);
        case constraint_kind_t::LESS_THAN_ZERO: return (constant < 0);
        case constraint_kind_t::NOT_ZERO: return (constant != 0);
        default: throw std::exception();
        }
    }

    // Test whether the constraint is guaranteed to be true.
    [[nodiscard]] bool is_tautology() const {
        if (!_expression.variable_terms().empty()) {
            return false;
        }
        return !is_contradiction();
    }

    // Construct the logical NOT of this constraint.
    [[nodiscard]] linear_constraint_t negate() const {
        switch (_constraint_kind) {
        case constraint_kind_t::NOT_ZERO:
            return linear_constraint_t(_expression, constraint_kind_t::EQUALS_ZERO);
        case constraint_kind_t::EQUALS_ZERO:
            return linear_constraint_t(_expression, constraint_kind_t::NOT_ZERO);
        case constraint_kind_t::LESS_THAN_ZERO:
            return linear_constraint_t(-_expression, constraint_kind_t::LESS_THAN_OR_EQUALS_ZERO);
        case constraint_kind_t::LESS_THAN_OR_EQUALS_ZERO:
            return linear_constraint_t(-_expression, constraint_kind_t::LESS_THAN_ZERO);
        default: throw std::exception();
        }
    }
};

// Output a linear constraint to a stream.
inline std::ostream& operator<<(std::ostream& o, const linear_constraint_t& constraint)
{
    if (constraint.is_contradiction()) {
        o << "false";
    } else if (constraint.is_tautology()) {
        o << "true";
    } else {
        // Display constraint as the simpler form of (e.g.):
        //     Ax + By < -C
        // instead of the internal representation of:
        //     Ax + By + C < 0
        auto expression = constraint.expression();
        expression.output_variable_terms(o);

        const char* constraint_kind_label[] = {" == ", " <= ", " < ", " != "};
        o << constraint_kind_label[(int)constraint.kind()] << -expression.constant_term();
    }
    return o;
};
