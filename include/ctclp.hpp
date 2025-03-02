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

constexpr auto pow_base10(long long exp) noexcept {
  long double result{1};
  for (auto i : std::views::iota(0, std::llabs(exp))) {result *= 10; }
  return exp < 0 ? 1/result : result;
}

constexpr auto to_double(long long int_part, long long frac_part, unsigned int frac_digits) {
  long double fraction = frac_part / pow_base10(frac_digits);
  return int_part + (int_part >= 0 ? fraction : -fraction);
}

template <options opts, size_t capacity>
class parser {
public:    
    
  template <size_t argc>
  class parser_result_t {
  public:

    template <options, size_t> friend class parser;
        
    constexpr operator bool() const noexcept { return !errors.right_size; }

    template <literal_string opt, typename opt_type>
    constexpr auto get() const noexcept -> std::pair<std::optional<opt_type>, 
                                                     std::optional<error_code>> {        
      auto const m_pos = rng::find_if(options_map, [](std::string str) { 
        return str == opt.to_string(); }, [](auto const& pair) { 
          return pair.first.name; });
          
      if (m_pos != rng::end(options_map)) {
        return convert_raw_value<opt_type>(m_pos->first, m_pos->second);
      }

      auto const options = rng::subrange{rng::begin(opts), rng::end(opts)};
      auto const o_pos = rng::find_if(options, [](std::string str) { 
        return str == opt.to_string(); }, &option::name);

      if (o_pos == rng::end(options)) { return {{}, error_code::option_not_found}; }
            
      auto const default_value = std::string_view{o_pos->defval};
      if (default_value.empty()) { return {{}, error_code::option_not_found}; }

      return convert_raw_value<opt_type>(*o_pos, default_value);            
    }

    parse_errors<opts.size> errors{};

  private:

    constexpr parser_result_t() = default;
        
    template <typename opt_type>
    constexpr auto convert_raw_value(option const& opt, std::string_view raw_opt_value) const noexcept
    -> std::pair<std::optional<opt_type>, std::optional<error_code>> {        
      if constexpr (std::integral<opt_type>) {
        if (opt.value != option::integral) { return {{}, error_code::incorrect_type}; }
        opt_type result{};
        auto const [_, ec] = std::from_chars(raw_opt_value.data(), 
                                             raw_opt_value.data() + raw_opt_value.size(), 
                                             result);
        if (ec != std::errc{}) { return {{}, error_code::conversion_error}; }
        return {result, {}};
      } else if constexpr (std::is_same_v<opt_type, std::string_view>) { 
        if (opt.value != option::string) { return {{}, error_code::incorrect_type}; }
        return {raw_opt_value, {}};
      }  else if constexpr (std::floating_point<opt_type>) { 
        if (opt.value != option::floating_point) { return {{}, error_code::incorrect_type}; }
        return raw_value_as_double(raw_opt_value);
      } else {
        return {{}, error_code::unknown_type};
      }
    }
    
    constexpr auto raw_value_as_double(std::string_view str) const noexcept 
    -> std::pair<std::optional<long double>, std::optional<error_code>> {        
      long long int_part{0};
      auto const end_pos = rng::data(str) + rng::size(str);
      auto const [ptr, ec] = std::from_chars(rng::data(str), end_pos, int_part);

      if (ec != std::errc{}) { return {{}, ctclp::error_code::conversion_error}; }
      if (ptr == end_pos) { return {{static_cast<long double>(int_part)}, {}}; } 
      if (*ptr != '.') { return {{}, ctclp::error_code::conversion_error}; }

      long long frac_part{0};
      auto const [ptr_frac, ec_frac] = std::from_chars(rng::next(ptr), end_pos, frac_part);
      if (ptr_frac != end_pos) { return {{}, ctclp::error_code::conversion_error}; }

      if (ec_frac != std::errc{} && std::distance(ptr+1, ptr_frac) == 0) { 
          return {{static_cast<long double>(int_part)}, {}};
      } 

      if (ec_frac != std::errc{}) { return {{}, ctclp::error_code::conversion_error}; }  

      auto is_neg_zero = false;
      if (auto const pos = str.find('.', 0); pos != std::string_view::npos) {
          if (str.substr(0, pos) == "-0") { is_neg_zero = true; }
      }

      auto frac_digit = std::distance(rng::next(ptr), ptr_frac);
      if (is_neg_zero) { frac_part = -frac_part; } 

      return {{to_double(int_part, frac_part, frac_digit)}, {}};    
    }
        
