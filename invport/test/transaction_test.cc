/**
 * @file transaction_test.cc
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#include "invport/detail/transaction.h"

#include <gtest/gtest.h>

#include "invport/detail/common.h"
#include "invport/detail/utils.h"

using TransactionPool = inv::TransactionPool;
using Transaction = TransactionPool::Transaction;

const auto& tr1 =
    TransactionPool::TransactionFactory(inv::Date::Today(), inv::Symbol("tsla"), Transaction::Type::SELL, 2, 3,
                                        4, Transaction::Tags{"tag1", "tag2", "tag3"}, "comment");

TEST(Transaction, Serialization)
{
  // Serialize
  const auto [json, ec] = tr1.Serialize();
  ASSERT_EQ(ec, iex::ErrorCode());

  // Deserialize
  const auto& tr2 = TransactionPool::TransactionFactory(json);

  EXPECT_TRUE(tr1.MemberwiseEquals(tr2));
}

TEST(Transaction, UniqueID)
{
  const auto [json, ec] = tr1.Serialize();
  ASSERT_EQ(ec, iex::ErrorCode());

  std::size_t num_transactions = 100;
  std::unordered_set<Transaction::ID> ids;
  for (std::size_t i = 0; i < num_transactions; ++i)
  {
    ids.insert(TransactionPool::TransactionFactory(json).id);
  }

  EXPECT_EQ(num_transactions, ids.size());
}

TEST(Transaction, Find)
{
  const auto [json, ec] = tr1.Serialize();
  ASSERT_EQ(ec, iex::ErrorCode());

  std::size_t num_transactions = 100;
  std::unordered_set<Transaction::ID> ids;
  for (std::size_t i = 0; i < num_transactions; ++i)
  {
    ids.insert(TransactionPool::TransactionFactory(json).id);
  }

  for (const auto& id : ids)
  {
    const auto* ptr = TransactionPool::Find(id);
    EXPECT_TRUE(ptr);
    if (ptr)
    {
      EXPECT_EQ(id, ptr->id);
    }
  }
}
