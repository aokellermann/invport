/**
 * @file transaction_history.cc
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#include "transaction_history.h"

namespace inv
{
TransactionHistory TransactionHistory::Factory(const iex::json::Json& input_json)
{
  TransactionHistory th;
  auto ec = th.Deserialize(input_json);
  if (ec.Failure()) throw std::runtime_error(ErrorCode("TransactionHistory::Factory() failed", std::move(ec)));

  return th;
}

void TransactionHistory::Remove(const TransactionID& id)
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

[[nodiscard]] iex::SymbolMap<TransactionHistory::Totals> TransactionHistory::GetTotals(const Date& start_date,
                                                                                       const Date& end_date) const
{
  const auto begin = start_date.count() != 0 ? timeline_.lower_bound(start_date) : timeline_.begin();
  const auto end = end_date.count() != 0 ? timeline_.upper_bound(end_date) : timeline_.end();

  iex::SymbolMap<TransactionHistory::Totals> map;
  for (auto iter = begin; iter != end; ++iter)
  {
    for (const auto& id : iter->second)
    {
      const auto* tr_ptr = TransactionPool::Find(id);
      if (tr_ptr)
      {
        map[tr_ptr->symbol] += Totals(*tr_ptr);
      }
    }
  }
  return map;
}

ValueWithErrorCode<iex::json::Json> TransactionHistory::Serialize() const
{
  json::Json json = json::Json::array();

  try
  {
    for (const auto& [date, transactions] : timeline_)
    {
      for (const auto& id : transactions)
      {
        auto [j_tr, ec] = TransactionPool::Find(id)->Serialize();
        if (ec.Failure()) throw std::runtime_error(ec);

        json.emplace_back(std::move(j_tr));
      }
    }
  }
  catch (const std::exception& e)
  {
    return {json, ErrorCode("TransactionHistory::Serialize() failed", ErrorCode(e.what()))};
  }

  auto str = json.dump();
  return {json, {}};
}
ErrorCode TransactionHistory::Deserialize(const iex::json::Json& input_json)
{
  try
  {
    for (const auto& j_tr : input_json)
    {
      Add(j_tr);
    }
  }
  catch (const std::exception& e)
  {
    return ErrorCode("TransactionHistory::Deserialize() failed", ErrorCode(e.what()));
  }

  return {};
}
bool TransactionHistory::MemberwiseEquals(const TransactionHistory& other) const
{
  const auto [it1, it2] = std::mismatch(begin(), end(), other.begin(), other.end(), [](const auto& p1, const auto& p2) {
    if (p1.first != p2.first) return false;

    std::begin(*this);

    const auto [tr_it1, tr_it2] =
        std::mismatch(p1.second.begin(), p1.second.end(), p2.second.begin(), p2.second.end(),
                      [](const auto& tr1, const auto& tr2) { return tr1.MemberwiseEquals(tr2); });
    return tr_it1 == p1.second.end() && tr_it2 == p2.second.end();
  });

  return it1 == end() && it2 == other.end();
}
}  // namespace inv