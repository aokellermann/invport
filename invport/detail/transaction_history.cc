/**
 * @file transaction_history.cc
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#include "transaction_history.h"

namespace inv
{
TransactionHistory TransactionHistory::Factory(const iex::json::Json& input_json)
{
  TransactionHistory th;
  auto ec = th.Deserialize(input_json);
  if (ec.Failure()) throw std::runtime_error(ErrorCode("TransactionHistory::Factory() failed", std::move(ec)));

  return th;
}

ValueWithErrorCode<iex::json::Json> TransactionHistory::Serialize() const
{
  json::Json json = json::Json::array();

  try
  {
    for (const auto& [date, transactions] : timeline_)
    {
      for (const auto& id : transactions)
      {
        auto [j_tr, ec] = TransactionPool::Find(id)->Serialize();
        if (ec.Failure()) throw std::runtime_error(ec);

        json.emplace_back(std::move(j_tr));
      }
    }
  }
  catch (const std::exception& e)
  {
    return {json, ErrorCode("TransactionHistory::Serialize() failed", ErrorCode(e.what()))};
  }

  auto str = json.dump();
  return {json, {}};
}
ErrorCode TransactionHistory::Deserialize(const iex::json::Json& input_json)
{
  try
  {
    for (const auto& j_tr : input_json)
    {
      Add(j_tr);
    }
  }
  catch (const std::exception& e)
  {
    return ErrorCode("TransactionHistory::Deserialize() failed", ErrorCode(e.what()));
  }

  return {};
}
}  // namespace inv