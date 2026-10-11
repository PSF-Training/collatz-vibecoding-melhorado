#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

/// Collatz sequence generation for a series of positive start values.
namespace collatz {

/// Fixed-width integer used for every value produced by the Collatz map.
using value_type = std::uint64_t;

/// Default ceiling on transitions, guarding against a pathological or endless run.
inline constexpr std::size_t kDefaultMaxSteps = 10'000'000;

/// Reason a generated sequence stopped advancing.
enum class Status {
  Ok,         ///< Reached 1, the conjectured terminus of every Collatz orbit.
  Overflow,   ///< A 3n+1 step would exceed the representable 64-bit range.
  Truncated,  ///< The configured maximum step count was reached first.
  Invalid,    ///< The start value was not a positive integer.
};

/// One Collatz orbit together with the reason generation stopped.
struct Sequence {
  value_type start = 0;            ///< Value the orbit was generated from.
  std::vector<value_type> values;  ///< Visited values in order, beginning with `start`.
  Status status = Status::Ok;      ///< Why generation stopped.
};

/// Aggregate metrics for a single orbit.
struct Statistics {
  std::size_t steps = 0;         ///< Transitions applied before stopping.
  std::size_t stopping_time = 0;  ///< Transitions until the orbit first drops below its start.
  value_type max_value = 0;       ///< Largest value seen, including the start value.
  Status status = Status::Ok;     ///< Why generation stopped.
};

/// Reports whether `n` is a legal Collatz start value, that is any positive integer.
bool is_valid_start(value_type n) noexcept;

/// Generates the Collatz orbit for `start`, capping the work at `max_steps`.
Sequence generate(value_type start, std::size_t max_steps = kDefaultMaxSteps);

/// Computes the metrics of the orbit for `start`, capping the work at `max_steps`.
Statistics analyse(value_type start, std::size_t max_steps = kDefaultMaxSteps);

/// Renders a status as a stable lowercase token for diagnostics and tests.
std::string to_string(Status status);

}  // namespace collatz
