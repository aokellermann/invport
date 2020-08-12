/**
 * @file transaction.h
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#pragma once

#include <gtkmm.h>

#include <optional>
#include <string>
#include <unordered_set>

#include "invport/detail/utils.h"

namespace inv
{
namespace detail
{
/**
 * Represents a market transaction, such as the purchase of a stock.
 *
 * Note: All constructors and factory methods here should only be called from TransactionPool, as that is how each
 * Transaction's unique identifier is assigned.
 */
struct Transaction : json::JsonBidirectionalSerializable
{
  using ID = uint64_t;
  using Quantity = double;
  using Tag = std::string;
  struct Tags : std::unordered_map<Tag, std::optional<std::string>>
  {
    Tags() = default;
    Tags(std::initializer_list<Tag> tags);
    Tags& operator=(const json::Json& json);

    operator std::unordered_set<std::string>() const noexcept;  // NOLINT
    operator json::Json() const noexcept;                       // NOLINT

    std::unordered_set<Tag> Keys() const;

    inline void Add(Tag tag) { emplace(std::move(tag), std::nullopt); }
    inline void Add(Tag tag, std::string value) { emplace(std::move(tag), std::move(value)); }
  };
  using Comment = std::string;

  enum Type
  {
    BUY,
    SELL
  };

  enum Field
  {
    UNIQUE_ID,
    DATE,
    SYMBOL,
    TYPE,
    PRICE,
    QUANTITY,
    FEE,
    TAGS,
    COMMENT,
    NUM_FIELDS
  };

  struct Totals
  {
    Totals() = default;
    Totals(const Transaction& tr, bool reset_on_sell_all)
        : spent(tr.type == Transaction::Type::BUY ? tr.price * tr.quantity : -tr.price * tr.quantity),
          quantity(tr.type == Transaction::Type::BUY ? tr.quantity : -tr.quantity),
          fees(tr.fee),
          reset_on_sell_all_(reset_on_sell_all)
    {
    }

    inline Totals& operator+=(const Totals& other)
    {
      spent += other.spent;
      quantity += other.quantity;
      fees += other.fees;

      if (other.reset_on_sell_all_ && (FloatingEqual(quantity, 0) || quantity < 0))
      {
        spent = 0;
        quantity = 0;
        fees = 0;
      }
      return *this;
    }

    Price spent;
    Transaction::Quantity quantity;
    Price fees;

    bool reset_on_sell_all_;
  };

  /**
   * Initializes an empty transaction with an ID number.
   */
  explicit Transaction(ID tr_id) : id(tr_id), date(0) {}

  /**
   * Transaction factory method to deserialize from JSON data.
   * @param id unique ID
   * @param input_json JSON data
   * @return new Transaction
   */
  static Transaction Factory(ID id, const json::Json& input_json);

  /**
   * Transaction factory method to populate members manually.
   * @param id unique ID
   * @return new Transaction
   */
  static Transaction Factory(ID id, Date d, Symbol s, Type t, Price p, Quantity q, Price f, Tags tags = {},
                             Comment c = {});

  void ToTreeRow(Gtk::TreeRow& row) const;

  [[nodiscard]] ValueWithErrorCode<json::Json> Serialize() const override;

  ErrorCode Deserialize(const json::Json& input_json) override;

  // Equality operators only check the id, so it is important that they are unique.
  bool operator==(const Transaction& other) const { return id == other.id; }
  bool operator!=(const Transaction& other) const { return !(*this == other); }

  [[nodiscard]] bool MemberwiseEquals(const Transaction& other) const;

  // Ordering operators correspond to the Transaction's date.
  bool operator<(const Transaction& other) const { return date < other.date; }
  bool operator>(const Transaction& other) const { return date > other.date; }
  bool operator<=(const Transaction& other) const { return !(*this > other); }
  bool operator>=(const Transaction& other) const { return !(*this < other); }

  /**
   * Unique transaction identifier
   */
  const ID id;
  /**
   * The date of the transaction
   */
  Date date;
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

struct TransactionMemberwiseComparator
{
  bool operator()(const Transaction::ID& left, const Transaction::ID& right) const;
};

struct TransactionMemberwiseHasher
{
  std::size_t operator()(const Transaction::ID& id) const;
};
}  // namespace detail

/**
 * Static class for managing Transactions.
 */
class TransactionPool
{
 public:
  using Transaction = detail::Transaction;
  using TransactionID = Transaction::ID;

  TransactionPool() = delete;
  TransactionPool(const TransactionPool&) = delete;

  /**
   * Creates a Transaction, passing the args to the Transactions's factory method.
   */
  template <typename... Args>
  static const Transaction& TransactionFactory(Args&&... args)
  {
    Transaction tr = Transaction::Factory(next_id_++, std::forward<Args>(args)...);
    const auto [iter, inserted] = transaction_id_map_.emplace(tr.id, std::move(tr));
    return iter->second;
  }

  /**
   * Returns a pointer to the Transaction with the given ID.
   * @param id the ID to find
   * @return valid pointer if found, nullptr if not
   */
  static Transaction* Find(TransactionID id)
  {
    const auto iter = transaction_id_map_.find(id);
    return iter != transaction_id_map_.end() ? &iter->second : nullptr;
  }

 private:
  inline static std::unordered_map<TransactionID, Transaction> transaction_id_map_;
  inline static TransactionID next_id_ = 0;
};

}  // namespace inv