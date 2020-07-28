/**
 * @file transaction.h
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#pragma once

#include <forward_list>
#include <string>

#include "invport/detail/common.h"

namespace inv
{
/**
 * Represents a market transaction, such as the purchase of a stock.
 */
struct Transaction : json::JsonBidirectionalSerializable
{
  using ID = uint64_t;
  using Quantity = double;
  using Tag = std::string;
  using Tags = std::forward_list<Tag>;
  using Comment = std::string;

  enum Type
  {
    BUY,
    SELL
  };

  Transaction() = default;

  Transaction(ID id, Timestamp ts, Symbol s, Type t, Price p, Quantity q, Price f, Tags tags = {}, Comment c = {})
      : id(id),
        date(std::move(ts)),
        symbol(std::move(s)),
        type(t),
        price(p),
        quantity(q),
        fee(f),
        tags(std::move(tags)),
        comment(std::move(c))
  {
  }

  static Transaction Factory(const json::Json& input_json);

  static const char* TypeToString(const Type t);

  [[nodiscard]] ValueWithErrorCode<json::Json> Serialize() const override;

  ErrorCode Deserialize(const json::Json& input_json) override;

  bool operator==(const Transaction& other) const { return id == other.id; }
  bool operator!=(const Transaction& other) const { return !(*this == other); }
  bool operator<(const Transaction& other) { return date < other.date; }
  bool operator>(const Transaction& other) { return *this < other; }
  bool operator<=(const Transaction& other) { return !(*this > other); }
  bool operator>=(const Transaction& other) { return !(*this < other); }

  /**
   * Unique transaction id
   */
  ID id;
  /**
   * The date of the transaction
   */
  Timestamp date;
  /**
   * The transaction's associated symbol
   */
  Symbol symbol;
  /**
   * The kind of transaction
   */
  Type type;
  /**
   * The price per share
   */
  Price price;
  /**
   * The number of shares
   */
  Quantity quantity;
  /**
   * Fee of transaction
   */
  Price fee;
  /**
   * User-defined tags
   */
  Tags tags;
  /**
   * User-defined comment
   */
  Comment comment;
};

}  // namespace inv