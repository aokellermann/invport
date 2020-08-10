/**
 * @file utils_test.cc
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#include "invport/detail/utils.h"

#include <gtest/gtest.h>

using Date = inv::Date;

TEST(Date, DefaultConstructor)
{
  Date date;
  EXPECT_TRUE(date.IsZero());
  EXPECT_EQ(date.ToPrimitive(), 0);
  EXPECT_ANY_THROW({ auto _ = date.ToString(); });
}

TEST(Date, Zero)
{
  EXPECT_TRUE(Date::kZero.IsZero());
  EXPECT_EQ(Date::kZero.ToPrimitive(), 0);
  EXPECT_ANY_THROW({ auto _ = Date::kZero.ToString(); });
}

TEST(Date, Comparison)
{
  const auto ts1 = inv::Date(14, 7, 2015);
  const auto ts2 = inv::Date(6, 8, 2015);
  const auto ts3 = inv::Date(2, 12, 2017);
  const auto ts4 = inv::Date(30, 1, 2018);
  const auto ts5 = inv::Date(6, 8, 2020);

  const std::vector<Date> sorted = {ts1, ts2, ts3, ts4, ts5};
  EXPECT_TRUE(std::is_sorted(sorted.begin(), sorted.end(), std::less<>()));
  EXPECT_TRUE(std::is_sorted(sorted.rbegin(), sorted.rend(), std::greater<>()));
}

struct DateConstructorInitParams
{
  std::string str;
  int day, month, year;
  bool throws;
};

struct DateConstructor : ::testing::TestWithParam<DateConstructorInitParams>
{
  Date DateFromString() { return Date(GetParam().str); }
  Date DateFromComponents() { return Date(GetParam().day, GetParam().month, GetParam().year); }
};

TEST_P(DateConstructor, StringConstructor)
{
  if (GetParam().throws)
    EXPECT_ANY_THROW(DateFromString());
  else
    EXPECT_NO_THROW(DateFromString());
}

TEST_P(DateConstructor, ComponentConstructor)
{
  if (GetParam().throws)
    EXPECT_ANY_THROW(DateFromComponents());
  else
    EXPECT_NO_THROW(DateFromComponents());
}

TEST_P(DateConstructor, Correctness)
{
  if (!GetParam().throws)
  {
    EXPECT_NO_THROW({
      Date date_s = DateFromString();
      Date date_c = DateFromComponents();
      for (const auto& date : {date_s, date_c})
      {
        EXPECT_EQ(date.day, GetParam().day);
        EXPECT_EQ(date.month, GetParam().month);
        EXPECT_EQ(date.year, GetParam().year - Date::kYearOffset);
      }
    });
  }
}

TEST_P(DateConstructor, ToPrimitive)
{
  if (!GetParam().throws)
  {
    EXPECT_NO_THROW({
      Date date_s = DateFromString();
      Date date_c = DateFromComponents();
      for (const auto& date : {date_s, date_c})
      {
        Date::PrimitiveType manual = (static_cast<int>(date.day)) |
                                     (static_cast<int>(date.month) << Date::kDayBitLength) |
                                     (static_cast<int>(date.year) << (Date::kDayBitLength + Date::kMonthBitLength));
        EXPECT_EQ(date.ToPrimitive(), manual);
      }
    });
  }
}

TEST_P(DateConstructor, ToString)
{
  if (!GetParam().throws)
  {
    EXPECT_NO_THROW({
      Date date_s = DateFromString();
      Date date_c = DateFromComponents();
      for (const auto& date : {date_s, date_c})
      {
        auto str = GetParam().str;
        auto i1 = str.find('/');
        auto i2 = str.rfind('/');
        auto day = str.substr(0, i1);
        auto mon = str.substr(i1 + 1, i2 - (i1 + 1));
        auto yr = str.substr(i2 + 1);
        day = day.substr(day.find_first_not_of('0'));
        mon = mon.substr(mon.find_first_not_of('0'));
        yr = yr.substr(yr.find_first_not_of('0'));
        EXPECT_EQ(date.ToString(), std::string(day + '/' + mon + '/' + yr));
      }
    });
  }
}

TEST_P(DateConstructor, Equality)
{
  if (!GetParam().throws)
  {
    EXPECT_NO_THROW({
      Date date_s = DateFromString();
      Date date_c = DateFromComponents();
      EXPECT_EQ(date_s, date_c);
    });
  }
}

TEST_P(DateConstructor, IsZero)
{
  if (!GetParam().throws)
  {
    EXPECT_NO_THROW({
      Date date_s = DateFromString();
      Date date_c = DateFromComponents();
      EXPECT_FALSE(date_s.IsZero());
      EXPECT_FALSE(date_c.IsZero());
    });
  }
}

INSTANTIATE_TEST_CASE_P(ValidParams,
                        DateConstructor,
                        testing::Values(DateConstructorInitParams{"3/2/2020", 3, 2, 2020, false},
                                        DateConstructorInitParams{"9/9/1980", 9, 9, 1980, false},
                                        DateConstructorInitParams{"4/11/1988", 4, 11, 1988, false},
                                        DateConstructorInitParams{"30/6/2011", 30, 6, 2011, false},
                                        DateConstructorInitParams{"25/10/2018", 25, 10, 2018, false},
                                        DateConstructorInitParams{"03/2/2020", 3, 2, 2020, false},
                                        DateConstructorInitParams{"9/09/1980", 9, 9, 1980, false},
                                        DateConstructorInitParams{"07/05/1988", 7, 5, 1988, false}));

INSTANTIATE_TEST_CASE_P(InvalidParams,
                        DateConstructor,
                        testing::Values(DateConstructorInitParams{"0/3/2020", 0, 3, 2020, true},
                                        DateConstructorInitParams{"9/0/1980", 9, 0, 1980, true},
                                        DateConstructorInitParams{"11/4/1960", 11, 4, 1960, true},
                                        DateConstructorInitParams{"30/13/2011", 30, 13, 2011, true},
                                        DateConstructorInitParams{"32/2/2018", 32, 2, 2018, true},
                                        DateConstructorInitParams{"36/15/2020", 36, 15, 2020, true},
                                        DateConstructorInitParams{"9/2/2098", 9, 2, 2098, true}));
