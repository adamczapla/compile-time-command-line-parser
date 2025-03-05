#include <ctclp.hpp>
#include <iostream>

constexpr auto test_string = [] consteval {
  static constexpr auto opts = ctclp::options<1>{}
    .add<"mode", ctclp::option::string, 
                 ctclp::values<"debug", "release">,
                 ctclp::default_value<"debug">>();
  static constexpr char const* argv[] = {"programm", "--mode=release"}; 
  static constexpr auto argc = std::size(argv);
  static constexpr auto parser_result = ctclp::parser<opts, 256>::try_parse<argc, argv>();
  static_assert(parser_result, "test_string: parsing failed."); 
  static constexpr auto result = parser_result.template get<"mode", std::string_view>();
  if constexpr (result.first.has_value()) {
    static_assert(*result.first == "release", "test_string: result does not match.");
    return true;
  }
  return false;
}();

constexpr auto test_string_default_value = [] consteval {
  static constexpr auto opts = ctclp::options<1>{}
    .add<"mode", ctclp::option::string, 
                 ctclp::values<"debug", "release">,
                 ctclp::default_value<"debug">>();
  static constexpr char const* argv[] = {"programm"}; 
  static constexpr auto argc = std::size(argv);
  static constexpr auto parser_result = ctclp::parser<opts, 256>::try_parse<argc, argv>();
  static_assert(parser_result, "test_string_default_value: parsing failed."); 
  static constexpr auto result = parser_result.template get<"mode", std::string_view>();
  if constexpr (result.first.has_value()) {
    static_assert(*result.first == "debug", "test_string_default_value: result does not match.");
    return true;
  }
  return false;
}();

constexpr auto test_integral = [] consteval {
  static constexpr auto opts = ctclp::options<1>{}
    .add<"age", ctclp::option::integral, 
                ctclp::values<"18", "22", "38">, 
                ctclp::default_value<"22">>();
  static constexpr char const* argv[] = {"programm", "--age=38"}; 
  static constexpr auto argc = std::size(argv);
  static constexpr auto parser_result = ctclp::parser<opts, 256>::try_parse<argc, argv>();
  static_assert(parser_result, "test_integral: parsing failed."); 
  static constexpr auto result = parser_result.template get<"age", int>();
  if constexpr (result.first.has_value()) {
    static_assert(*result.first == 38, "test_integral: result does not match.");
    return true;
  }
  return false;
}();

constexpr auto test_integral_default_value = [] consteval {
  static constexpr auto opts = ctclp::options<1>{}
    .add<"age", ctclp::option::integral, 
                ctclp::values<"18", "22", "38">, 
                ctclp::default_value<"22">>();
  static constexpr char const* argv[] = {"programm"}; 
  static constexpr auto argc = std::size(argv);
  static constexpr auto parser_result = ctclp::parser<opts, 256>::try_parse<argc, argv>();
  static_assert(parser_result, "test_integral_default_value: parsing failed."); 
  static constexpr auto result = parser_result.template get<"age", int>();
  if constexpr (result.first.has_value()) {
    static_assert(*result.first == 22, "test_integral_default_value: result does not match.");
    return true;
  }
  return false;
}();

constexpr auto test_floating_point = [] consteval {
  static constexpr auto opts = ctclp::options<1>{}
    .add<"amount", ctclp::option::floating_point, 
                   ctclp::values<"1000.99", "0.0001", "-0.0012345">,
                   ctclp::default_value<"-0.0012345">>();
  static constexpr char const* argv[] = {"programm", "--amount=0.0001"}; 
  static constexpr auto argc = std::size(argv);
  static constexpr auto parser_result = ctclp::parser<opts, 256>::try_parse<argc, argv>();
  static_assert(parser_result, "test_floating_point: parsing failed."); 
  static constexpr auto result = parser_result.template get<"amount", double>();
  if constexpr (result.first.has_value()) {
    static_assert(*result.first == 0.0001, "test_floating: result does not match.");
    return true;
  }
  return false;
}();

