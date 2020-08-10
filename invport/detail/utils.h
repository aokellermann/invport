/**
 * @file utils.h
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#pragma once

#include <limits>

#include "invport/detail/common.h"

namespace inv
{
template <typename T1, typename T2>
bool FloatingEqual(T1 a, T2 b)
{
  const auto difference = std::fabs(a - b);
  return difference < std::numeric_limits<decltype(difference)>::epsilon();
}

// region Date

/**
 * Represents a date, without hour/minute/second etc.
 */
struct Date
{
  using PrimitiveType = uint16_t;

  static const Date kZero;
  static constexpr const uint8_t kDayBitLength = 5;
  static constexpr const uint8_t kMonthBitLength = 4;
  static constexpr const uint8_t kYearBitLength = 7;
  static constexpr const uint16_t kYearOffset = 1970;

  constexpr Date() noexcept : day(0), month(0), year(0) {}

  explicit Date(const json::Json& json) : Date(json.get<PrimitiveType>()) {}

  explicit Date(const std::string& str);

  explicit Date(const PrimitiveType pt) { std::memcpy(this, &pt, sizeof(PrimitiveType)); }

  Date(unsigned d, unsigned m, unsigned y) : Date(std::to_string(d) + '/' + std::to_string(m) + '/' + std::to_string(y))
  {
  }

  [[nodiscard]] PrimitiveType ToPrimitive() const
  {
    if (IsZero()) return 0;
    PrimitiveType pt;
    memcpy(&pt, this, sizeof(PrimitiveType));
    return pt;
  }

  [[nodiscard]] std::string ToString() const
  {
    if (IsZero()) throw std::runtime_error("Date is Zero");
    return std::to_string(day) + '/' + std::to_string(month) + '/' +
           std::to_string(kYearOffset + static_cast<int>(year));
  }

  [[nodiscard]] bool IsZero() const { return day == 0U || month == 0U; }

  static const Date& Zero() { return kZero; }

  static Date Today()
  {
    const auto time_now = std::time(nullptr);
    const auto* const tm_now = std::gmtime(&time_now);
    return Date(tm_now->tm_mday, tm_now->tm_mon + 1, tm_now->tm_year + 1900);
  }

  bool operator==(const Date& other) const { return ToPrimitive() == other.ToPrimitive(); }
  bool operator!=(const Date& other) const { return !(*this == other); }
  bool operator<(const Date& other) const { return ToPrimitive() < other.ToPrimitive(); }
  bool operator>(const Date& other) const { return ToPrimitive() > other.ToPrimitive(); }
  bool operator<=(const Date& other) const { return !(*this > other); }
  bool operator>=(const Date& other) const { return !(*this < other); }

  /**
   * Day of month: [1,31]
   */
  unsigned day : kDayBitLength;
  /**
   * Month of year: [1,12]
   */
  unsigned month : kMonthBitLength;
  /**
   * Years since 1970 [0,127]
   */
  unsigned year : kYearBitLength;
};

// endregion Date

}  // namespace inv
