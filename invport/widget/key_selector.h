/**
 * @file key_selector.h
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#pragma once

#include <gtkmm.h>

#include "invport/detail/common.h"
#include "invport/detail/keychain.h"
#include "invport/widget/base.h"

namespace inv::widget
{
class KeySelector : public Gtk::MessageDialog, private WidgetBase
{
  using Keychain = inv::key::Keychain;
  using Key = Keychain::Key;

 public:
  KeySelector(BaseObjectType* obj, const Glib::RefPtr<Gtk::Builder>& bldr, Keychain& keychain)
      : Gtk::MessageDialog(obj), WidgetBase(bldr), keychain_(keychain)
  {
    signal_show().connect(sigc::mem_fun(*this, &KeySelector::SignalShow));
    signal_response().connect(sigc::mem_fun(*this, &KeySelector::SignalActivate));
  }

 private:
  void SignalShow();

  void SignalActivate(int response_id);

  Keychain& keychain_;
};
}  // namespace inv::widget
