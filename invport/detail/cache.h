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
  using EndpointMap = std::unordered_map<iex::Endpoint::Type, iex::EndpointPtr<>>;
  using SecurityIndex = iex::SymbolMap<EndpointMap>;

 public:
  using Endpoint = iex::Endpoint;
  using SymbolEndpoint = iex::SymbolEndpoint;
  using Symbol = iex::Symbol;

  template <typename E = Endpoint>
  using EndpointPtr = iex::EndpointPtr<E>;

  Cache();

  inline ~Cache() override { Flush(); }

  ErrorCode Flush() const;

 private:
  ValueWithErrorCode<json::Json> Serialize() const final;

  ErrorCode Deserialize(const json::Json& input_json) final;

  SecurityIndex stock_index_;
};
}  // namespace inv
