#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <limits>
#include <vector>

#include "collatz/collatz.hpp"

TEST_CASE("a start value of 1 is a single-element orbit", "[core]") {
  const collatz::Sequence sequence = collatz::generate(1);
  REQUIRE(sequence.values == std::vector<collatz::value_type>{1});
  REQUIRE(sequence.status == collatz::Status::Ok);

  const collatz::Statistics stats = collatz::analyse(1);
  REQUIRE(stats.steps == 0);
  REQUIRE(stats.max_value == 1);
  REQUIRE(stats.stopping_time == 0);
}

TEST_CASE("the orbit of 6 matches the textbook sequence", "[core]") {
  const collatz::Sequence sequence = collatz::generate(6);
  REQUIRE(sequence.values ==
          std::vector<collatz::value_type>{6, 3, 10, 5, 16, 8, 4, 2, 1});
  REQUIRE(sequence.status == collatz::Status::Ok);

  const collatz::Statistics stats = collatz::analyse(6);
  REQUIRE(stats.steps == 8);
  REQUIRE(stats.max_value == 16);
  REQUIRE(stats.stopping_time == 1);
}

TEST_CASE("the orbit of 27 reports its known length and peak", "[core]") {
  const collatz::Sequence sequence = collatz::generate(27);
  REQUIRE(sequence.status == collatz::Status::Ok);
  REQUIRE(sequence.values.front() == 27);
  REQUIRE(sequence.values.back() == 1);

  const collatz::Statistics stats = collatz::analyse(27);
  REQUIRE(stats.steps == 111);
  REQUIRE(stats.max_value == 9232);
  REQUIRE(sequence.values.size() == stats.steps + 1);
}

TEST_CASE("an overflowing 3n+1 step is reported instead of wrapping", "[core][overflow]") {
  const collatz::value_type start = std::numeric_limits<collatz::value_type>::max();

  const collatz::Sequence sequence = collatz::generate(start);
  REQUIRE(sequence.status == collatz::Status::Overflow);
  REQUIRE(sequence.values == std::vector<collatz::value_type>{start});

  const collatz::Statistics stats = collatz::analyse(start);
  REQUIRE(stats.status == collatz::Status::Overflow);
  REQUIRE(stats.steps == 0);
}

TEST_CASE("the maximum step count truncates a long orbit", "[core]") {
  const collatz::Sequence sequence = collatz::generate(27, 5);
  REQUIRE(sequence.status == collatz::Status::Truncated);
  REQUIRE(sequence.values.size() == 6);
}

TEST_CASE("only positive integers are legal start values", "[core]") {
  REQUIRE(collatz::is_valid_start(1));
  REQUIRE(collatz::is_valid_start(27));
  REQUIRE_FALSE(collatz::is_valid_start(0));
}

TEST_CASE("status tokens render stably", "[core]") {
  REQUIRE(collatz::to_string(collatz::Status::Ok) == "ok");
  REQUIRE(collatz::to_string(collatz::Status::Overflow) == "overflow");
  REQUIRE(collatz::to_string(collatz::Status::Truncated) == "truncated");
  REQUIRE(collatz::to_string(collatz::Status::Invalid) == "invalid");
}
