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


} // ctclp

