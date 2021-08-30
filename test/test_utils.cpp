#include "test_utils.h"
#include "unity.h"
#include "../include/utils.h"


void test_min_max() {

  constexpr int i1{1}, i2{2}, i3{3};
  constexpr float f0{0}, f1{1}, f2{2}, f10{10};

  using namespace utils;

  constexpr auto min1 = min(i1, i2), min2 = min(i3, i2, i1);
  TEST_ASSERT_EQUAL(i1, min1);
  TEST_ASSERT_EQUAL(i1, min2);

  constexpr auto min3 = min(f1, f10, f0);
  TEST_ASSERT_EQUAL_FLOAT(f0, min3);

  constexpr auto max1 = max(i1, i2), max2 = max(i1, i3, i2);
  TEST_ASSERT_EQUAL(i2, max1);
  TEST_ASSERT_EQUAL(i3, max2);

  constexpr auto max3 = max(f0, f2, f10, f0);
  TEST_ASSERT_EQUAL_FLOAT(f10, max3);


}
