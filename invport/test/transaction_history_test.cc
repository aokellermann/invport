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
  const auto ts1 = inv::Timestamp(50);
  const auto ts2 = inv::Timestamp(100);
  const auto ts3 = inv::Timestamp(25);
  Transaction::Tags tags = {"tag1", "tag2", "tag3"};

  TransactionHistory th;
  th.Add(ts1, iex::Symbol("tsla"), Transaction::Type::BUY, 2, 3, 4, tags, "1");
  th.Add(ts2, iex::Symbol("aapl"), Transaction::Type::BUY, 5, 6, 7, tags, "2");
  th.Add(ts2, iex::Symbol("amd"), Transaction::Type::BUY, 8, 9, 10, tags, "3");
  th.Add(ts3, iex::Symbol("brk.a"), Transaction::Type::BUY, 11, 12, 13, tags, "4");
  th.Add(ts3, iex::Symbol("mj"), Transaction::Type::BUY, 14, 15, 16, tags, "5");
  th.Add(ts3, iex::Symbol("tsla"), Transaction::Type::BUY, 17, 18, 19, tags, "6");

  const auto [json, ec] = th.Serialize();
  ASSERT_EQ(ec, iex::ErrorCode());

  TransactionHistory th2;
  th2.Deserialize(json);

  auto iter1 = th.begin();
  auto iter2 = th2.begin();
  int i = 0;
  for (; iter1 != th.end() && iter2 != th2.end(); ++iter1, ++iter2)
  {
    EXPECT_EQ(iter1->first, iter2->first);
    EXPECT_EQ(iter1->second.size(), iter2->second.size());
  }

  EXPECT_EQ(iter1, th.end());
  EXPECT_EQ(iter2, th2.end());
}