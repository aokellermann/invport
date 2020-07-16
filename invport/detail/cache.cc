/**
 * @file cache.cc
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#include "cache.h"

#include <iex/api/company.h>
#include <iex/api/quote.h>
#include <iex/api/symbols.h>
#include <iex/api/system_status.h>
#include <iex/iex.h>

#include <memory>
#include <type_traits>

namespace inv
{
namespace
{
using Endpoint = Cache::Endpoint;
using SymbolEndpoint = Cache::SymbolEndpoint;
using Symbol = Cache::Symbol;

template <typename E = Endpoint>
using EndpointPtr = Cache::EndpointPtr<E>;

constexpr const char* const kIndexNames[] = {"stock_index"};

template <typename E = Endpoint>
ValueWithErrorCode<EndpointPtr<>> EndpointFactory(const json::Json& input_json)
{
  static_assert(std::is_base_of<Endpoint, E>::value, "E must derive from Endpoint");

  try
  {
    return {std::make_shared<const E>(json::JsonStorage{input_json}), {}};
  }
  catch (const std::exception& e)
  {
    return {nullptr, ErrorCode("Endpoint::Deserialize() failed", {"exception", ErrorCode(e.what())})};
  }
}

template <typename E = SymbolEndpoint>
ValueWithErrorCode<EndpointPtr<SymbolEndpoint>> EndpointFactory(const json::Json& input_json, Symbol symbol)
{
  static_assert(std::is_base_of<SymbolEndpoint, E>::value, "E must derive from SymbolEndpoint");

  try
  {
    return {std::make_shared<const E>(json::JsonStorage{input_json}, std::move(symbol)), {}};
  }
  catch (const std::exception& e)
  {
    return {nullptr, ErrorCode("SymbolEndpoint::Deserialize() failed", {"exception", ErrorCode(e.what())})};
  }
}
}  // namespace

inv::Cache::Cache(const file::Directory directory)
    : file::FileIoBase("cache", directory), json::JsonBidirectionalSerializable()
{
  auto vec = ReadFile();
  if (vec.second.Success() && !vec.first.empty())
  {
    vec.second = Deserialize(json::Json::parse(vec.first));
  }

  if (vec.second.Failure())
  {
    throw std::runtime_error(ErrorCode("Cache initialization failed", std::move(vec.second)));
  }
}

ErrorCode Cache::Flush() const
{
  auto vec = Serialize();
  if (vec.second.Success())
  {
    vec.second = WriteFile(vec.first.dump());
  }

  return vec.second;
}

ValueWithErrorCode<json::Json> Cache::Serialize() const
{
  ErrorCode ec;

  try
  {
    json::Json json;

    std::size_t i = 0;
    for (const auto& index : {stock_index_})
    {
      for (const auto& [symbol, endpoint_map] : index)
      {
        for (const auto& [type, endpoint] : endpoint_map)
        {
          auto serialize_vec = endpoint->Serialize();
          if (serialize_vec.second.Success())
          {
            json[kIndexNames[i]][symbol.Get()][std::to_string(static_cast<int>(type))] = std::move(serialize_vec.first);
          }
          else
          {
            ec = std::move(serialize_vec.second);
          }
        }
      }

      ++i;
    }

    std::string jstr = json.dump();
    return {std::move(json), std::move(ec)};
  }
  catch (const std::exception& e)
  {
    return {{}, ErrorCode("Cache::Serialize() failed", ErrorCode(e.what()))};
  }
}

ErrorCode Cache::Deserialize(const json::Json& input_json)
{
  ErrorCode ec;

  std::string jstr = input_json.dump();

  try
  {
    std::size_t i = 0;
    for (auto& index : {&stock_index_})
    {
      auto json_index_reference = input_json.find(kIndexNames[i]);
      if (json_index_reference != input_json.end())
      {
        for (auto& json_symbol : json_index_reference->items())
        {
          const Symbol symbol(json_symbol.key());
          for (const auto& json_endpoint : json_symbol.value().items())
          {
            ValueWithErrorCode<EndpointPtr<SymbolEndpoint>> vec;
            const auto type = static_cast<Endpoint::Type>(std::stoi(json_endpoint.key()));
            switch (type)
            {
              case Endpoint::Type::QUOTE:
                vec = EndpointFactory<iex::EndpointTypename<Endpoint::Type::QUOTE>>(json_endpoint.value(), symbol);
                break;
              case Endpoint::Type::COMPANY:
                vec = EndpointFactory<iex::EndpointTypename<Endpoint::Type::COMPANY>>(json_endpoint.value(), symbol);
                break;

              default:
                break;
            }
            if (vec.second.Success())
            {
              (*index)[symbol][type] = std::move(vec.first);
            }
            else
            {
              ec = std::move(vec.second);
            }
          }
        }
      }
      ++i;
    }

    return ec;
  }
  catch (const std::exception& e)
  {
    return ErrorCode("Cache::Deserialize() failed", ErrorCode(e.what()));
  }
}
}  // namespace inv