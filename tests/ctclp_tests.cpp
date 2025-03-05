#include <ctclp.hpp>
#include <iostream>

//   static constexpr auto opts = ctclp::options<4>{}
//     .add<"para1", ctclp::option::integral, 
//                   ctclp::values<"11", "22", "33">, 
//                   ctclp::default_value<"22">>()
//     .add<"para2", ctclp::option::string, 
//                   ctclp::values<"hello", "world", "!!!">>();
//     // .add<"para3", ctclp::option::floating_point, 
//     //               ctclp::values<"1000.99", "0.0001", "-0.0012345">,
//     //               ctclp::default_value<"-0.0012345">>()
//     // .add<"mode",  ctclp::option::string, 
//     //               ctclp::values<"debug", "release">,
//     //               ctclp::default_value<"debug">>();
//   // static constexpr char const* argv[]{"programm"};
//   static constexpr char const* argv[] = {"programm", "--para1=33"}; 
//   static constexpr auto argc = std::size(argv);
//   static constexpr auto parser_result = ctclp::parser<opts, 256>::try_parse<argc, argv>();
//   static_assert(parser_result, "");

// // static_assert(test_1, "");

auto main() -> int {
    static constexpr auto opts = ctclp::options<4>{}
      .add<"para1", ctclp::option::integral, 
                    ctclp::values<"11", "22", "33">, 
                    ctclp::default_value<"22">>();
      // .add<"para2", ctclp::option::string, 
      //               ctclp::values<"hello", "world", "!!!">>(); 
//   //     .add<"para3", ctclp::option::floating_point, 
//   //                   ctclp::values<"1000.99", "0.0001", "-0.0012345">,
//   //                   ctclp::default_value<"-0.0012345">>()
//   //     .add<"mode",  ctclp::option::string, 
//   //                   ctclp::values<"debug", "release">,
//   //                   ctclp::default_value<"debug">>();
//   //   // static constexpr char const* argv[]{"programm"};
    static constexpr char const* argv[] = {"programm", "--para1=33"}; 
    static constexpr auto argc = std::size(argv);
    static constexpr auto parser_result = ctclp::parser<opts, 256>::try_parse<argc, argv>();
//   //   // static_assert(parser_result, "");
//   // std::cout << "All compile-time tests passed!" << std::endl;
  return 0;
}
