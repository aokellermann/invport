/**
 * @file transaction_creator.h
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#pragma once

#include <gtkmm.h>

#include "invport/detail/transaction_history.h"
#include "invport/detail/common.h"
#include "invport/widget/base.h"

namespace inv::widget
{
/**
 * This dialog is used to allow manual key entry from the user.
 */
class TransactionCreator : public Gtk::MessageDialog, private WidgetBase
{
 public:
  TransactionCreator(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& bldr, TransactionHistory& th)
      : Gtk::MessageDialog(obj), WidgetBase(bldr), transaction_history_(th)
  {
    signal_response().connect(sigc::mem_fun(*this, &TransactionCreator::SignalActivate));
    signal_hide().connect(sigc::mem_fun(*this, &TransactionCreator::SignalHide));
  }

 private:
  void SignalActivate(int response_id);

  void SignalHide();

  void DisplayError(const Glib::ustring& title, const Glib::ustring& message);

  TransactionHistory& transaction_history_;
};
}  // namespace inv::widget
