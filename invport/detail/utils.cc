/**
 * @file utils.cc
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#include "invport/detail/utils.h"

#include <fstream>

namespace inv
{
const Date Date::kZero;

std::stringstream Read(const std::filesystem::path& path)
{
  std::ifstream ifstream(path);
  std::stringstream sstr;
  sstr << ifstream.rdbuf();
  return sstr;
}

std::vector<std::string> Split(std::stringstream sstr)
{
  std::vector<std::string> lines;
  std::string line;
  while (std::getline(sstr, line)) lines.emplace_back(std::move(line));
  return lines;
}

Date::Date(const std::string& str, Format format)
{
  std::vector<std::size_t> delimiter_indices;
  for (size_t i = 0; i < str.size(); ++i)
    if (str[i] == '/') delimiter_indices.emplace_back(i);

  if (delimiter_indices.size() != 2) throw std::runtime_error("Invalid Date format");

  const std::string day_string = str.substr(0, delimiter_indices[0]);
  const std::string month_string =
      str.substr(delimiter_indices[0] + 1, delimiter_indices[0] + 1 - delimiter_indices[1]);
  const std::string year_string = str.substr(delimiter_indices[1] + 1);

  const auto first = std::strtoul(day_string.c_str(), nullptr, 10);
  const auto second = std::strtoul(month_string.c_str(), nullptr, 10);
  const auto third = std::strtoul(year_string.c_str(), nullptr, 10) - kYearOffset;

  const auto& day_l = format == DDMMYYYY ? first : second;
  const auto& month_l = format == DDMMYYYY ? second : first;
  const auto& year_l = third;

  if (!(day_l >= kMinDay && day_l <= kMaxDay)) throw std::runtime_error("Invalid day of month");
  if (!(month_l >= kMinMonth && month_l <= kMaxMonth)) throw std::runtime_error("Invalid month of year");
  if (year_l >= kMaxYear) throw std::runtime_error("Invalid year");

  day = day_l;
  month = month_l;
  year = year_l;
}
}  // namespace inv
