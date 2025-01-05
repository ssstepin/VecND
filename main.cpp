#include <array>
#include <cassert>
#include <iostream>

#include "vecnd.h"

namespace ConstructorTests {

namespace {

void test_default_constructor() {
  constexpr size_t N = 3;
  const Personal::VecND<int, N, N, N> vec;
  for (size_t x = 0; x < N; ++x)
    for (size_t y = 0; y < N; ++y)
      for (size_t z = 0; z < N; ++z)
        assert(vec.at(x, y, z) == 0);

  constexpr auto expected_sizes = std::array<size_t, 3>{N, N, N};
  assert(vec.sizes() == expected_sizes);
}

void test_fill_constructor() {
  constexpr size_t N = 3;
  static constexpr std::string test_string = "test";
  const Personal::VecND<std::string, N, N, N> vec(test_string);
  for (size_t x = 0; x < N; ++x) {
    for (size_t y = 0; y < N; ++y) {
      for (size_t z = 0; z < N; ++z) {
        assert(vec.at(x, y, z) == "test");
      }
    }
  }
}

void test_fill_lambda_constructor() {
  constexpr size_t N = 3;
  constexpr auto f = [](size_t x, size_t y, size_t z) -> size_t {
    return x * y * z;
  };
  const Personal::VecND<int, N, N, N> vec(f);
  for (size_t x = 0; x < N; ++x) {
    for (size_t y = 0; y < N; ++y) {
      for (size_t z = 0; z < N; ++z) {
        assert(vec.at(x, y, z) == f(x, y, z));
      }
    }
  }
}
} // namespace

void Test() {
  test_default_constructor();
  test_fill_constructor();
  test_fill_lambda_constructor();
}

} // namespace ConstructorTests

namespace LookUpTests {
namespace {
void test_at() {
  constexpr size_t N = 3;
  constexpr auto f = [](size_t x, size_t y, size_t z) -> size_t {
    return x * y * z;
  };
  const Personal::VecND<int, N, N, N> vec(f);
  for (size_t x = 0; x < N; ++x) {
    for (size_t y = 0; y < N; ++y) {
      for (size_t z = 0; z < N; ++z) {
        assert(vec.at(x, y, z) == f(x, y, z));
      }
    }
  }
}

void test_get() {
  constexpr size_t N = 3;
  constexpr auto f = [](size_t x, size_t y, size_t z) -> size_t {
    return x * y * z;
  };
  Personal::VecND<int, N, N, N> vec;
  for (size_t x = 0; x < N; ++x) {
    for (size_t y = 0; y < N; ++y) {
      for (size_t z = 0; z < N; ++z) {
        vec.get(x, y, z) = f(x, y, z);
      }
    }
  }

  for (size_t x = 0; x < N; ++x) {
    for (size_t y = 0; y < N; ++y) {
      for (size_t z = 0; z < N; ++z) {
        assert(vec.at(x, y, z) == f(x, y, z));
      }
    }
  }
}
} // namespace

void Test() {
  test_at();
  test_get();
}
} // namespace LookUpTests

void test_all() {
  ConstructorTests::Test();
  LookUpTests::Test();
}

int main() {
  test_all();
  std::cout << "All assertions passed!\n";
}