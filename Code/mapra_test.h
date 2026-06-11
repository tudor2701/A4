// Copyright (c) 2022, The MaPra Authors.

#ifndef MAPRA_TEST_H_
#define MAPRA_TEST_H_

#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace mapra {

template <typename T>
std::ostream& operator<<(std::ostream& stream, const std::vector<T>& vektor) {
  stream << "(";
  for (const auto& v : vektor) {
    stream << v << ", ";
  }
  stream << ")";
  return stream;
}


class MapraTest {
 public:
  MapraTest(const std::string& case_name, std::ostream& out = std::cerr)
      : name_(case_name), out_(out) {}
  MapraTest(const MapraTest&) = delete;
  MapraTest& operator=(const MapraTest&) = delete;
  ~MapraTest() { Summary(); }

  void Assert(const std::string& test_name, bool condition) {
    ++num_tests_;
    if (condition) {
      out_ << "[   OK   ] Test '" << name_ << '.' << test_name << "' passed.\n";
    } else {
      out_ << "[ FAILED ] Test '" << name_ << '.' << test_name << "' failed.\n";
      ++failed_tests_;
    }
  }

  template <typename T>
  void AssertEq(const std::string& test_name, const T& expected_value,
                const T& computed_value) {
    const bool kPassed = expected_value == computed_value;
    this->Assert(test_name, kPassed);
    if (!kPassed)
      out_ << "[        ] expected '" << expected_value << "', got '"
           << computed_value << "'.\n";
  }

  template <typename T>
  void AssertEq(const std::string& test_name, const std::vector<T>& expected_value,
                const std::vector<T>& computed_value) {
    const bool kPassed = expected_value == computed_value;
    this->Assert(test_name, kPassed);
    if (!kPassed)
      out_ << "[        ] expected '" << expected_value << "',\n"
           << "[        ] got      '" << computed_value << "'.\n";
  }

  void Summary() {
    out_ << "[========] Case '" << name_ << "': " << failed_tests_ << " out of "
         << num_tests_ << " tests failed!\n";
    if (failed_tests_)
      out_ << "[ FAILED ] Go and fix some bugs!\n";
    else
      out_ << "[CONGRATS] Everything seems to be ok.\n";
  }

 private:
  int num_tests_ = 0;
  int failed_tests_ = 0;
  std::string name_;
  std::ostream& out_ = std::cerr;
};

}  // namespace mapra

#endif  // MAPRA_TEST_H_