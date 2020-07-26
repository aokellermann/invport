/**
 * @file utils.h
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#pragma once

#include "invport/detail/common.h"

namespace inv::util
{

Timestamp ToDate(const std::time_t& time)
{
  auto *const tm_now = std::gmtime(&time);
  tm_now->tm_sec = 0;
  tm_now->tm_min = 0;
  tm_now->tm_hour = 0;
  const std::time_t seconds_time = std::mktime(tm_now) - timezone;
  return std::chrono::duration_cast<Timestamp>(std::chrono::seconds(seconds_time));
}

Timestamp GetTodayDate()
{
  const auto time_now = std::time(nullptr);
  return ToDate(time_now);
}
}
