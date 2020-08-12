/**
 * @file portfolio.cc
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#include "invport/widget/portfolio.h"

#include <spdlog/spdlog.h>

namespace inv::widget
{
namespace
{
enum Columns
{
  TIMESTAMP,
  SYMBOL,
  PRICE,
  SHARES,
  VALUE,
  GAIN,
  GAIN_PERCENT,
  GAIN_1D,
  GAIN_1D_PERCENT,
};
}

void Portfolio::RefreshGains()
{
  auto totals = transaction_history_.GetTotals(true);
  iex::SymbolSet symbols;
  for (const auto& [symbol, total] : totals) symbols.insert(symbol);

  auto resp = iex::Get<iex::Endpoint::Type::QUOTE>(symbols);
  if (resp.second.Failure())
  {
    spdlog::error(ErrorCode("Portfolio::RefreshGains failed", resp.second));
    return;
  }

  portfolio_tree_store_.clear();

  Price total_value = 0;
  Price total_spent = 0;
  Price total_gain = 0;
  Price total_value_1d = 0;
  Price total_gain_1d = 0;
  for (const auto& [symbol, quote] : resp.first)
  {
    auto row = *portfolio_tree_store_.append();
    auto ts = quote->Get<iex::Quote::MemberType::LATEST_UPDATE>();
    if (ts.has_value())
    {
      std::time_t seconds = std::chrono::duration_cast<std::chrono::seconds>(ts.value()).count();
      auto* tm = std::localtime(&seconds);
      char buffer[26];
      std::strftime(buffer, 26, "%m-%d %I:%M:%S", tm);
      row.set_value(TIMESTAMP, std::string(buffer));
    }
    row.set_value(SYMBOL, symbol.Get());

    auto final_quantity = totals[symbol].quantity;
    row.set_value(SHARES, ToString(final_quantity));

    auto price = quote->Get<iex::Quote::MemberType::LATEST_PRICE>();
    if (!price.has_value()) continue;

    row.set_value(PRICE, ToString(price.value()));

    auto final_value = final_quantity * price.value();

    total_value += final_value;
    row.set_value(VALUE, ToString(final_value));

    auto spent = totals[symbol].spent;
    total_spent += spent;

    auto gain = final_value - spent;
    auto gain_percent = 100.0 * ((final_value / spent) - 1.0);

    total_gain += gain;
    row.set_value(GAIN, ToString(gain));
    row.set_value(GAIN_PERCENT, ToString(gain_percent));

    auto price_1d = quote->Get<iex::Quote::MemberType::PREVIOUS_CLOSE>();
    if (price_1d.has_value())
    {
      auto value_1d = final_quantity * price_1d.value();
      auto gain_1d = final_value - value_1d;
      auto gain_1d_percent = 100.0 * ((final_value / value_1d) - 1.0);

      total_value_1d += value_1d;
      total_gain_1d += gain_1d;
      row.set_value(GAIN_1D, ToString(gain_1d));
      row.set_value(GAIN_1D_PERCENT, ToString(gain_1d_percent));
    }
  }

  auto row = *portfolio_tree_store_.append();
  row.set_value(0, std::string("TOTALS"));
  row.set_value(Columns::VALUE, ToString(total_value));
  row.set_value(Columns::GAIN, ToString(total_gain));
  row.set_value(Columns::GAIN_PERCENT, ToString(100.0 * ((total_value / total_spent) - 1.0)));
  row.set_value(Columns::GAIN_1D, ToString(total_gain_1d));
  row.set_value(Columns::GAIN_1D_PERCENT, ToString(100.0 * ((total_value / total_value_1d) - 1.0)));
}
}  // namespace inv::widget