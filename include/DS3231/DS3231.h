#ifndef DS_3231_H_
#define DS_3231_H_

/**
 * @file DS3231.h
 * @brief Class to communicate with DS3231 RTC over I2C
 *
 */

#include <cstdint>

/**
 * @brief DS3231 communication
 *
 */
class DS3231 {
public:
  /**
   * @brief To distinguish if AM/PM is used
   *
   */
  enum AM_PM : uint8_t {
    AM_PM_UNUSED,
    AM,
    PM,
  };
  /**
   * @brief Used by the class interface to get/set the time of the RTC
   *
   */
  struct time {
    uint8_t seconds, minutes, hours, day, date, month, am_pm;
    uint16_t year;
  };

  /**
   * @brief Reads the time from the RTC and stores it in the parameter
   *
   */
  [[nodiscard]] bool get_time(time&);
  /**
   * @brief Sets the RTC time from the parameter
   *
   */
  [[nodiscard]] bool set_time(time&);
  /**
   * @brief Configures the RTC
   *
   */
  void config();

  /**
   * @brief Prints the time in a user-readable way, static
   *
   * @param t time to be printed
   */
  static void report_time(const time& t);

  /**
   * @brief Reads all of the registers and prints it over uart
   *
   */
  void report_all_registers();

private:
  static constexpr uint8_t dev_address_{ 0b1101000 << 1 }; /*!< Shifted I2C address, ready to be used */
};


extern DS3231 rtc;

#endif
