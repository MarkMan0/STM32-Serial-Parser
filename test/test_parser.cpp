#include "test_parser.h"
#include "../include/parser.h"
#include "unity.h"

#ifdef __cplusplus
extern "C" {
#endif

void test_parser() {
  Parser parser;
  const char msg[] = " A132 S1 Q52 C";
  parser.set_string(msg);

  TEST_ASSERT_EQUAL('A', parser.get_prefix());
  TEST_ASSERT_EQUAL(132, parser.get_number());
  int16_t d{ 0 };
  bool ret = parser.get_parameter('S', d, -1);
  TEST_ASSERT_TRUE(ret);
  TEST_ASSERT_EQUAL(1, d);

  ret = parser.get_parameter('Q', d, -1);
  TEST_ASSERT_TRUE(ret);
  TEST_ASSERT_EQUAL(52, d);

  ret = parser.get_parameter('C', d, -1);
  TEST_ASSERT_TRUE(ret);
  TEST_ASSERT_EQUAL(-1, d);
  d = 2;
  ret = parser.get_parameter('W', d, -1);
  TEST_ASSERT_FALSE(ret);
  TEST_ASSERT_EQUAL(-1, d);
}



#ifdef __cplusplus
}
#endif

#include "../src/parser.cpp"