constexpr auto test_floating_point_default_value = [] consteval {
  static constexpr auto opts = ctclp::options<1>{}
    .add<"amount", ctclp::option::floating_point, 
                   ctclp::values<"1000.99", "0.0001", "-0.0012345">,
                   ctclp::default_value<"-0.0012345">>();
  static constexpr char const* argv[] = {"programm"}; 
  static constexpr auto argc = std::size(argv);
  static constexpr auto parser_result = ctclp::parser<opts, 256>::try_parse<argc, argv>();
  static_assert(parser_result, "test_floating_point_default_value: parsing failed."); 
  static constexpr auto result = parser_result.template get<"amount", double>();
  if constexpr (result.first.has_value()) {
    static_assert(*result.first == -0.0012345, "test_floating_point_default_value: result does not match.");
    return true;
  }
  return false;
}();

constexpr auto test_unknown_parameter = [] consteval {
  static constexpr auto opts = ctclp::options<1>{}
    .add<"para1", ctclp::option::integral, ctclp::values<"1", "2">>();
  static constexpr char const* argv[] = {"programm", "--para2=99"}; 
  static constexpr auto argc = std::size(argv);
  constexpr auto parser_result = ctclp::parser<opts, 256>::try_parse<argc, argv>();
  static_assert(!parser_result, "test_unknown_parameter: parser should fail on unknown param.");
  return true;
}();

constexpr auto test_invalid_integral = [] consteval {
  static constexpr auto opts = ctclp::options<1>{}
    .add<"id", ctclp::option::integral, ctclp::values<"100", "200">>();
  static constexpr char const* argv[] = {"programm", "--id=ABC"}; 
  static constexpr auto argc = std::size(argv);
  constexpr auto parser_result = ctclp::parser<opts, 256>::try_parse<argc, argv>();
  static_assert(!parser_result, "test_invalid_integral: parser should fail on invalid number.");
  return true;
}();

constexpr auto test_invalid_floating_point = [] consteval {
  static constexpr auto opts = ctclp::options<1>{}
    .add<"val", ctclp::option::floating_point, 
                ctclp::values<"3.14", "2.71">>();
  static constexpr char const* argv[] = {"programm", "--val=notafloat"}; 
  static constexpr auto argc = std::size(argv);
  constexpr auto parser_result = ctclp::parser<opts, 256>::try_parse<argc, argv>();
  static_assert(!parser_result, "test_invalid_floating_point: parser should fail on invalid float.");
  return true;
}();

constexpr auto test_multiple_options = [] consteval {
  static constexpr auto opts = ctclp::options<3>{}
    .add<"x", ctclp::option::integral, ctclp::values<"1", "2">>()
    .add<"y", ctclp::option::string, ctclp::values<"red", "blue">>()
    .add<"z", ctclp::option::floating_point, ctclp::values<"0.5", "1.5">>();
  static constexpr char const* argv[] = {"programm", "--x=2", "--y=blue", "--z=1.5"}; 
  static constexpr auto argc = std::size(argv);
  constexpr auto parser_result = ctclp::parser<opts, 256>::try_parse<argc, argv>();
  static_assert(parser_result, "test_multiple_options: parser should succeed.");
  constexpr auto res_x = parser_result.get<"x", int>();
  constexpr auto res_y = parser_result.get<"y", std::string_view>();
  constexpr auto res_z = parser_result.get<"z", double>();
  static_assert(res_x.first.has_value() && *res_x.first == 2, "x mismatch");
  static_assert(res_y.first.has_value() && *res_y.first == "blue", "y mismatch");
  static_assert(res_z.first.has_value() && *res_z.first == 1.5, "z mismatch");
  return true;
}();

constexpr auto test_empty_value = [] consteval {
  static constexpr auto opts = ctclp::options<1>{}
    .add<"para", ctclp::option::string, ctclp::values<"A", "B">>();
  static constexpr char const* argv[] = {"programm", "--para="}; 
  static constexpr auto argc = std::size(argv);
  constexpr auto parser_result = ctclp::parser<opts, 256>::try_parse<argc, argv>();
  static_assert(!parser_result, "test_empty_value: parser should fail on empty value.");
  return true;
}();

auto main() -> int {
  std::cout << "All compile-time tests passed!" << std::endl;
  return 0;
}
