/**
 * @file key_selector.cc
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#include "invport/widget/key_selector.h"

#include "invport/widget/util.h"

namespace inv::widget
{
namespace
{
/**
 * Entries are indexed by KeyType.
 */
constexpr const char* const kKeyEntryNames[key::Keychain::NUM_KEYS] = {
    "key_selector_dialog_public_entry",
    "key_selector_dialog_secret_entry",
    "key_selector_dialog_public_sandbox_entry",
    "key_selector_dialog_secret_sandbox_entry",
};

constexpr const char* const kErrorLabelName = "key_selector_dialog_error_label";
constexpr const char* const kErrorMessageLabelName = "key_selector_dialog_error_message_label";

}  // namespace

void KeySelector::SignalShow()
{
  // Sets entry text to preexisting keys if available. This is useful if the user wants to change their keys.
  for (int i = 0; i < Keychain::NUM_KEYS; ++i)
  {
    GetWidget<Gtk::Entry>(builder, kKeyEntryNames[i]).set_text(keychain_.Get(static_cast<Keychain::KeyType>(i)).first);
  }
}

void KeySelector::SignalActivate(const int response_id)
{
  // Hide on cancel.
  if (response_id == Gtk::ResponseType::RESPONSE_CANCEL)
  {
    hide();
    return;
  }

  // Otherwise, try to set the keys. If any are invalid, display an error and let the user try again.
  for (int i = 0; i < Keychain::NUM_KEYS; ++i)
  {
    const auto ec = keychain_.Set(static_cast<Keychain::KeyType>(i),
                                  GetWidget<Gtk::Entry>(builder, kKeyEntryNames[i]).get_text(),
                                  i == Keychain::NUM_KEYS - 1);

    if (ec.Failure())
    {
      GetWidget<Gtk::Label>(builder, kErrorLabelName).set_text("Error:");
      GetWidget<Gtk::Label>(builder, kErrorMessageLabelName).set_text(ec);

      // Don't hide so that user can try again.
      return;
    }
  }

  // Only hide if success.
  hide();
}
}  // namespace inv::widget