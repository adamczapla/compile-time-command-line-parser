#include <ctclp.hpp>
#include <iostream>

consteval auto test_string() noexcept {
  static constexpr auto opts = ctclp::options<1>{}
    .add<"mode", ctclp::option::string, 
                 ctclp::values<"debug", "release">,
                 ctclp::default_value<"debug">>();
  static constexpr char const* argv[] = {"programm", "--mode=release"}; 
  static constexpr auto argc = std::size(argv);
  static constexpr auto parser_result = ctclp::parser<opts, 256>::try_parse<argc, argv>();
  static_assert(parser_result, "test_string: parsing failed."); 
  static constexpr auto result = parser_result.template get<"mode", std::string_view>();
  static_assert(*result.first == "release", "test_string: result does not match.");
}

consteval auto test_string_default_value() noexcept {
  static constexpr auto opts = ctclp::options<1>{}
    .add<"mode", ctclp::option::string, 
                 ctclp::values<"debug", "release">,
                 ctclp::default_value<"debug">>();
  static constexpr char const* argv[] = {"programm"}; 
  static constexpr auto argc = std::size(argv);
  static constexpr auto parser_result = ctclp::parser<opts, 256>::try_parse<argc, argv>();
  static_assert(parser_result, "test_string_default_value: parsing failed."); 
  static constexpr auto result = parser_result.template get<"mode", std::string_view>();
  static_assert(*result.first == "debug", "test_string_default_value: result does not match.");
}

consteval auto test_integral() noexcept {
  static constexpr auto opts = ctclp::options<1>{}
    .add<"age", ctclp::option::integral, 
                ctclp::values<"18", "22", "38">, 
                ctclp::default_value<"22">>();
  static constexpr char const* argv[] = {"programm", "--age=38"}; 
  static constexpr auto argc = std::size(argv);
  static constexpr auto parser_result = ctclp::parser<opts, 256>::try_parse<argc, argv>();
  static_assert(parser_result, "test_integral: parsing failed."); 
  static constexpr auto result = parser_result.template get<"age", int>();
  static_assert(*result.first == 38, "test_integral: result does not match.");
}

consteval auto test_integral_default_value() noexcept {
  static constexpr auto opts = ctclp::options<1>{}
    .add<"age", ctclp::option::integral, 
                ctclp::values<"18", "22", "38">, 
                ctclp::default_value<"22">>();
  static constexpr char const* argv[] = {"programm"}; 
  static constexpr auto argc = std::size(argv);
  static constexpr auto parser_result = ctclp::parser<opts, 256>::try_parse<argc, argv>();
  static_assert(parser_result, "test_integral_default_value: parsing failed."); 
  static constexpr auto result = parser_result.template get<"age", int>();
  static_assert(*result.first == 22, "test_integral_default_value: result does not match.");
}

consteval auto test_floating_point() noexcept {
  static constexpr auto opts = ctclp::options<1>{}
    .add<"amount", ctclp::option::floating_point, 
                   ctclp::values<"1000.99", "0.0001", "-0.0012345">,
                   ctclp::default_value<"-0.0012345">>();
  static constexpr char const* argv[] = {"programm", "--amount=0.0001"}; 
  static constexpr auto argc = std::size(argv);
  static constexpr auto parser_result = ctclp::parser<opts, 256>::try_parse<argc, argv>();
  static_assert(parser_result, "test_floating_point: parsing failed."); 
  static constexpr auto result = parser_result.template get<"amount", double>();
  static_assert(*result.first == 0.0001, "test_floating: result does not match.");
}

consteval auto test_floating_point_default_value() {
  static constexpr auto opts = ctclp::options<1>{}
    .add<"amount", ctclp::option::floating_point, 
                   ctclp::values<"1000.99", "0.0001", "-0.0012345">,
                   ctclp::default_value<"-0.0012345">>();
  static constexpr char const* argv[] = {"programm"}; 
  static constexpr auto argc = std::size(argv);
  static constexpr auto parser_result = ctclp::parser<opts, 256>::try_parse<argc, argv>();
  static_assert(parser_result, "test_floating_point_default_value: parsing failed."); 
  static constexpr auto result = parser_result.template get<"amount", double>();
  static_assert(*result.first == -0.0012345, "test_floating_point_default_value: result does not match.");
}

consteval auto test_unknown_parameter() {
  static constexpr auto opts = ctclp::options<1>{}
    .add<"para1", ctclp::option::integral, ctclp::values<"1", "2">>();
  static constexpr char const* argv[] = {"programm", "--para2=99"}; 
  static constexpr auto argc = std::size(argv);
  constexpr auto parser_result = ctclp::parser<opts, 256>::try_parse<argc, argv>();
  static_assert(!parser_result, "test_unknown_parameter: parser should fail on unknown param.");
}

