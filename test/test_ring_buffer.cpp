/**
 * @file test_ring_buffer.cpp
 * Ring Buffer test implementation
 *
 */

#include "test_ring_buffer.h"
#include "../include/ring_buffer.h"
#include "unity.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Run ring buffer tests
 *
 */
void test_ring_buffer() {
  constexpr size_t buff_sz = 5;
  RingBuffer<int, buff_sz> buff;
  buff.reset();

  constexpr int val = 5;

  TEST_ASSERT_TRUE_MESSAGE(buff.is_empty(), "Buffer is empty at start");
  TEST_ASSERT_FALSE_MESSAGE(buff.is_full(), "Buffer is not full at start");
  TEST_ASSERT_EQUAL_MESSAGE(0, buff.num_occupied(), "Occupied is 0 at start");

  // add one to buffer
  int* ptr = buff.get_next_free();
  TEST_ASSERT_TRUE_MESSAGE(ptr != nullptr, "get_next_free ptr in not null");

  *ptr = val;
  buff.push();
  TEST_ASSERT_FALSE_MESSAGE(buff.is_empty(), "Buffer not empty after push");
  TEST_ASSERT_FALSE_MESSAGE(buff.is_full(), "Buffer is not full after pushing 1");

  ptr = nullptr;
  ptr = buff.get_next_occupied();
  TEST_ASSERT_TRUE_MESSAGE(ptr != nullptr, "get_next_occupied ptr in not null");
  TEST_ASSERT_EQUAL_MESSAGE(val, *ptr, "Value got equals value pushed");

  buff.pop();
  TEST_ASSERT_TRUE_MESSAGE(buff.is_empty(), "Buffer is empty after popping one");


  for (size_t i = 0; i < buff_sz; ++i) {
    auto ptr = buff.get_next_free();
    TEST_ASSERT_NOT_EQUAL_MESSAGE(nullptr, ptr, "ptr not nullptr");
    *ptr = i;
    buff.push();
  }
  TEST_ASSERT_TRUE_MESSAGE(buff.is_full(), "Buffer is full");

  ptr = buff.get_next_free();
  TEST_ASSERT_EQUAL_MESSAGE(nullptr, ptr, "Next free is nullptr");

  for (size_t i = 0; i < buff_sz; ++i) {
    auto ptr = buff.get_next_occupied();
    TEST_ASSERT_NOT_EQUAL_MESSAGE(nullptr, ptr, "ptr not nullptr");
    TEST_ASSERT_EQUAL_MESSAGE(i, *ptr, "Read equals written");
    buff.pop();
  }

  TEST_ASSERT_TRUE_MESSAGE(buff.is_empty(), "Buffer is empty after popping");


  // concurrent get_next_free()
  buff.reset();
  auto ptr1 = buff.get_next_free();
  auto ptr2 = buff.get_next_free();
  TEST_ASSERT_FALSE(ptr1 == ptr2);

  *ptr1 = 100;
  *ptr2 = 200;

  buff.push();
  buff.push();
  auto val1 = *(buff.get_next_occupied());
  buff.pop();
  auto val2 = *(buff.get_next_occupied());
  buff.pop();

  TEST_ASSERT_EQUAL(100, val1);
  TEST_ASSERT_EQUAL(200, val2);
}


#ifdef __cplusplus
}
#endif
