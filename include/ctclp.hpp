#pragma once

#include <ctre.hpp>
#include <ranges>
#include <string>
#include <string_view>
#include <array>
#include <algorithm>
#include <ostream>
#include <tuple>

namespace ctclp {

namespace rng = std::ranges;
using ctll::fixed_string;

template <auto value> consteval auto& to_static() { return value; }

template <auto max_size, auto string_builder> consteval auto to_string_view() {
  constexpr auto intermediate_data = [] {   
    std::array<char, max_size> max_size_array{};
    auto const end_pos = rng::copy(string_builder(), rng::begin(max_size_array));
    auto const right_size = rng::distance(rng::cbegin(max_size_array), end_pos.out);
    return std::pair{max_size_array, right_size};
  }();

  constexpr auto right_size_array = [&] {
    std::array<char, intermediate_data.second + 1> right_size_array{};
    rng::copy_n(rng::cbegin(intermediate_data.first), intermediate_data.second,
                rng::begin(right_size_array));
    right_size_array[intermediate_data.second] = '\0';
    return right_size_array; 
  }();

  return std::string_view{to_static<right_size_array>()}; 
}

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

template <literal_string... opt_values> struct values {

  using values_t = void;
  
  constexpr auto as_tuple() const noexcept {
    return std::tie(opt_values...);
  }

};

template <typename opt_values_t> 
concept option_values = requires { 

  typename opt_values_t::values_t; 

};

template <literal_string value> struct default_value {

  using default_value_t = void;

  constexpr auto to_string() const noexcept {
    return value.to_string();
  }

  constexpr auto to_string_view() const noexcept {
    return value.to_string_view();
  }

};

template <typename opt_default_value_t> 
concept option_default_value = requires { 

  typename opt_default_value_t::default_value_t; 

};

template <auto capacity>
struct options {

  constexpr options() = default;

  template <literal_string opt_name, option::type opt_type, option_values opt_values, 
            option_default_value opt_default_value = default_value<"">> 
  consteval auto& add() noexcept {
    static constexpr auto default_value = opt_default_value{};
    static constexpr auto values = opt_values{}.as_tuple();
      
    static_assert([&] {
      return std::apply([](auto const&... values) {
        if (default_value.to_string().empty()) { return true; }
        bool is_valid{false};
        ((is_valid += (values.to_string() == default_value.to_string())), ...);
        return is_valid;
      }, values);
    }(), "Default value does not match any of the provided values.");
      
    constexpr auto opt_regex = to_string_view<128, [] {
      return std::apply([](auto const&... values) {
        size_t cnt{sizeof...(values)};
        auto result = std::string{"\\s*(--"} + opt_name.to_string() + "=(?<" 
                                             + opt_name.to_string() + ">(";
        ((result += values.to_string() + (--cnt ? "|" : "")) , ...);
        return result.append("))){0,1}");
      }, values);
    }>();
      
    data[size++] = option{opt_name.to_string_view(), opt_type, 
                          default_value.to_string_view(), opt_regex};
    return *this;
  }

  constexpr auto begin() const noexcept { return data.begin(); }
  constexpr auto end() const noexcept { return std::next(begin(), size); }

  size_t size{0};
  std::array<option, capacity> data{};

};

enum class error_code { parse_error, option_not_found, incorrect_type, conversion_error, unknown_type };

auto operator<<(std::ostream& os, error_code const& ec) -> std::ostream& {
  constexpr std::array<std::string_view, 5> error_messages {"parse_error", "option_not_found", 
                                                            "incorrect_type", "conversion_error", 
                                                            "unknown_type"};
  auto const num = static_cast<int>(ec);
  return os << "Error code: " << num << " (" << error_messages[num] << ")";
}

using parse_error_info = std::tuple<std::string_view, error_code>;

template <size_t max_size>
struct parse_errors {    

  constexpr auto begin() const noexcept { return data.begin(); }
  constexpr auto end() const noexcept { return std::next(begin(), right_size); }

  constexpr auto append(parse_error_info const& error_info) noexcept -> parse_errors& {
    data[right_size++] = error_info;
    return *this;
  }

  std::array<parse_error_info, max_size> data{};
  size_t right_size{0};

  friend auto operator<<(std::ostream& os, parse_errors const& errors) noexcept -> std::ostream& {
    os << "Errors:\n";
    for (auto const& error_info : errors) {
      os << "  " << std::get<0>(error_info) 
         << ", " << std::get<1>(error_info)
         << '\n';
    }
    return os;
  }
    
};



} // ctclp

