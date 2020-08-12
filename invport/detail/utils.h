/**
 * @file utils.h
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#pragma once

#include <filesystem>
#include <limits>
#include <sstream>
#include <type_traits>

#include "invport/detail/common.h"

namespace inv
{
template <typename T1, typename T2>
bool FloatingEqual(T1 a, T2 b)
{
  const auto difference = std::fabs(a - b);
  return difference < std::numeric_limits<decltype(difference)>::epsilon();
}

template <typename T>
std::string ToString(T num)
{
  if constexpr (std::is_floating_point_v<T>)
  {
    std::string str = std::to_string(num);
    str.erase(str.find_last_not_of('0') + 1, std::string::npos);
    if (!str.empty() && str[str.size() - 1] == '.') str.pop_back();
    return str;
  }
  else if constexpr (std::is_integral_v<T>)
  {
    return std::to_string(num);
  }
}

template <typename T>
T ToNum(const std::string& str, int base = 10)
{
  if constexpr (std::is_same_v<T, float>) return std::strtof(str.c_str(), nullptr);
  if constexpr (std::is_same_v<T, double>) return std::strtod(str.c_str(), nullptr);
  if constexpr (std::is_same_v<T, long double>) return std::strtold(str.c_str(), nullptr);

  if constexpr (std::is_same_v<T, long>)  // NOLINT
    return std::strtol(str.c_str(), nullptr, base);
  if constexpr (std::is_same_v<T, long long>)  // NOLINT
    return std::strtoll(str.c_str(), nullptr, base);
  if constexpr (std::is_same_v<T, unsigned long>)  // NOLINT
    return std::strtoul(str.c_str(), nullptr, base);
  if constexpr (std::is_same_v<T, unsigned long long>)  // NOLINT
    return std::strtoull(str.c_str(), nullptr, base);
}

template <typename InputIt>
std::string Join(InputIt begin, InputIt end, const std::string& delimiter)
{
  std::string str;
  for (auto it = begin; it != end; ++it) str.append(*it + delimiter);

  if (begin != end) str.erase(str.size() - 1 - delimiter.size());

  return str;
}

std::stringstream Read(const std::filesystem::path& path);

std::vector<std::string> Split(std::stringstream sstr);

inline std::vector<std::string> Split(const std::string& str) { return Split(std::stringstream(str)); }

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
  static constexpr const uint8_t kMinDay = 1;
  static constexpr const uint8_t kMaxDay = 31;
  static constexpr const uint8_t kMinMonth = 1;
  static constexpr const uint8_t kMaxMonth = 12;
  static constexpr const uint8_t kMinYear = 0;
  static constexpr const uint8_t kMaxYear = 127;

  enum Format
  {
    DDMMYYYY,
    MMDDYYYY,
  };

  constexpr Date() noexcept : day(0), month(0), year(0) {}

  explicit Date(const json::Json& json) : Date(json.get<PrimitiveType>()) {}

  explicit Date(const std::string& str, Format format = DDMMYYYY);

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

  [[nodiscard]] std::string ToString(Format format = Format::DDMMYYYY) const;

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

template <typename T>
using DateMap = std::map<Date, T>;

// endregion Date

}  // namespace inv
