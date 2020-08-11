/**
 * @file transactions.h
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#pragma once

#include <gtkmm.h>

#include "invport/detail/common.h"
#include "invport/detail/transaction_history.h"
#include "invport/widget/base.h"
#include "invport/widget/transaction_creator.h"
#include "invport/widget/util.h"

namespace inv::widget
{
class Transactions : public Gtk::Paned, private WidgetBase
{
 public:
  Transactions(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& bldr, TransactionHistory& th)
      : Gtk::Paned(obj),
        WidgetBase(bldr),
        transaction_history_(th),
        transaction_creator_(
            GetWidgetDerived<TransactionCreator>(builder, "transaction_creator_dialog", transaction_history_)),
        transactions_tree_view_(GetWidget<Gtk::TreeView>(builder, "transaction_history_tree_view")),
        transactions_list_store_(GetObject<Gtk::TreeStore>(builder, "transaction_history_tree_store"))
  {
    GetWidget<Gtk::Button>(bldr, "add_transaction_button")
        .signal_clicked()
        .connect(sigc::mem_fun(*this, &Transactions::AddTransactionButtonSignalActivate));

    GetWidget<Gtk::Button>(bldr, "remove_transaction_button")
        .signal_clicked()
        .connect(sigc::mem_fun(*this, &Transactions::RemoveTransactionButtonSignalActivate));

    GetWidget<Gtk::FileChooserButton>(bldr, "vanguard_file_chooser_button")
        .signal_file_set()
        .connect(sigc::mem_fun(*this, &Transactions::VanguardFileChooserButtonSignalFileSet));

    transaction_creator_.signal_hide().connect(sigc::mem_fun(*this, &Transactions::RefreshAndFlush));

    Refresh(false);
    transactions_tree_view_.expand_all();
  }

 private:
  void AddTransactionButtonSignalActivate();

  void RemoveTransactionButtonSignalActivate();

  void VanguardFileChooserButtonSignalFileSet();

  inline void RefreshAndFlush() { Refresh(true); }

  void Refresh(bool flush);

  TransactionHistory& transaction_history_;

  TransactionCreator& transaction_creator_;

  Gtk::TreeView& transactions_tree_view_;
  Gtk::TreeStore& transactions_list_store_;
};
}  // namespace inv::widget