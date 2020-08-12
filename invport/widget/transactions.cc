/**
 * @file transactions.cc
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#include "invport/widget/transactions.h"

#include "invport/detail/vanguard.h"

namespace inv::widget
{
void Transactions::AddTransactionButtonSignalActivate() { transaction_creator_.show(); }

void Transactions::RemoveTransactionButtonSignalActivate()
{
  auto selection = transactions_tree_view_.get_selection();
  if (selection)
  {
    std::string id_string;
    selection->get_selected()->get_value(TransactionHistory::Transaction::Field::UNIQUE_ID, id_string);
    if (!id_string.empty())
    {
      TransactionHistory::TransactionID id = std::strtoull(id_string.c_str(), nullptr, 10);
      transaction_history_.Remove(id);

      RefreshAndFlush();
    }
  }
}

void Transactions::VanguardFileChooserButtonSignalFileSet()
{
  const auto path = vanguard_file_chooser_button_.get_filename();
  vanguard_file_chooser_button_.unselect_all();

  auto th = vanguard::Parse(path);
  for (auto date_iter = th.begin(); date_iter != th.end();)
  {
    auto& [new_date, new_trs] = *date_iter;
    bool increment = true;

    if (const auto date_it = transaction_history_.Find(new_date); date_it != transaction_history_.end())
    {
      auto& trs = date_it->second;

      // Make hashtables of duplicate transactions
      TransactionHistory::MemberwiseTransactionMap<TransactionHistory::TransactionSet> counter;
      TransactionHistory::MemberwiseTransactionMap<TransactionHistory::TransactionSet> new_counter;
      for (const auto& tr : trs) counter[tr].insert(tr);
      for (const auto& tr : new_trs) new_counter[tr].insert(tr);

      // Only insert the difference of duplicates
      for (const auto& [new_id, new_ids] : new_counter)
      {
        const auto& ids = counter[new_id];
        auto num_duplicate_new_trs = static_cast<int>(ids.size());

        auto it = new_ids.begin();
        while (num_duplicate_new_trs > 0 && it != new_ids.end())
        {
          auto pair = th.Remove(*it);
          date_iter = pair.first;
          if (pair.second) increment = false;
          ++it;
          --num_duplicate_new_trs;
        }
      }
    }
    if (increment) ++date_iter;
  }

  transaction_history_.Merge(th);
  RefreshAndFlush();
}

void Transactions::Refresh(bool flush)
{
  transaction_history_.ToTreeStore(transactions_list_store_);
  transactions_tree_view_.expand_all();

  if (flush) transaction_history_.Flush();
}
}  // namespace inv::widget
