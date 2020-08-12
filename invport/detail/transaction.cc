/**
 * @file transaction.cc
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#include "invport/detail/transaction.h"

namespace inv::detail
{
namespace
{
constexpr json::MemberName kJsonDateKey = "date";
constexpr json::MemberName kJsonSymbolKey = "symbol";
constexpr json::MemberName kJsonTypeKey = "type";
constexpr json::MemberName kJsonPriceKey = "price";
constexpr json::MemberName kJsonQuantityKey = "quantity";
constexpr json::MemberName kJsonFeeKey = "fee";
constexpr json::MemberName kJsonTagsKey = "tags";
constexpr json::MemberName kJsonCommentKey = "comment";

constexpr const char* const kBuyStr = "Buy";
constexpr const char* const kSellStr = "Sell";

std::string TypeToString(const Transaction::Type t) { return t == Transaction::Type::BUY ? kBuyStr : kSellStr; }
}  // namespace

Transaction::Tags::Tags(std::initializer_list<Tag> tags)
{
  for (const auto& tag : tags) Add(std::move(tag));
}

Transaction::Tags& Transaction::Tags::operator=(const json::Json& json)
{
  for (const auto& jstr : json)
  {
    auto vec = Split(jstr.get<std::string>());
    if (vec.size() == 2)
      Add(std::move(vec[0]), std::move(vec[1]));
    else if (vec.size() == 1)
      Add(std::move(vec[0]));
  }
  return *this;
}

Transaction::Tags::operator std::unordered_set<std::string>() const noexcept
{
  std::unordered_set<std::string> ts;
  ts.reserve(size());
  for (const auto& [key, value] : *this) ts.insert(value.has_value() ? key + "=" + value.value() : key);
  return ts;
}

Transaction::Tags::operator json::Json() const noexcept { return operator std::unordered_set<std::string>(); }

std::unordered_set<Transaction::Tag> Transaction::Tags::Keys() const
{
  std::unordered_set<Tag> set;
  for (const auto& [key, value] : *this) set.insert(key);
  return set;
}

Transaction Transaction::Factory(const ID id, const json::Json& input_json)
{
  Transaction tr(id);
  auto ec = tr.Deserialize(input_json);
  if (ec.Failure()) throw std::runtime_error(ErrorCode("Transaction::Factory() failed", std::move(ec)));

  return tr;
}

Transaction Transaction::Factory(Transaction::ID id, Date d, Symbol s, Transaction::Type t, Price p,
                                 Transaction::Quantity q, Price f, Transaction::Tags tags, Transaction::Comment c)
{
  Transaction tr(id);
  tr.date = d;
  tr.symbol = std::move(s);
  tr.type = t;
  tr.price = p;
  tr.quantity = q;
  tr.fee = f;
  tr.tags = std::move(tags);
  tr.comment = std::move(c);
  return tr;
}

void Transaction::ToTreeRow(Gtk::TreeRow& row) const
{
  row.set_value(Field::UNIQUE_ID, ToString(id));
  row.set_value(Field::DATE, date.ToString(Date::Format::MMDDYYYY));
  row.set_value(Field::SYMBOL, symbol.Get());
  row.set_value(Field::TYPE, TypeToString(type));
  row.set_value(Field::QUANTITY, ToString(quantity));
  row.set_value(Field::PRICE, ToString(price));
  row.set_value(Field::FEE, ToString(fee));
  std::unordered_set<std::string> tag_set = tags;
  row.set_value(Field::TAGS,
                Join(std::make_move_iterator(tag_set.begin()), std::make_move_iterator(tag_set.end()), ", "));
  row.set_value(Field::COMMENT, comment);
}

[[nodiscard]] ValueWithErrorCode<json::Json> Transaction::Serialize() const
{
  json::Json json;

  try
  {
    json[kJsonDateKey] = date.ToPrimitive();
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
    tags = input_json[kJsonTagsKey];
    input_json.find(kJsonCommentKey)->get_to(comment);
  }
  catch (const std::exception& e)
  {
    return ErrorCode("Transaction::Deserialize() failed", ErrorCode(e.what()));
  }

  return {};
}

bool Transaction::MemberwiseEquals(const Transaction& other) const
{
  return date == other.date && symbol == other.symbol && type == other.type && price == other.price &&
         quantity == other.quantity && fee == other.fee;
}

bool TransactionMemberwiseComparator::operator()(const Transaction::ID& left, const Transaction::ID& right) const
{
  return TransactionPool::Find(left)->MemberwiseEquals(*TransactionPool::Find(right));
}

std::size_t TransactionMemberwiseHasher::operator()(const Transaction::ID& id) const
{
  const auto& tr = *TransactionPool::Find(id);
  std::vector<std::string> v = {tr.date.ToString(), tr.symbol.Get(),       TypeToString(tr.type),
                                ToString(tr.price), ToString(tr.quantity), ToString(tr.fee)};
  return std::hash<std::string>()(Join(v.begin(), v.end(), "."));
}
}  // namespace inv::detail