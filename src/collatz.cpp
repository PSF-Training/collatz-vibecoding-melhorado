#include "collatz/collatz.hpp"

#include <limits>

namespace collatz {
namespace {

/// Largest odd `n` for which `3n + 1` still fits in the 64-bit value type.
constexpr value_type kMaxSafeOdd = (std::numeric_limits<value_type>::max() - 1) / 3;

/// Applies the single Collatz transition to `n`, assuming `n` fits the value type.
value_type next_value(value_type n) noexcept {
  // Splitting on parity keeps the halving exact and skips the multiply when even.
  return (n % 2 == 0) ? n / 2 : 3 * n + 1;
}

/// Reports whether stepping an odd `n` would overflow the value type.
bool next_overflows(value_type n) noexcept {
  return (n % 2 != 0) && (n > kMaxSafeOdd);
}

}  // namespace

bool is_valid_start(value_type n) noexcept { return n >= 1; }

Sequence generate(value_type start, std::size_t max_steps) {
  Sequence result;
  result.start = start;

  if (!is_valid_start(start)) {
    // Zero has no orbit under the Collatz map, so it is reported rather than looped.
    result.status = Status::Invalid;
    return result;
  }

  result.values.push_back(start);
  value_type current = start;

  for (std::size_t step = 0; current != 1; ++step) {
    if (step >= max_steps) {
      result.status = Status::Truncated;
      return result;
    }
    if (next_overflows(current)) {
      // Stopping before the multiply guarantees no wrapped value is ever emitted.
      result.status = Status::Overflow;
      return result;
    }
    current = next_value(current);
    result.values.push_back(current);
  }

  return result;
}

Statistics analyse(value_type start, std::size_t max_steps) {
  Statistics stats;
  stats.max_value = start;

  if (!is_valid_start(start)) {
    stats.status = Status::Invalid;
    return stats;
  }

  value_type current = start;
  bool dropped = false;

  for (std::size_t step = 0; current != 1; ++step) {
    if (step >= max_steps) {
      stats.status = Status::Truncated;
      return stats;
    }
    if (next_overflows(current)) {
      stats.status = Status::Overflow;
      return stats;
    }
    current = next_value(current);
    ++stats.steps;
    if (current > stats.max_value) {
      stats.max_value = current;
    }
    // The stopping time is fixed at the first visit below the start, never updated.
    if (!dropped && current < start) {
      stats.stopping_time = stats.steps;
      dropped = true;
    }
  }

  return stats;
}

std::string to_string(Status status) {
  switch (status) {
    case Status::Ok:
      return "ok";
    case Status::Overflow:
      return "overflow";
    case Status::Truncated:
      return "truncated";
    case Status::Invalid:
      return "invalid";
  }
  return "unknown";
}

}  // namespace collatz
