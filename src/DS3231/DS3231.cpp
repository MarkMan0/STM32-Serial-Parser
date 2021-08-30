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
  utils::hal_wrap(HAL_I2C_Master_Receive(&i2c.hi2c1_, dev_address_, buff, REGISTER_END, 1000));
  constexpr size_t msg_len = 25;
  char msg[msg_len];
  for (uint8_t i = 0; i < REGISTER_END; ++i) {
    snprintf(msg, msg_len - 1, "buff %d, val: %d", i, buff[i]);
    uart2.send_queue(msg);
  }
}


bool DS3231::get_time(time& t) {
  uint8_t buff[7];

  buff[0] = SECONDS;

  HAL_StatusTypeDef res{ HAL_ERROR };

  res = HAL_I2C_Master_Transmit(&i2c.hi2c1_, dev_address_, buff, 1, 100);
  if (res != HAL_OK) {
    return false;
  }

  res = HAL_I2C_Master_Receive(&i2c.hi2c1_, dev_address_, buff, 7, 100);
  if (res != HAL_OK) {
    return false;
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
  uint8_t buff[8];

  buff[0] = SECONDS;

  using utils::is_within;

  if (!is_within(t.seconds, 0, 59)) t.seconds = valid_time.seconds;
  if (!is_within(t.minutes, 0, 59)) t.minutes = valid_time.minutes;
  if (!is_within(t.hours, 0, 23)) t.hours = valid_time.hours;
  if (!is_within(t.day, 1, 7)) t.day = valid_time.day;
  if (!is_within(t.date, 1, 31)) t.date = valid_time.date;
  if (!is_within(t.month, 1, 12)) t.month = valid_time.month;
  if (!is_within(t.am_pm, AM_PM_UNUSED, AM_PM::PM)) t.am_pm = valid_time.am_pm;
  if (!is_within(t.year, 2000, 2099)) t.year = valid_time.year;


  buff[1] = ((t.seconds / 10) << 4) | ((t.seconds % 10) & MASK_SECONDS);
  buff[2] = ((t.minutes / 10) << 4) | ((t.minutes % 10) & MASK_MINUTES);

  buff[3] = 0;
  if (t.am_pm == AM_PM::AM_PM_UNUSED) {
    if (t.hours >= 20) {
      buff[3] |= MASK_AM_PM_20_HOUR;
    }
  } else {
    buff[3] |= MASK_12_24;
    if (t.am_pm == AM_PM::PM) {
      buff[3] |= MASK_AM_PM_20_HOUR;
    }
  }
  if (t.hours >= 10 && t.hours < 20) {
    buff[3] |= MASK_10_HOUR;
  }
  buff[3] |= (t.hours % 10) & MASK_HOURS;

  buff[4] = t.day & MASK_DAY;


  buff[5] = ((t.date / 10) << 4) | ((t.date % 10) & MASK_DATE);


  buff[6] = (t.month % 10) & MASK_MONTH;
  if (t.month >= 10) {
    buff[6] |= MASK_10_MONTH;
  }

  buff[7] = ((t.year - 2000) / 10) << 4;
  buff[7] |= (t.year % 10) & MASK_YEAR;

  if (HAL_I2C_Master_Transmit(&i2c.hi2c1_, dev_address_, buff, 8, 100) != HAL_OK) {
    return false;
  }

  return true;
}

void DS3231::report_time(const time& t) const {
  constexpr size_t buff_sz{ 30 };
  char buff[buff_sz];

  auto len =
      snprintf(buff, buff_sz - 1, "%2d.%2d.%4d %2d:%2d:%2d", t.date, t.month, t.year, t.hours, t.minutes, t.seconds);
  uart2.send_queue(buff, len);

  static constexpr const char* strs[] = {
    "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"
  };
  if (!utils::is_within(t.day, 1, 7)) {
    return;
  }
  len = snprintf(buff, buff_sz - 1, "DOW: %s", strs[t.day - 1]);
  uart2.send_queue(buff, len);
}


DS3231 rtc;
