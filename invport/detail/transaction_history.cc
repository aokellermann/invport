/**
 * @file transaction_history.cc
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#include "transaction_history.h"

#include <spdlog/spdlog.h>

namespace inv
{
TransactionHistory TransactionHistory::Factory(const file::Path& relative_path, file::Directory directory)
{
  TransactionHistory th(relative_path, directory);
  auto vec = th.ReadFile();
  if (vec.second.Failure())
    throw std::runtime_error(ErrorCode("TransactionHistory::Factory() failed", std::move(vec.second)));

  if (!vec.first.empty())
  {
    auto ec = th.Deserialize(json::Json::parse(vec.first));
    if (ec.Failure()) throw std::runtime_error(ErrorCode("TransactionHistory::Factory() failed", std::move(ec)));
  }
  return th;
}

void TransactionHistory::ToTreeStore(Gtk::TreeStore& tree) const
{
  tree.clear();

  for (const auto& [date, transactions] : timeline_)
  {
    if (!transactions.empty())
    {
      Gtk::TreeRow date_row = *tree.append();
      date_row.set_value(Transaction::Field::DATE, date.ToString(Date::Format::MMDDYYYY));
      for (const auto& id : transactions)
      {
        Gtk::TreeRow tr_row = *tree.append(date_row.children());
        TransactionPool::Find(id)->ToTreeRow(tr_row);
      }
    }
  }
}

std::pair<TransactionHistory::Timeline::iterator, bool> TransactionHistory::Remove(const TransactionID& id)
{
  if (const auto* tr_ptr = TransactionPool::Find(id); tr_ptr != nullptr)
  {
    if (const auto iter = timeline_.find(tr_ptr->date); iter != timeline_.end())
    {
      if (iter->second.erase(id))
      {
        if (iter->second.empty()) return {timeline_.erase(iter), true};
        return {iter, false};
      }
    }
  }
  return {timeline_.end(), false};
}

void TransactionHistory::Merge(const TransactionHistory& other,
                               const std::unordered_set<TransactionHistory::Transaction::Tag>& exclude_tags)
{
  TransactionSet exclude_set;
  for (const auto& tag : exclude_tags) exclude_set.merge(GetAssociatedTransactions(tag));

  for (const auto& [date, trs] : other)
  {
    auto& date_ref = timeline_[date];
    for (const auto& tr_id : trs)
      if (!exclude_set.count(tr_id)) date_ref.insert(tr_id);
  }
}

[[nodiscard]] TransactionHistory::TotalsMap TransactionHistory::GetTotals(const Date& start_date,
                                                                          const Date& end_date,
                                                                          bool reset_on_sell_all) const
{
  const auto begin = !start_date.IsZero() ? timeline_.lower_bound(start_date) : timeline_.begin();
  const auto end = !end_date.IsZero() ? timeline_.upper_bound(end_date) : timeline_.end();

  iex::SymbolMap<TransactionHistory::Totals> map;
  for (auto iter = begin; iter != end; ++iter)
  {
    for (const auto& id : iter->second)
    {
      const auto* tr_ptr = TransactionPool::Find(id);
      if (tr_ptr)
      {
        auto& ref = (map[tr_ptr->symbol] += Totals(*tr_ptr, reset_on_sell_all));
        if (reset_on_sell_all && ref.quantity == 0) map.erase(tr_ptr->symbol);
      }
    }
  }
  return map;
}

[[nodiscard]] TransactionHistory::TotalsMap TransactionHistory::GetTotals(bool reset_on_sell_all) const
{
  return GetTotals(Date::Zero(), Date::Zero(), reset_on_sell_all);
}

TransactionHistory::TransactionSet TransactionHistory::GetAssociatedTransactions(
    const TransactionHistory::Transaction::Tag& tag)
{
  TransactionHistory::TransactionSet set;
  for (const auto& [date, transactions] : timeline_)
  {
    for (const auto& id : transactions)
    {
      if (TransactionPool::Find(id)->tags.count(tag)) set.insert(id);
    }
  }
  return set;
}

std::unordered_map<TransactionHistory::Transaction::Tag, TransactionHistory::TransactionSet>
TransactionHistory::GetAssociatedTransactions()
{
  std::unordered_map<TransactionHistory::Transaction::Tag, TransactionHistory::TransactionSet> map;
  for (const auto& [date, transactions] : timeline_)
  {
    for (const auto& id : transactions)
    {
      for (const auto& tag : TransactionPool::Find(id)->tags) map[tag.first].insert(id);
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

    const auto [tr_it1, tr_it2] = std::mismatch(
        p1.second.begin(), p1.second.end(), p2.second.begin(), p2.second.end(), [](const auto& tr1, const auto& tr2) {
          return TransactionPool::Find(tr1)->MemberwiseEquals(*TransactionPool::Find(tr2));
        });
    return tr_it1 == p1.second.end() && tr_it2 == p2.second.end();
  });

  return it1 == end() && it2 == other.end();
}

void TransactionHistory::Flush()
{
  try
  {
    auto vec = Serialize();
    if (vec.second.Failure()) throw std::runtime_error(vec.second);

    auto ec = WriteFile(vec.first.dump());
    if (ec.Failure()) throw std::runtime_error(ec);
  }
  catch (const std::exception& e)
  {
    spdlog::critical(ErrorCode("TransactionHistory::Flush failed", ErrorCode(e.what())));
  }
}

}  // namespace inv