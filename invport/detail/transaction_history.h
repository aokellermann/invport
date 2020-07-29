/**
 * @file transaction_history.h
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#pragma once

#include <map>
#include <unordered_map>

#include "invport/detail/common.h"
#include "invport/detail/transaction.h"
#include "invport/detail/utils.h"

namespace inv
{
class TransactionHistory : public json::JsonBidirectionalSerializable
{
 public:
  using TransactionID = TransactionPool::TransactionID;
  using Transaction = TransactionPool::Transaction;

  using Date = util::Date;
  using TransactionSet = std::unordered_set<TransactionID>;
  using Timeline = std::map<Date, TransactionSet>;

  static TransactionHistory Factory(const json::Json& input_json);

  auto begin() { return timeline_.begin(); }
  auto end() { return timeline_.end(); }
  [[nodiscard]] auto begin() const { return timeline_.begin(); }
  [[nodiscard]] auto end() const { return timeline_.end(); }

  template <typename... Args>
  TransactionID Add(Args&&... args)
  {
    const auto& tr = TransactionPool::TransactionFactory(std::forward<Args>(args)...);
    timeline_[tr.date].insert(tr.id);
    return tr.id;
  }

  void Remove(const TransactionID& id)
  {
    if (const auto* tr_ptr = TransactionPool::Find(id); tr_ptr != nullptr)
    {
      if (const auto iter = timeline_.find(tr_ptr->date); iter != timeline_.end())
      {
        if (iter->second.erase(id) && iter->second.empty())
        {
          timeline_.erase(iter);
        }
      }
    }
  }

  [[nodiscard]] ValueWithErrorCode<json::Json> Serialize() const override;

  ErrorCode Deserialize(const json::Json& input_json) override;

  friend bool operator==(const TransactionHistory& lhs, const TransactionHistory& rhs)
  {
    return lhs.timeline_ == rhs.timeline_;
  }

  friend bool operator!=(const TransactionHistory& lhs, const TransactionHistory& rhs) { return !(lhs == rhs); }

 private:
  Timeline timeline_;
};
}  // namespace inv