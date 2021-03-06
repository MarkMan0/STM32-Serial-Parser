#include "DS3231/DS3231.h"

#include "main.h"
#include "i2c.h"
#include "utils.h"
#include "stdio.h"
#include "uart.h"
#include "cmsis_os.h"


/**
 * @brief DS3231 Hardware description
 *
 */
namespace DS3231Reg {
  /**
   * @brief DS3231 register addresses
   *
   */
  enum reg {
    SECONDS,
    MINUTES,
    HOURS,
    DAY,
    DATE,
    MONTH,
    YEAR,
    ALARM_1_SECONDS,
    ALARM_1_MINUTES,
    ALARM_1_HOURS,
    ALARM_1_DAY_DATE,
    ALARM_2_MINUTES,
    ALARM_2_HOURS,
    ALARM_2_DAY_DATE,
    CONTROL,
    CONTROL_STATUS,
    AGING_OFFSET,
    TEMP_MSB,
    TEMP_LSB,
    REGISTER_END,
  };

  /**
   * @brief Masks to hel manipulate registers
   *
   */
  enum mask {
    MASK_SECONDS = 0b1111,
    MASK_10_SECONDS = 0b1110000,
    MASK_MINUTES = 0b1111,
    MASK_10_MINUTES = 0b1110000,
    MASK_HOURS = 0b1111,
    MASK_10_HOUR = 0b10000,
    MASK_AM_PM_20_HOUR = 0b100000,
    MASK_12_24 = 0b1000000,
    MASK_DAY = 0b111,
    MASK_DATE = 0b1111,
    MASK_10_DATE = 0b110000,
    MASK_MONTH = 0b1111,
    MASK_10_MONTH = 0b10000,
    MASK_CENTURY = 0b10000000,
    MASK_YEAR = 0b1111,
    MASK_10_YEAR = 0b11110000,
    MASK_N_EOSC = 1 << 7,
    MASK_BBSQW = 1 << 6,
    MASK_CONV = 1 << 5,
    MASK_RS2 = 1 << 4,
    MASK_RS1 = 1 << 3,
    MASK_INTCN = 1 << 2,
    MASK_A2IE = 1 << 1,
    MASK_A1IE = 1 << 0,
    MASK_OSF = 1 << 7,
    MASK_EN32KHZ = 1 << 3,
    MASK_BSY = 1 << 2,
    MASK_A2F = 1 << 1,
    MASK_A1F = 1 << 0,
  };
}  // namespace DS3231Reg

using namespace DS3231Reg;

constexpr uint8_t DS3231::dev_address_;

/**
 * @brief Used as default value
 *
 */
static constexpr DS3231::time valid_time{ .seconds = 0,
                                          .minutes = 0,
                                          .hours = 0,
                                          .day = 1,
                                          .date = 1,
                                          .month = 1,
                                          .am_pm = DS3231::AM_PM_UNUSED,
                                          .year = 2000 };

void DS3231::report_all_registers() {
  uint8_t buff[REGISTER_END];
  {
    auto lck = i2c.get_lock();
    if (!lck.lock()) {
      return;
    }
    if (!i2c.read_register(dev_address_, SECONDS, buff, REGISTER_END)) {
      return;
    }
  }
  for (uint8_t i = 0; i < REGISTER_END; ++i) {
    uart2.printf("buff %d, val: %d", i, buff[i]);
  }
}


bool DS3231::get_time(time& t) {
  uint8_t buff[7];

  buff[0] = SECONDS;
  {
    auto lck = i2c.get_lock();
    if (!lck.lock()) {
      return false;
    }
    if (!i2c.read_register(dev_address_, SECONDS, buff, 7)) {
      return false;
    }
  }

  t = valid_time;

  t.seconds = (buff[0] & MASK_SECONDS) + 10 * ((buff[0] & MASK_10_SECONDS) >> 4);
  t.minutes = (buff[1] & MASK_MINUTES) + 10 * ((buff[1] & MASK_10_MINUTES) >> 4);
  if (buff[2] & MASK_12_24) {
    t.am_pm = (buff[2] & MASK_AM_PM_20_HOUR) ? AM_PM::PM : AM_PM::AM;
  } else {
    t.am_pm = AM_PM::AM_PM_UNUSED;
  }
  t.hours = (buff[2] & MASK_HOURS) + 10 * (!!(buff[2] & MASK_10_HOUR));
  if (t.am_pm == AM_PM::AM_PM_UNUSED) {
    t.hours += 20 * (!!(buff[2] & MASK_AM_PM_20_HOUR));
  }

  t.day = buff[3] & MASK_DAY;
  t.date = (buff[4] & MASK_DATE) + 10 * ((buff[4] & MASK_10_DATE) >> 4);
  t.month = (buff[5] & MASK_MONTH) + 10 * (!!(buff[5] & MASK_10_MONTH));
  t.year = 2000 + (buff[6] & MASK_YEAR) + 10 * ((buff[6] & MASK_10_YEAR) >> 4);


  return true;
}


bool DS3231::set_time(time& t) {
  uint8_t buff[7];

  using utils::is_within;

  if (!is_within(t.seconds, 0, 59)) return false;
  if (!is_within(t.minutes, 0, 59)) return false;
  if (!is_within(t.hours, 0, 23)) return false;
  if (!is_within(t.day, 1, 7)) return false;
  if (!is_within(t.date, 1, 31)) return false;
  if (!is_within(t.month, 1, 12)) return false;
  if (!is_within(t.am_pm, AM_PM_UNUSED, AM_PM::PM)) return false;
  if (!is_within(t.year, 2000, 2099)) return false;


  buff[0] = ((t.seconds / 10) << 4) | ((t.seconds % 10) & MASK_SECONDS);
  buff[1] = ((t.minutes / 10) << 4) | ((t.minutes % 10) & MASK_MINUTES);

  buff[2] = 0;
  if (t.am_pm == AM_PM::AM_PM_UNUSED) {
    if (t.hours >= 20) {
      buff[2] |= MASK_AM_PM_20_HOUR;
    }
  } else {
    buff[2] |= MASK_12_24;
    if (t.am_pm == AM_PM::PM) {
      buff[2] |= MASK_AM_PM_20_HOUR;
    }
  }
  if (t.hours >= 10 && t.hours < 20) {
    buff[2] |= MASK_10_HOUR;
  }
  buff[2] |= (t.hours % 10) & MASK_HOURS;

  buff[3] = t.day & MASK_DAY;


  buff[4] = ((t.date / 10) << 4) | ((t.date % 10) & MASK_DATE);


  buff[5] = (t.month % 10) & MASK_MONTH;
  if (t.month >= 10) {
    buff[5] |= MASK_10_MONTH;
  }

  buff[6] = ((t.year - 2000) / 10) << 4;
  buff[6] |= (t.year % 10) & MASK_YEAR;

  return i2c.get_lock().lock() && i2c.write_register(dev_address_, SECONDS, buff, 7);
}

void DS3231::report_time(const time& t) {
  uart2.printf("%2d.%2d.%4d %2d:%2d:%2d", t.date, t.month, t.year, t.hours, t.minutes, t.seconds);

  static constexpr const char* strs[] = {
    "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"
  };
  if (!utils::is_within(t.day, 1, 7)) {
    return;
  }
  uart2.printf("DOW: %s", strs[t.day - 1]);
}


DS3231 rtc;
