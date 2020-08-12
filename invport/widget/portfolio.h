/**
 * @file portfolio.h
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#pragma once

#include <gtkmm.h>

#include "invport/detail/common.h"
#include "invport/detail/transaction_history.h"
#include "invport/widget/base.h"
#include "invport/widget/util.h"

namespace inv::widget
{
class Portfolio : public Gtk::Notebook, private WidgetBase
{
 public:
  Portfolio(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& bldr, TransactionHistory& th)
      : Gtk::Notebook(obj),
        WidgetBase(bldr),
        transaction_history_(th),
        portfolio_tree_view_(GetWidget<Gtk::TreeView>(builder, "portfolio_gains_tree_view")),
        portfolio_tree_store_(GetObject<Gtk::TreeStore>(builder, "portfolio_gains_tree_store"))
  {
    GetWidget<Gtk::Button>(bldr, "portfolio_gains_reload_button")
        .signal_clicked()
        .connect(sigc::mem_fun(*this, &Portfolio::RefreshGains));

    RefreshGains();
  }

 private:
  void RefreshGains();

  TransactionHistory& transaction_history_;

  Gtk::TreeView& portfolio_tree_view_;
  Gtk::TreeStore& portfolio_tree_store_;
};
}  // namespace inv::widget