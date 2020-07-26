/**
 * @file transaction_test.cc
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#include "invport/detail/transaction.h"

#include <gtest/gtest.h>

#include "invport/detail/common.h"
#include "invport/detail/utils.h"

using Transaction = inv::Transaction;

const Transaction basic_sell_transaction(inv::util::GetTodayDate(), inv::Symbol("tsla"), Transaction::Type::SELL, 2, 3,
                                         4, {"tag1", "tag2", "tag3"}, "comment");

TEST(Transaction, Serialize)
{
  const auto [json, ec] = basic_sell_transaction.Serialize();
  ASSERT_EQ(ec, iex::ErrorCode());

  Transaction tr2;
  tr2.Deserialize(json);

  EXPECT_EQ(basic_sell_transaction, tr2);
}
