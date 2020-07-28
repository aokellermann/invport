/**
 * @file transaction.cc
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#include "invport/detail/transaction.h"

namespace inv
{
namespace
{
constexpr static json::MemberName kJsonDateKey = "date";
constexpr static json::MemberName kJsonSymbolKey = "symbol";
constexpr static json::MemberName kJsonTypeKey = "type";
constexpr static json::MemberName kJsonPriceKey = "price";
constexpr static json::MemberName kJsonQuantityKey = "quantity";
constexpr static json::MemberName kJsonFeeKey = "fee";
constexpr static json::MemberName kJsonTagsKey = "tags";
constexpr static json::MemberName kJsonCommentKey = "comment";

constexpr const char* const kBuyStr = "Buy";
constexpr const char* const kSellStr = "Sell";
}  // namespace

Transaction Transaction::Factory(const json::Json& input_json) {
  Transaction tr;
  auto ec = tr.Deserialize(input_json);
  if (ec.Failure())
    throw std::runtime_error(ErrorCode("Transaction::Factory() failed", std::move(ec)));

  return tr;
}

const char* Transaction::TypeToString(const Transaction::Type t) { return t == BUY ? kBuyStr : kSellStr; }

[[nodiscard]] ValueWithErrorCode<json::Json> Transaction::Serialize() const
{
  json::Json json;

  try
  {
    json[kJsonDateKey] = date.count();
    json[kJsonSymbolKey] = symbol.Get();
    json[kJsonTypeKey] = type;
    json[kJsonPriceKey] = price;
    json[kJsonQuantityKey] = quantity;
    json[kJsonFeeKey] = fee;
    json[kJsonTagsKey] = tags;
    json[kJsonCommentKey] = comment;
  }
  catch (const std::exception& e)
  {
    return {json, ErrorCode("Transaction::Serialize() failed", ErrorCode(e.what()))};
  }
  return {json, {}};
}

ErrorCode Transaction::Deserialize(const json::Json& input_json)
{
  try
  {
    date = decltype(date)(input_json[kJsonDateKey]);
    symbol = decltype(symbol)(input_json[kJsonSymbolKey]);
    input_json.find(kJsonTypeKey)->get_to(type);
    input_json.find(kJsonPriceKey)->get_to(price);
    input_json.find(kJsonQuantityKey)->get_to(quantity);
    input_json.find(kJsonFeeKey)->get_to(fee);
    input_json.find(kJsonTagsKey)->get_to(tags);
    input_json.find(kJsonCommentKey)->get_to(comment);
  }
  catch (const std::exception& e)
  {
    return ErrorCode("Transaction::Deserialize() failed", ErrorCode(e.what()));
  }

  return {};
}

bool Transaction::operator==(const Transaction& other) const
{
  return date == other.date && symbol == other.symbol && type == other.type && price == other.price &&
         quantity == other.quantity && fee == other.fee && tags == other.tags && comment == other.comment;
}

}  // namespace inv