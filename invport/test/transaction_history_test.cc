/**
 * @file transaction_history_test.cc
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#include "invport/detail/transaction_history.h"

#include <gtest/gtest.h>

#include "invport/detail/common.h"
#include "invport/detail/transaction.h"

using TransactionHistory = inv::TransactionHistory;
using Transaction = TransactionHistory::Transaction;

TEST(TransactionHistory, Serialize)
{
  const auto ts1 = inv::Date(50);
  const auto ts2 = inv::Date(100);
  const auto ts3 = inv::Date(25);
  Transaction::Tags tags = {"tag1", "tag2", "tag3"};

  TransactionHistory th(TransactionHistory::kTempTag);
  th.Add(ts1, iex::Symbol("tsla"), Transaction::Type::BUY, 2, 3, 4, tags, "1");
  th.Add(ts2, iex::Symbol("aapl"), Transaction::Type::BUY, 5, 6, 7, tags, "2");
  th.Add(ts2, iex::Symbol("amd"), Transaction::Type::BUY, 8, 9, 10, tags, "3");
  th.Add(ts3, iex::Symbol("brk.a"), Transaction::Type::BUY, 11, 12, 13, tags, "4");
  th.Add(ts3, iex::Symbol("mj"), Transaction::Type::BUY, 14, 15, 16, tags, "5");
  th.Add(ts3, iex::Symbol("tsla"), Transaction::Type::BUY, 17, 18, 19, tags, "6");

  const auto [json, ec] = th.Serialize();
  ASSERT_EQ(ec, iex::ErrorCode());

  TransactionHistory th2(TransactionHistory::kTempTag);
  th2.Deserialize(json);

  auto iter1 = th.begin();
  auto iter2 = th2.begin();
  for (; iter1 != th.end() && iter2 != th2.end(); ++iter1, ++iter2)
  {
    EXPECT_EQ(iter1->first, iter2->first);
    EXPECT_EQ(iter1->second.size(), iter2->second.size());
  }

  EXPECT_EQ(iter1, th.end());
  EXPECT_EQ(iter2, th2.end());
}

TEST(TransactionHistory, GetQuantities)
{
  const auto ts1 = inv::Date(14, 7, 2015);
  const auto ts2 = inv::Date(6, 8, 2015);
  const auto ts3 = inv::Date(2, 12, 2017);
  const auto ts4 = inv::Date(30, 1, 2018);
  const auto ts5 = inv::Date(6, 8, 2020);
  Transaction::Tags tags = {"tag1", "tag2", "tag3"};

  TransactionHistory th = TransactionHistory::Factory(std::to_string(std::rand()) + "th", inv::file::Directory::TEMP);
  th.Add(ts1, iex::Symbol("tsla"), Transaction::Type::BUY, 2, 3, 4, tags, "1");
  th.Add(ts2, iex::Symbol("aapl"), Transaction::Type::BUY, 5, 6, 7, tags, "2");
  th.Add(ts2, iex::Symbol("amd"), Transaction::Type::BUY, 8, 9, 10, tags, "3");
  th.Add(ts3, iex::Symbol("brk.a"), Transaction::Type::BUY, 11, 12, 13, tags, "4");
  th.Add(ts3, iex::Symbol("mj"), Transaction::Type::BUY, 14, 15, 16, tags, "5");
  th.Add(ts3, iex::Symbol("tsla"), Transaction::Type::SELL, 1, 1, 19, tags, "6");
  th.Add(ts4, iex::Symbol("tsla"), Transaction::Type::SELL, 1, 2, 19, tags, "6");
  th.Add(ts4, iex::Symbol("brk.a"), Transaction::Type::BUY, 11, 12, 13, tags, "4");
  th.Add(ts5, iex::Symbol("brk.a"), Transaction::Type::SELL, 5, 4, 13, tags, "4");

  auto totals_map = th.GetTotals();

  EXPECT_EQ(totals_map[iex::Symbol("tsla")].spent, 3);
  EXPECT_EQ(totals_map[iex::Symbol("aapl")].spent, 30);
  EXPECT_EQ(totals_map[iex::Symbol("amd")].spent, 72);
  EXPECT_EQ(totals_map[iex::Symbol("brk.a")].spent, 244);
  EXPECT_EQ(totals_map[iex::Symbol("mj")].spent, 210);

  EXPECT_EQ(totals_map[iex::Symbol("tsla")].quantity, 0);
  EXPECT_EQ(totals_map[iex::Symbol("aapl")].quantity, 6);
  EXPECT_EQ(totals_map[iex::Symbol("amd")].quantity, 9);
  EXPECT_EQ(totals_map[iex::Symbol("brk.a")].quantity, 20);
  EXPECT_EQ(totals_map[iex::Symbol("mj")].quantity, 15);

  EXPECT_EQ(totals_map[iex::Symbol("tsla")].fees, 42);
  EXPECT_EQ(totals_map[iex::Symbol("aapl")].fees, 7);
  EXPECT_EQ(totals_map[iex::Symbol("amd")].fees, 10);
  EXPECT_EQ(totals_map[iex::Symbol("brk.a")].fees, 39);
  EXPECT_EQ(totals_map[iex::Symbol("mj")].fees, 16);

  totals_map = th.GetTotals({}, ts3);

  EXPECT_EQ(totals_map[iex::Symbol("tsla")].spent, 5);
  EXPECT_EQ(totals_map[iex::Symbol("aapl")].spent, 30);
  EXPECT_EQ(totals_map[iex::Symbol("amd")].spent, 72);
  EXPECT_EQ(totals_map[iex::Symbol("brk.a")].spent, 132);
  EXPECT_EQ(totals_map[iex::Symbol("mj")].spent, 210);

  EXPECT_EQ(totals_map[iex::Symbol("tsla")].quantity, 2);
  EXPECT_EQ(totals_map[iex::Symbol("aapl")].quantity, 6);
  EXPECT_EQ(totals_map[iex::Symbol("amd")].quantity, 9);
  EXPECT_EQ(totals_map[iex::Symbol("brk.a")].quantity, 12);
  EXPECT_EQ(totals_map[iex::Symbol("mj")].quantity, 15);

  EXPECT_EQ(totals_map[iex::Symbol("tsla")].fees, 23);
  EXPECT_EQ(totals_map[iex::Symbol("aapl")].fees, 7);
  EXPECT_EQ(totals_map[iex::Symbol("amd")].fees, 10);
  EXPECT_EQ(totals_map[iex::Symbol("brk.a")].fees, 13);
  EXPECT_EQ(totals_map[iex::Symbol("mj")].fees, 16);
}