/**
 * @file cache.h
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#pragma once

#include <iex/detail/json_serializer.h>
#include <iex/iex.h>

#include <unordered_map>

#include "invport/detail/common.h"
#include "invport/detail/file_serializable.h"

/**
 * Contains classes and methods for manipulating a cache of IEX endpoints.
 */
namespace inv::cache
{
/**
 * Contains methods for caching IEX endpoints. All data is kept in memory.
 */
class Cache : private file::FileIoBase, private json::JsonBidirectionalSerializable
{
 public:
  using Endpoint = iex::Endpoint;
  using SymbolEndpoint = iex::SymbolEndpoint;
  using Symbol = iex::Symbol;
  using EndpointType = iex::Endpoint::Type;

  template <typename E = Endpoint>
  using EndpointPtr = iex::EndpointPtr<E>;

  /**
   * Constructs a Cache in the specified directory.
   * @param directory
   */
  explicit Cache(file::Directory directory = file::Directory::HOME);

  /**
   * Writes the Cache's data to a file on destruction.
   */
  inline ~Cache() override { Flush(); }

  /**
   * Write's the Cache's data to a file.
   * @return whether the write was successful or not
   */
  ErrorCode Flush() const;

  /**
   * Gets an Endpoint from the Cache.
   * @tparam Type the EndpointType to get
   * @return valid pointer if exists, nullptr if not
   */
  template <EndpointType Type>
  EndpointPtr<iex::EndpointTypename<Type>> Get() const
  {
    const auto endpoint_map_iter = endpoint_index_.find(Type);
    if (endpoint_map_iter != endpoint_index_.end())
    {
      return std::dynamic_pointer_cast<iex::EndpointTypename<Type>>(endpoint_map_iter->second);
    }

    return nullptr;
  }

  /**
   * Gets a SymbolEndpoint from the Cache.
   * @tparam Type the EndpointType to get
   * @param symbol the symbol of the endpoint
   * @return valid pointer if exists, nullptr if not
   */
  template <EndpointType Type>
  EndpointPtr<iex::EndpointTypename<Type>> Get(const Symbol& symbol) const
  {
    const auto stock_index_iter = stock_index_.find(symbol);
    if (stock_index_iter != stock_index_.end())
    {
      const auto endpoint_map_iter = stock_index_iter->second.find(Type);
      if (endpoint_map_iter != stock_index_iter->second.end())
      {
        return std::dynamic_pointer_cast<iex::EndpointTypename<Type>>(endpoint_map_iter->second);
      }
    }

    return nullptr;
  }

  /**
   * Adds an Endpoint to the Cache.
   * @param endpoint_ptr the pointer to add
   */
  template <typename E>
  std::enable_if_t<std::negation_v<std::is_base_of<SymbolEndpoint, E>>, void> Set(EndpointPtr<E> endpoint_ptr)
  {
    endpoint_index_[endpoint_ptr->GetType()] = std::move(endpoint_ptr);
  }

  /**
   * Adds a SymbolEndpoint to the Cache.
   * @param endpoint_ptr the pointer to add
   */
  void Set(EndpointPtr<SymbolEndpoint> endpoint_ptr)
  {
    stock_index_[endpoint_ptr->symbol][endpoint_ptr->GetType()] = std::move(endpoint_ptr);
  }

 private:
  ValueWithErrorCode<json::Json> Serialize() const final;

  ErrorCode Deserialize(const json::Json& input_json) final;

  using EndpointMap = std::unordered_map<EndpointType, EndpointPtr<Endpoint>>;
  using SymbolEndpointMap = std::unordered_map<EndpointType, EndpointPtr<SymbolEndpoint>>;
  using SecurityIndex = iex::SymbolMap<SymbolEndpointMap>;

  EndpointMap endpoint_index_;
  SecurityIndex stock_index_;
};
}  // namespace inv::cache
