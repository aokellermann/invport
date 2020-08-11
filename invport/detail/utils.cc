/**
 * @file utils.cc
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#include "invport/detail/utils.h"

namespace inv
{
const Date Date::kZero;

Date::Date(const std::string& str)
{
  std::vector<std::size_t> delimiter_indices;
  for (size_t i = 0; i < str.size(); ++i)
    if (str[i] == '/') delimiter_indices.emplace_back(i);

  if (delimiter_indices.size() != 2) throw std::runtime_error("Invalid Date format");

  const std::string day_string = str.substr(0, delimiter_indices[0]);
  const std::string month_string =
      str.substr(delimiter_indices[0] + 1, delimiter_indices[0] + 1 - delimiter_indices[1]);
  const std::string year_string = str.substr(delimiter_indices[1] + 1);

  const auto lday = std::strtoul(day_string.c_str(), nullptr, 10);
  const auto lmonth = std::strtoul(month_string.c_str(), nullptr, 10);
  const auto lyear = std::strtoul(year_string.c_str(), nullptr, 10) - kYearOffset;

  if (!(lday >= 1 && lday <= 31)) throw std::runtime_error("Invalid day of month");
  if (!(lmonth >= 1 && lmonth <= 12)) throw std::runtime_error("Invalid month of year");
  if (lyear >= std::pow(2, 7)) throw std::runtime_error("Invalid year");

  day = lday;
  month = lmonth;
  year = lyear;
}
}  // namespace inv
