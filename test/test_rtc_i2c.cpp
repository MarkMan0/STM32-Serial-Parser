#include "test_rtc_i2c.h"

#include "../include/DS3231/DS3231.h"
#include "../include/i2c.h"

#include "unity.h"

/**
 * @brief Test RTC and alongside it, I2C driver functionality
 *
 * Modifies time of RTC, but will restore at the end if nothing failed
 */
void test_rtc_i2c() {
  i2c.init_peripheral();


  DS3231::time t;
  // get the start time
  auto start_ms = HAL_GetTick();
  bool res = rtc.get_time(t);
  auto t_start = t;
  TEST_ASSERT_TRUE_MESSAGE(res, "Get time failed");

  TEST_ASSERT_NOT_EQUAL_MESSAGE(59, t.minutes,
                                "Test can't restore time correctly at 59minutes, wait a minute and run again");


  t.seconds = 16;
  t.minutes = 12;
  t.hours = 2;
  t.day = 1;
  t.date = 3;
  t.month = 4;
  t.year = 2010;
  res = rtc.set_time(t);
  TEST_ASSERT_TRUE_MESSAGE(res, "Set time failed");
  t = DS3231::time{ 0 };

  res = rtc.get_time(t);
  TEST_ASSERT_TRUE(res);
  TEST_ASSERT_LESS_OR_EQUAL_MESSAGE(2, 16 - t.seconds, "Seconds wrong");
  TEST_ASSERT_EQUAL_MESSAGE(12, t.minutes, "Minutes wrong");
  TEST_ASSERT_EQUAL_MESSAGE(2, t.hours, "Hours wrong");
  TEST_ASSERT_EQUAL_MESSAGE(1, t.day, "Day wrong");
  TEST_ASSERT_EQUAL_MESSAGE(3, t.date, "Date wrong");
  TEST_ASSERT_EQUAL_MESSAGE(4, t.month, "Month wrong");
  TEST_ASSERT_EQUAL_MESSAGE(2010, t.year, "Year wrong");

  auto elapsed_s = (HAL_GetTick() - start_ms) / 1000;
  // if no seconds elapsed, just set the start time
  if (elapsed_s != 0) {
    t_start.seconds += elapsed_s;
    if (t_start.seconds >= 60) {
      // we don't handle minutes "overflow", but it's fine, we made sure we are not at 59 minutes
      t_start.minutes += 1;
      t_start.seconds = 0;
    }
  }
  res = rtc.set_time(t_start);
  TEST_ASSERT_TRUE_MESSAGE(res, "Time restore failed");
}




#include "../src/DS3231/DS3231.cpp"
#include "../src/i2c.cpp"
