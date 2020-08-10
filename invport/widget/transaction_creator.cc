/**
 * @file transaction_creator.cc
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#include "invport/widget/transaction_creator.h"

#include "invport/detail/transaction.h"
#include "invport/widget/util.h"

namespace inv::widget
{
namespace
{
using Transaction = TransactionHistory::Transaction;
using Field = Transaction::Field;

constexpr const char* const kKeyEntryNames[Field::NUM_FIELDS] = {
    "transaction_creator_dialog_date_entry",     "transaction_creator_dialog_symbol_entry",
    "transaction_creator_dialog_type_combo_box", "transaction_creator_dialog_price_entry",
    "transaction_creator_dialog_quantity_entry", "transaction_creator_dialog_fee_entry",
    "transaction_creator_dialog_tags_entry",     "transaction_creator_dialog_comment_entry",
};

constexpr const char* const kErrorLabelName = "transaction_creator_dialog_error_label";
constexpr const char* const kErrorMessageLabelName = "transaction_creator_dialog_error_message_label";
}  // namespace
void TransactionCreator::SignalActivate(int response_id)
{
  // Hide on cancel.
  if (response_id == Gtk::ResponseType::RESPONSE_CANCEL)
  {
    hide();
    return;
  }

  try
  {
    Date date(GetWidget<Gtk::Entry>(builder, kKeyEntryNames[Field::DATE]).get_text());

    Symbol symbol(GetWidget<Gtk::Entry>(builder, kKeyEntryNames[Field::SYMBOL]).get_text());

    Transaction::Type type = static_cast<Transaction::Type>(
        GetWidget<Gtk::ComboBox>(builder, kKeyEntryNames[Field::TYPE]).get_active_row_number());

    Price price = Glib::Ascii::strtod(GetWidget<Gtk::Entry>(builder, kKeyEntryNames[Field::PRICE]).get_text());

    Transaction::Quantity quantity =
        Glib::Ascii::strtod(GetWidget<Gtk::Entry>(builder, kKeyEntryNames[Field::QUANTITY]).get_text());

    Price fee = Glib::Ascii::strtod(GetWidget<Gtk::Entry>(builder, kKeyEntryNames[Field::FEE]).get_text());

    std::istringstream sstr(GetWidget<Gtk::Entry>(builder, kKeyEntryNames[Field::TAGS]).get_text());
    std::string tok;
    Transaction::Tags tags;
    while (std::getline(sstr, tok, ','))
    {
      auto begin = tok.find_first_not_of(" \f\n\r\t\v");
      auto end = tok.find_last_not_of(" \f\n\r\t\v");
      tags.emplace_front(tok.substr(begin, end - begin));
    }

    Transaction::Comment comment(GetWidget<Gtk::Entry>(builder, kKeyEntryNames[Field::COMMENT]).get_text());

    transaction_history_.Add(date, symbol, type, price, quantity, fee, tags, comment);
  }
  catch (const std::exception& e)
  {
    DisplayError("Error:", e.what());

    // Don't hide so that user can try again.
    return;
  }

  // Only hide if success.
  hide();
}

void TransactionCreator::SignalHide()
{
  // Clear errors
  GetWidget<Gtk::Label>(builder, kErrorLabelName).set_text("");
  GetWidget<Gtk::Label>(builder, kErrorMessageLabelName).set_text("");
}

void TransactionCreator::DisplayError(const Glib::ustring& title, const Glib::ustring& message)
{
  GetWidget<Gtk::Label>(builder, kErrorLabelName).set_text(title);
  GetWidget<Gtk::Label>(builder, kErrorMessageLabelName).set_text(message);
}
}  // namespace inv::widget