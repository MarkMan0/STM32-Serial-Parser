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

  TEST_ASSERT_TRUE_MESSAGE(buff.isEmpty(), "Buffer is empty at start");
  TEST_ASSERT_FALSE_MESSAGE(buff.isFull(), "Buffer is not full at start");
  TEST_ASSERT_EQUAL_MESSAGE(0, buff.numOccupied(), "Occupied is 0 at start");

  // add one to buffer
  int* ptr = buff.getNextFree();
  TEST_ASSERT_TRUE_MESSAGE(ptr != nullptr, "get_next_free ptr in not null");

  *ptr = val;
  buff.push();
  TEST_ASSERT_FALSE_MESSAGE(buff.isEmpty(), "Buffer not empty after push");
  TEST_ASSERT_FALSE_MESSAGE(buff.isFull(), "Buffer is not full after pushing 1");

  ptr = nullptr;
  ptr = buff.getNextOccupied();
  TEST_ASSERT_TRUE_MESSAGE(ptr != nullptr, "get_next_occupied ptr in not null");
  TEST_ASSERT_EQUAL_MESSAGE(val, *ptr, "Value got equals value pushed");

  buff.pop();
  TEST_ASSERT_TRUE_MESSAGE(buff.isEmpty(), "Buffer is empty after popping one");


  for (size_t i = 0; i < buff_sz; ++i) {
    auto ptr = buff.getNextFree();
    TEST_ASSERT_NOT_EQUAL_MESSAGE(nullptr, ptr, "ptr not nullptr");
    *ptr = i;
    buff.push();
  }
  TEST_ASSERT_TRUE_MESSAGE(buff.isFull(), "Buffer is full");

  ptr = buff.getNextFree();
  TEST_ASSERT_EQUAL_MESSAGE(nullptr, ptr, "Next free is nullptr");

  for (size_t i = 0; i < buff_sz; ++i) {
    auto ptr = buff.getNextOccupied();
    TEST_ASSERT_NOT_EQUAL_MESSAGE(nullptr, ptr, "ptr not nullptr");
    TEST_ASSERT_EQUAL_MESSAGE(i, *ptr, "Read equals written");
    buff.pop();
  }

  TEST_ASSERT_TRUE_MESSAGE(buff.isEmpty(), "Buffer is empty after popping");
}


#ifdef __cplusplus
}
#endif
