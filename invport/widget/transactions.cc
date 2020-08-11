/**
 * @file transactions.cc
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#include "invport/widget/transactions.h"

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

}

void Transactions::Refresh(bool flush)
{
  transaction_history_.ToTreeStore(transactions_list_store_);
  transactions_tree_view_.expand_all();

  if (flush) transaction_history_.Flush();
}
}  // namespace inv::widget
