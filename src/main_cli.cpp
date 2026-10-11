#include <charconv>
#include <cstddef>
#include <iostream>
#include <string>
#include <system_error>
#include <vector>

#include <CLI/CLI.hpp>

#include "collatz/collatz.hpp"

namespace {

/// Prints one orbit and its metrics in a stable, script-friendly layout.
void print_result(const collatz::Sequence& sequence, const collatz::Statistics& stats) {
  std::cout << sequence.start << ": steps=" << stats.steps
            << " max=" << stats.max_value
            << " stopping_time=" << stats.stopping_time;
  if (stats.status != collatz::Status::Ok) {
    std::cout << " status=" << collatz::to_string(stats.status);
  }
  std::cout << '\n';

  // Space separation keeps each start value on a single, greppable record.
  for (std::size_t i = 0; i < sequence.values.size(); ++i) {
    if (i != 0) {
      std::cout << ' ';
    }
    std::cout << sequence.values[i];
  }
  std::cout << '\n';
}

/// Parses `token` into a positive Collatz start value, rejecting anything else.
bool parse_start(const std::string& token, collatz::value_type& out) {
  collatz::value_type value = 0;
  const char* begin = token.data();
  const char* end = token.data() + token.size();
  const auto parsed = std::from_chars(begin, end, value);
  // A trailing character or a zero means the token is not a legal start value.
  if (parsed.ec != std::errc{} || parsed.ptr != end || value == 0) {
    return false;
  }
  out = value;
  return true;
}

/// Collects whitespace-separated tokens from standard input.
std::vector<std::string> read_stdin_tokens() {
  std::vector<std::string> tokens;
  std::string token;
  while (std::cin >> token) {
    tokens.push_back(token);
  }
  return tokens;
}

}  // namespace

int main(int argc, char** argv) {
  CLI::App app{"Present Collatz sequences for a series of positive integers."};
  std::vector<std::string> numbers;
  app.add_option("numbers", numbers,
                 "Starting values; omit them to read a series from standard input");

  CLI11_PARSE(app, argc, argv);

  if (numbers.empty()) {
    numbers = read_stdin_tokens();
  }

  if (numbers.empty()) {
    std::cerr << "error: no start values given; pass them as arguments or on standard input\n";
    return 1;
  }

  bool saw_invalid = false;
  for (const std::string& token : numbers) {
    collatz::value_type start = 0;
    if (!parse_start(token, start)) {
      std::cerr << "error: '" << token << "' is not a positive integer\n";
      saw_invalid = true;
      continue;
    }
    print_result(collatz::generate(start), collatz::analyse(start));
  }

  // Distinct codes let a caller tell "nothing to do" apart from "bad input".
  return saw_invalid ? 2 : 0;
}
