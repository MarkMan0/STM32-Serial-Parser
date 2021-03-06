#include "test_utils.h"
#include "unity.h"
#include "../include/utils.h"


void test_min_max() {
  constexpr int i1{ 1 }, i2{ 2 }, i3{ 3 };
  constexpr float f0{ 0 }, f1{ 1 }, f2{ 2 }, f10{ 10 };

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


void test_is_within() {
  using utils::is_within;

  constexpr int low = 1, high = 5;

  TEST_ASSERT_TRUE(is_within(low, low, high));
  TEST_ASSERT_TRUE(is_within(high, low, high));
  TEST_ASSERT_FALSE(is_within(low - 1, low, high));
  TEST_ASSERT_FALSE(is_within(high + 1, low, high));
  TEST_ASSERT_TRUE(is_within((high + low) * 0.5f, low, high));

  using utils::is_within_strict;

  TEST_ASSERT_FALSE(is_within_strict(low, low, high));
  TEST_ASSERT_FALSE(is_within_strict(high, low, high));
  TEST_ASSERT_FALSE(is_within_strict(low - 1, low, high));
  TEST_ASSERT_FALSE(is_within_strict(high + 1, low, high));
  TEST_ASSERT_TRUE(is_within_strict((high + low) * 0.5f, low, high));
}


void test_point() {
  using utils::Point;

  Point<uint8_t> p1{ 2, 2 };
  Point<int16_t> p2{ 3, 3 };
  Point<double> p3{ 2.0, 3 };

  TEST_ASSERT_EQUAL_FLOAT(p1.distance(p2), p2.distance(p1));
  TEST_ASSERT_EQUAL_FLOAT(p1.distance(p3), p3.distance(p1));
  TEST_ASSERT_EQUAL_FLOAT(1.0, p1.distance(p3));
}

void test_constrain() {
  using utils::constrain;

  TEST_ASSERT_EQUAL(1, constrain(1, 0, 2));
  TEST_ASSERT_EQUAL(2, constrain(5, 0, 2));
  TEST_ASSERT_EQUAL(0, constrain(-1, 0, 2));
  TEST_ASSERT_EQUAL_FLOAT(1.0, constrain(1.0, 0, 5));
}
