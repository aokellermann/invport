/**
 * @file vanguard.cc
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#include "invport/detail/vanguard.h"

#include <spdlog/spdlog.h>

#include <optional>
#include <regex>
#include <sstream>

namespace inv::vanguard
{
namespace
{
using Transaction = TransactionHistory::Transaction;

enum SMATCH_INDEX
{
  ACCOUNT_NUMBER = 1,
  TRADE_DATE = 2,
  SETTLEMENT_DATE = 4,
  TRANSACTION_TYPE = 6,
  TRANSACTION_DESCRIPTION,
  INVESTMENT_NAME,
  SYMBOL,
  SHARES,
  SHARE_PRICE,
  PRINCIPLE_AMOUNT,
  COMMISSION_FEES,
  NET_AMOUNT,
  ACCRUED_INTEREST,
  ACCOUNT_TYPE
};

std::optional<Transaction::Type> GetType(const std::smatch& sm)
{
  const std::string& type = sm[TRANSACTION_TYPE];
  if (type == "Buy") return Transaction::Type::BUY;
  if (type == "Sell") return Transaction::Type::SELL;
  return std::nullopt;
}

}  // namespace

TransactionHistory Parse(const fs::path& path)
{
  spdlog::info("Parsing Vanguard file with path {0}", path.string());

  std::vector<std::string> lines = Split(Read(path));
  TransactionHistory th(TransactionHistory::kTempTag);

  spdlog::info("Read {0} lines", lines.size());
  if (lines.empty()) return th;

  // (\d+)\,((\d\d\/){2}\d{4})\,((\d\d\/){2}\d{4})\,([^\,]*)\,([^\,]*)\,([^\,]*)\,([\w\*\+\#\^\=\.]*)\,(\-?[\d\.]*)\,(\-?[\d\.]*)\,(\-?[\d\.]*)\,(\-?[\d\.]*)\,(\-?[\d\.]*)\,(\-?[\d\.]*)\,([^\,]*)\,
  std::regex regex(
      "(\\d+)\\,((\\d\\d\\/){2}\\d{4})\\,((\\d\\d\\/){2}\\d{4})\\,([^\\,]*)\\,([^\\,]*)\\,"
      "([^\\,]*)\\,([\\w\\*\\+\\#\\^\\=\\.]*)\\,(\\-?[\\d\\.]*)\\,(\\-?[\\d\\.]*)\\,(\\-?["
      "\\d\\.]*)\\,(\\-?[\\d\\.]*)\\,(\\-?[\\d\\.]*)\\,(\\-?[\\d\\.]*)\\,([^\\,]*)\\,");
  std::smatch sm;

  for (auto l = lines.rbegin(); l != lines.rend(); ++l)
  {
    if (l->empty()) continue;
    if (!std::regex_match(*l, sm, regex)) break;

    auto type = GetType(sm);
    if (!type.has_value()) continue;

    auto account_number = ToNum<uint64_t>(sm[ACCOUNT_NUMBER].str());
    auto trade_date = Date(sm[TRADE_DATE].str(), Date::Format::MMDDYYYY);
    auto settlement_date = Date(sm[SETTLEMENT_DATE].str(), Date::Format::MMDDYYYY);
    auto desc = sm[TRANSACTION_DESCRIPTION].str();
    auto inv_name = sm[INVESTMENT_NAME].str();
    auto symbol = Symbol(sm[SYMBOL].str());
    auto quantity = ToNum<Transaction::Quantity>(sm[SHARES].str());
    auto price = ToNum<Price>(sm[SHARE_PRICE].str());
    auto principle = ToNum<Price>(sm[PRINCIPLE_AMOUNT].str());
    auto fees = ToNum<Price>(sm[COMMISSION_FEES].str());
    auto net = ToNum<Price>(sm[NET_AMOUNT].str());
    auto interest = ToNum<Price>(sm[ACCRUED_INTEREST].str());
    auto account_type = sm[ACCOUNT_TYPE].str();

    Transaction::Tags tags;
    tags.Add(kAccountNumberTag, ToString(account_number));

    if (type.value() == Transaction::Type::SELL) quantity = -quantity;

    auto tr_id = th.Add(std::move(trade_date), std::move(symbol), *type, price, quantity, fees, std::move(tags));
    spdlog::info("Parsed new transaction with id {0}", tr_id);
  }

  return th;
}
}  // namespace inv::vanguard