consteval auto test_invalid_integral() noexcept {
  static constexpr auto opts = ctclp::options<1>{}
    .add<"id", ctclp::option::integral, ctclp::values<"100", "200">>();
  static constexpr char const* argv[] = {"programm", "--id=ABC"}; 
  static constexpr auto argc = std::size(argv);
  constexpr auto parser_result = ctclp::parser<opts, 256>::try_parse<argc, argv>();
  static_assert(!parser_result, "test_invalid_integral: parser should fail on invalid number.");
}

consteval auto test_invalid_floating_point() {
  static constexpr auto opts = ctclp::options<1>{}
    .add<"val", ctclp::option::floating_point, 
                ctclp::values<"3.14", "2.71">>();
  static constexpr char const* argv[] = {"programm", "--val=notafloat"}; 
  static constexpr auto argc = std::size(argv);
  constexpr auto parser_result = ctclp::parser<opts, 256>::try_parse<argc, argv>();
  static_assert(!parser_result, "test_invalid_floating_point: parser should fail on invalid float.");
}

consteval auto test_multiple_options() noexcept {
  static constexpr auto opts = ctclp::options<7>{}
    .add<"x", ctclp::option::integral, ctclp::values<"1", "2">>()
    .add<"y", ctclp::option::string, ctclp::values<"red", "blue">>()
    .add<"z", ctclp::option::floating_point, ctclp::values<"0.5", "1.5">>()
    .add<"flag", ctclp::option::integral, ctclp::values<"1", "0">>()  // bool
    .add<"small", ctclp::option::integral, ctclp::values<"10", "20">>()  // short
    .add<"big", ctclp::option::integral, ctclp::values<"100000", "200000">>() // long
    .add<"float_val", ctclp::option::floating_point, ctclp::values<"3.14", "2.71">>(); // float

  static constexpr char const* argv[] = {
      "programm", "--x=2", "--y=blue", "--z=1.5", "--flag=1", "--small=20", "--big=200000", "--float_val=3.14"
  };
  static constexpr auto argc = std::size(argv);
  constexpr auto parser_result = ctclp::parser<opts, 256>::try_parse<argc, argv>();
  static_assert(parser_result, "test_multiple_options: parser should succeed.");

  constexpr auto res_x = parser_result.get<"x", int>();
  constexpr auto res_y = parser_result.get<"y", std::string_view>();
  constexpr auto res_z = parser_result.get<"z", double>();
  constexpr auto res_flag = parser_result.get<"flag", bool>();
  constexpr auto res_small = parser_result.get<"small", short>();
  constexpr auto res_big = parser_result.get<"big", long>();
  constexpr auto res_float = parser_result.get<"float_val", float>();

  static_assert(res_x.first.has_value() && *res_x.first == 2, "x mismatch");
  static_assert(res_y.first.has_value() && *res_y.first == "blue", "y mismatch");
  static_assert(res_z.first.has_value() && *res_z.first == 1.5, "z mismatch");
  static_assert(res_flag.first.has_value() && *res_flag.first == true, "flag mismatch");
  static_assert(res_small.first.has_value() && *res_small.first == 20, "small mismatch");
  static_assert(res_big.first.has_value() && *res_big.first == 200000, "big mismatch");
  static_assert(res_float.first.has_value() && *res_float.first == 3.14f, "float mismatch");
}

consteval auto test_empty_value() noexcept {
  static constexpr auto opts = ctclp::options<1>{}
    .add<"para", ctclp::option::string, ctclp::values<"A", "B">>();
  static constexpr char const* argv[] = {"programm", "--para="}; 
  static constexpr auto argc = std::size(argv);
  constexpr auto parser_result = ctclp::parser<opts, 256>::try_parse<argc, argv>();
  static_assert(!parser_result, "test_empty_value: parser should fail on empty value.");
}

static constexpr auto opts = ctclp::options<3>{}
  .add<"count", ctclp::option::integral, ctclp::values<"1", "10", "100">, ctclp::default_value<"10">>()
  .add<"unit", ctclp::option::string, ctclp::values<"celsius", "fahrenheit", "kelvin">, ctclp::default_value<"celsius">>()
  .add<"threshold", ctclp::option::floating_point, ctclp::values<"0.1", "0.5", "1.0">, ctclp::default_value<"0.5">>();

auto main() -> int {

    // char const* argv[]{"programm"};
    char const* argv[]{"programm", "--count=100", "--unit=kelvin", "--threshold=1.0"};
    // char const* argv[]{"programm", "--count=200"};
    auto parser_result = ctclp::parser<opts, 256>::try_parse(argv);

    if (!parser_result) {
        std::cerr << "Parsing error:\n" << parser_result.errors << std::endl;
        return 1;
    }

    // Retrieve values and print them
    auto count = parser_result.get<"count", int>();
    auto unit = parser_result.get<"unit", std::string_view>();
    auto threshold = parser_result.get<"threshold", double>();

    std::cout << "Count: " << *count.first << "\n";
    std::cout << "Unit: " << *unit.first << "\n";
    std::cout << "Threshold: " << *threshold.first << "\n";
 
  return 0;
}
