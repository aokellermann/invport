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

namespace inv
{
class Cache : private file::FileIoBase, private json::JsonBidirectionalSerializable
{
 public:
  using Endpoint = iex::Endpoint;
  using SymbolEndpoint = iex::SymbolEndpoint;
  using Symbol = iex::Symbol;
  using EndpointType = iex::Endpoint::Type;

  template <typename E = Endpoint>
  using EndpointPtr = iex::EndpointPtr<E>;

  Cache();

  inline ~Cache() override { Flush(); }

  ErrorCode Flush() const;

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

  template <EndpointType Type>
  void Set(EndpointPtr<> endpoint_ptr, const Symbol& symbol)
  {
    stock_index_[symbol][Type] = std::move(endpoint_ptr);
  }

 private:
  ValueWithErrorCode<json::Json> Serialize() const final;

  ErrorCode Deserialize(const json::Json& input_json) final;

  using EndpointMap = std::unordered_map<EndpointType, EndpointPtr<>>;
  using SecurityIndex = iex::SymbolMap<EndpointMap>;

  SecurityIndex stock_index_;
};
}  // namespace inv
