/**
 * @file main_window.h
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#pragma once

#include <gtkmm.h>

#include "invport/detail/common.h"
#include "invport/detail/transaction_history.h"
#include "invport/widget/base.h"
#include "invport/widget/transactions.h"

namespace inv::widget
{
class MainWindow : public Gtk::ApplicationWindow, private WidgetBase
{
 public:
  MainWindow(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& bldr)
      : Gtk::ApplicationWindow(obj),
        WidgetBase(bldr),
        transaction_history_(TransactionHistory::Factory()),
        transaction_tab_(GetWidgetDerived<Transactions>(bldr, "transactions_tab_paned", transaction_history_))
  {
  }

 private:
  TransactionHistory transaction_history_;

  Transactions& transaction_tab_;
};
}  // namespace inv::widget