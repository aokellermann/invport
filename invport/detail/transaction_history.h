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
/**
 * Represents a timeline of transactions.
 */
class TransactionHistory : public json::JsonBidirectionalSerializable
{
 public:
  using TransactionID = TransactionPool::TransactionID;
  using Transaction = TransactionPool::Transaction;
  using Totals = Transaction::Totals;

  using Date = util::Date;
  using TransactionSet = std::unordered_set<TransactionID>;
  using Timeline = std::map<Date, TransactionSet>;

  static TransactionHistory Factory(const json::Json& input_json);

  auto begin() { return timeline_.begin(); }
  auto end() { return timeline_.end(); }
  [[nodiscard]] auto begin() const { return timeline_.begin(); }
  [[nodiscard]] auto end() const { return timeline_.end(); }

  /**
   * Adds a transaction to the timeline.
   * @param args Parameter pack of args passed to Transaction::Factory
   * @return id of new transaction
   */
  template <typename... Args>
  TransactionID Add(Args&&... args)
  {
    const auto& tr = TransactionPool::TransactionFactory(std::forward<Args>(args)...);
    timeline_[tr.date].insert(tr.id);
    return tr.id;
  }

  /**
   * Removes a transaction from the timeline.
   * @param id the id of the transaction to remove
   */
  void Remove(const TransactionID& id);

  /**
   * Merges two TransactionHistorys
   */
  friend void Merge(TransactionHistory& lhs, TransactionHistory& rhs) { lhs.timeline_.merge(rhs.timeline_); }

  /**
   * Gets the total number of shares per symbol until then given date.
   * @param start_date the starting date, inclusive, or zero, which will evaluate from begin()
   * @param end_date the stopping date, inclusive, or zero, which will evaluate until end()
   * @return symbol map Totals
   */
  [[nodiscard]] iex::SymbolMap<Totals> GetTotals(const Date& start_date = {}, const Date& end_date = {}) const;

  [[nodiscard]] ValueWithErrorCode<json::Json> Serialize() const override;

  ErrorCode Deserialize(const json::Json& input_json) override;

  friend bool operator==(const TransactionHistory& lhs, const TransactionHistory& rhs)
  {
    return lhs.timeline_ == rhs.timeline_;
  }

  friend bool operator!=(const TransactionHistory& lhs, const TransactionHistory& rhs) { return !(lhs == rhs); }

  [[nodiscard]] bool MemberwiseEquals(const TransactionHistory& other) const;

 private:
  Timeline timeline_;
};
}  // namespace inv