    std::array<std::pair<option, std::string_view>, argc> options_map{};

  };

  template <size_t argc, auto argv>
  static constexpr auto try_parse() noexcept {
    return parser{}.parse<argc>(argv, std::make_index_sequence<argc>{});
  }

  template <size_t argc>
  static constexpr auto try_parse(char const* (&argv)[argc]) noexcept {
    return parser{}.parse<argc>(argv, std::make_index_sequence<argc>{});
  }

private:
    
  constexpr parser() = default;

  template <size_t argc, size_t... idx>
  constexpr auto parse(auto const& argv, std::index_sequence<idx...>) const noexcept {
    parser_result_t<argc> result{};
    (parse_option<idx, argc>(argv, result), ...);
    return result;
  }

  template <size_t... indx>
  static constexpr auto get_parse_result(std::string_view input, std::index_sequence<indx...>) noexcept {
    std::optional<std::pair<size_t, std::string_view>> result{};

    std::tuple match_result_tuple{
      (input.contains(std::get<0>(std::get<indx>(regex_tuple)))
        ? ctre::match<std::get<2>(std::get<indx>(regex_tuple))>(input)
        : ctre::match<std::get<2>(std::get<indx>(regex_tuple))>(std::string_view{"..."}))
      ...  
    };

    (void) ((std::get<indx>(match_result_tuple) 
      ? (result = std::optional{std::pair{indx, std::string_view{std::get<indx>(match_result_tuple).
        template get<std::get<1>(std::get<indx>(regex_tuple))>()}}})
      : std::optional<std::pair<size_t, std::string_view>>{}), ...
    );
            
    return result;
  }

  template <size_t idx, size_t argc>
  constexpr auto parse_option(auto const& argv, parser_result_t<argc>& result) const noexcept {        
    auto parsed_result = get_parse_result(argv[idx], std::make_index_sequence<opts.size>{});
        
    if (!parsed_result.has_value()) {
      result.errors.append(parse_error_info{argv[idx], error_code::parse_error});
      return; 
    } 
        
    result.options_map[idx] = std::pair{
      opts.data[parsed_result->first], parsed_result->second
    };
  }

public:
    
  template <auto regex_array, size_t... idx>
  static constexpr auto make_regex_tuple(std::index_sequence<idx...>) noexcept {
    return std::tuple{
      std::tuple{
        std::string_view{std::string_view{opts.data[idx].name}},
        fixed_string{to_char_array<std::strlen(opts.data[idx].name)>(opts.data[idx].name)},  
        fixed_string{to_right_size<regex_array[idx].first, regex_array[idx].second>()}
      }...
    };
  }

  template <size_t... idx>
  static constexpr auto prepare_regex_array(std::index_sequence<idx...>) noexcept {
    std::array<std::pair<std::array<char, capacity>, size_t>, opts.size> result{};
    ((result[idx] = to_array<capacity, [] { 
      return std::string{opts.data[idx].regex}; }>()) , ...);
    return result;
  }

  static constexpr auto regex_tuple = [] { 
    constexpr auto regex_array = [] { 
      return prepare_regex_array(std::make_index_sequence<opts.size>{}); 
    }();
    return make_regex_tuple<regex_array>(std::make_index_sequence<opts.size>{});
  }();

};



} // ctclp

