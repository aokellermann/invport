/**
 * @file utils.h
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#pragma once

#include "invport/detail/common.h"

namespace inv::util
{
/**
 * Represents a date, without hour/minute/second etc.
 */
using Date = Timestamp;

/**
 * Gets the date of the given time.
 * @param time unix time in seconds
 * @return truncated date
 */
inline Date ToDate(const std::time_t& time)
{
  auto* const tm_now = std::gmtime(&time);
  tm_now->tm_sec = 0;
  tm_now->tm_min = 0;
  tm_now->tm_hour = 0;
  const std::time_t seconds_time = std::mktime(tm_now) - timezone;
  return std::chrono::duration_cast<Timestamp>(std::chrono::seconds(seconds_time));
}

/**
 * Gets today's date.
 * @return truncated date
 */
inline Date GetTodayDate()
{
  const auto time_now = std::time(nullptr);
  return ToDate(time_now);
}

/**
 * Gets the date from human-readable date format.
 * @param year YYYY
 * @param month MM [1,12]
 * @param day DD [1,31]
 * @return Date
 */
inline Date GetDate(int year, int month, int day)
{
  const std::time_t now = std::time(nullptr);
  std::tm* tm = std::gmtime(&now);
  tm->tm_year = year - 1900;
  tm->tm_mon = month - 1;
  tm->tm_mday = day;
  return ToDate(std::mktime(tm));
}

}  // namespace inv::util
