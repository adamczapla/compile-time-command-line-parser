#include <ctclp.hpp>
#include <iostream>

template <auto& parse_result>
constexpr void print_result() {
  static constexpr auto result = parse_result.template get<"para1", int>();
  // static constexpr auto result = parse_result.template get<"para2", std::string_view>();
  // static constexpr auto result = parse_result.template get<"para3", long double>();
  if constexpr (result.first.has_value()) {
    std::cout << *result.first << '\n';
  } else {
    std::cout << *result.second << '\n';
  }
}

auto main() -> int {

  static constexpr auto opts = ctclp::options<4>{}
    .add<"para1", ctclp::option::integral, 
                  ctclp::values<"11", "22", "33">, 
                  ctclp::default_value<"22">>()
    .add<"para2", ctclp::option::string, 
                  ctclp::values<"hello", "world", "!!!">>()
    .add<"para3", ctclp::option::floating_point, 
                  ctclp::values<"1000.99", "0.0001", "-0.0012345">,
                  ctclp::default_value<"-0.0012345">>()
    .add<"mode",  ctclp::option::string, 
                  ctclp::values<"debug", "release">,
                  ctclp::default_value<"debug">>();
  /*
    Compile-Time
  */ 

  static constexpr char const* argv[]{"programm"};
  // static constexpr char const* argv[] = {"programm", "--para1=33"}; 
  // static constexpr char const* argv[] = {"programm", "--para2=hello", "--para3=0.0001", "--para1=33"};
  // static constexpr char const* argv[] = {"programm", "--para1=33", "--para3=0.0001"};
  static constexpr auto argc = std::size(argv);
  static constexpr auto parser_result = ctclp::parser<opts, 256>::try_parse<argc, argv>();
  if constexpr (parser_result) {
    print_result<parser_result>();
  } else {
    std::cout << parser_result.errors << '\n';
  }

  /*
      Run-time
  */
  //char const* argv_2[] = {"programm", "--para2=hello", "--para1=33"};
  char const* argv_2[] = {"programm", "--para1=33", "--para2=hello"};
  auto const parser_result_2 = ctclp::parser<opts, 256>::try_parse(argv_2);

  if (parser_result_2) {
    auto result = parser_result_2.template get<"para3", long double>();
    // auto result = parser_result_2.template get<"para1", int>();
    if  (result.first.has_value()) {
      std::cout << *result.first << '\n';
    } else {
      std::cout << *result.second << '\n';
    }
  } else {
    std::cout << parser_result_2.errors << '\n';
  }

  return 0;
}