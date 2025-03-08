# CTCLP – Compile-Time Command Line Parser

**CTCLP is a simple command-line parser that is fully configured at compile time.** The configuration defines all available options, including allowed values and default settings. To ensure correctness, the configuration should be validated through compile-time parsing tests. This allows errors—such as typos in rarely used values, invalid default settings, or missing expected options—to be detected before the program is even compiled.

At runtime, CTCLP functions like a regular command-line parser. The greatest benefit comes from using it at compile time to validate the configuration and at runtime for actual parsing.

## Type Safety & Compile-Time Regular Expressions

CTCLP automatically converts input values into the desired C++ data type:

```c++
int result = parse_result.template get<"para", int>();
```

Supported types:
* **Integral types** (e.g., `bool`, `short`, `int`, `long`) 
* **Floating-point types** (e.g., `float`, `double`) 
* **Strings** (`std::string_view`) 

CTCLP uses [CTRE (Compile-Time Regular Expressions)](https://github.com/hanickadot/compile-time-regular-expressions) to validate input values.

## Installation

CTCLP is a **header-only library** and can be integrated into a project using **CMake FetchContent**.

### Requirements

* **C++23:** CTCLP relies on modern language features, so your project must be compiled with C++23. 
* **Recent compiler:** It is recommended to use the latest version of GCC or Clang, as older versions may not fully support all C++23 features. MSVC compatibility has not been tested.
* **CMake 3.20 or later:** FetchContent is available starting from CMake 3.11, but version 3.20 ensures stability for the required features. 

### Integrating with FetchContent

Add the following lines to your `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.20)

include(FetchContent)

FetchContent_Declare(
  ctclp
  GIT_REPOSITORY https://github.com/adamczapla/compile-time-command-line-parser.git
  GIT_TAG main
)

FetchContent_MakeAvailable(ctclp)

add_executable(my_project main.cpp)
target_link_libraries(my_project PRIVATE ctclp)
target_compile_features(my_project PRIVATE cxx_std_23)
```

### Usage

Once included, the parser can be used with:

```c++
#include <ctclp.hpp>
```
## Example: Compile-Time and Runtime Parsing

### 1. Configuration

The available command-line options are defined at **compile time**, including allowed values and default settings:
```c++
static constexpr auto opts = ctclp::options<3>{}
    .add<"count", 
      ctclp::option::integral, 
      ctclp::values<"1", "10", "100">, 
      ctclp::default_value<"10">>()
    .add<"unit", 
      ctclp::option::string, 
      ctclp::values<"celsius", "fahrenheit", "kelvin">, 
      ctclp::default_value<"celsius">>()
    .add<"threshold", ctclp::option::floating_point, 
      ctclp::values<"0.1", "0.5", "1.0">, 
      ctclp::default_value<"0.5">>();
```
### 2. Compile-Time Validation

CTCLP allows validating the configuration at **compile time** to ensure that all allowed values are correctly defined and free of typos.

#### Valid Configuration

```c++
static constexpr auto opts_valid = ctclp::options<1>{}
    .add<"unit", ctclp::option::string, 
                 ctclp::values<"celsius", "fahrenheit", "kelvin">, 
                 ctclp::default_value<"celsius">>();

static constexpr char const* argv[] = {"program", "--unit=kelvin"}; 
static constexpr auto argc = std::size(argv);

static constexpr auto parser_result = 
  ctclp::parser<opts_valid, 256>::try_parse<argc, argv>();

static_assert(parser_result, "test_valid_value: parsing failed."); 

static constexpr auto result = parser_result.template get<"unit", std::string_view>();
static_assert(*result.first == "kelvin", "test_valid_value: value does not match.");
```

#### Misconfiguration with a Rarely Used Value

```c++
static constexpr auto opts_invalid = ctclp::options<1>{} 
  .add<"unit", 
    ctclp::option::string, 
    ctclp::values<"celsius", "fahrenheit", "kelivn">, // ❌ Typo in "kelvin"!
    ctclp::default_value<"celsius">>();

static constexpr char const* argv[] = {"program", "--unit=kelvin"};
static constexpr auto argc = std::size(argv);

static constexpr auto parser_result = 
  ctclp::parser<opts_invalid, 256>::try_parse<argc, argv>();

// ❌ Expected compile-time error
static_assert(parser_result, "test_typo_in_value: parsing failed."); 
```

> In this case, `kelvin` was mistakenly written as `kelivn` in the **configuration**.
Without **compile-time validation**, this mistake could go unnoticed until someone actually tries to use `kelvin` as an input.

### 3. Usage (Runtime Parsing)

After defining and validating the configuration, CTCLP can be used to **parse command-line arguments at runtime**.


```c++
static constexpr auto opts = ctclp::options<3>{}
  .add<"count", 
    ctclp::option::integral, 
    ctclp::values<"1", "10", "100">, 
    ctclp::default_value<"10">>()
  .add<"unit", 
    ctclp::option::string, 
    ctclp::values<"celsius", "fahrenheit", "kelvin">, 
    ctclp::default_value<"celsius">>()
  .add<"threshold", 
    ctclp::option::floating_point, 
    ctclp::values<"0.1", "0.5", "1.0">, 
    ctclp::default_value<"0.5">>();

auto main(int argc, char** argv) -> int {

    // Try to parse the command-line arguments
    auto parser_result = ctclp::parser<opts, 256>::try_parse(argv);

    if (!parser_result) {
        std::cerr << "Parsing error:\n" << parser_result.errors << '\n';
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
```

#### Example Calls and Expected Output:

✅ **Using default values (no arguments provided):**

```
./program
```

**Output:**

```
Count: 10
Unit: celsius
Threshold: 0.5
```

✅ **Providing all values:**

```
./program --count=100 --unit=kelvin --threshold=1.0
```

**Output:**

```
Count: 100
Unit: kelvin
Threshold: 1.0
```

⛔ **Invalid value (parser rejects it):**

```
./program --count=200
```

**Error message:**

```lua
Parsing error:
Errors:
  --count=200, Error code: 0 (parse_error)
```

Since `count=200` is not one of the allowed values (`1`, `10`, `100`), the program does not continue with invalid input.