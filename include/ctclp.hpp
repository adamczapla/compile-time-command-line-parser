#pragma once

#include <ctre.hpp>
#include <ranges>

namespace ctclp {

namespace rng = std::ranges;
using ctll::fixed_string;

template <size_t size>
struct literal_string {

  constexpr literal_string(char const (&str)[size]) noexcept {
    rng::copy_n(str, size, rng::begin(data)); 
  }

  constexpr auto to_string() const noexcept {
    return std::string{rng::begin(data), std::prev(rng::end(data))};
  }

  constexpr auto to_string_view() const noexcept {
    return std::string_view{data.data(), size};
  }

  constexpr auto to_view_remove_suffix() const noexcept {
    return std::string_view{data.data(), size-1};
  }

  std::array<char, size> data{};

};

struct option {

  enum type : size_t { integral, floating_point, string };
  
  friend auto operator<<(std::ostream& os, type const& opt_type) -> std::ostream& {
      constexpr std::array<std::string_view, 2> type_messages {"digit", "string"};
      auto const num = static_cast<int>(opt_type);
      return os << "Option type: " << "(" << type_messages[num] << ")";
  }
  
  constexpr option() = default;
  constexpr option(std::string_view name, type value, std::string_view defval, std::string_view regex)
      : name{name.data()}, value{value}, defval{defval.data()}, regex{regex.data()} 
  {}

  char const* name{};
  type value{};
  char const* defval{};
  char const* regex{};

};


} // ctclp

