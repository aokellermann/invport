/**
 * @file cache_test.cc
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#include "invport/detail/cache.h"

#include <gtest/gtest.h>
#include <iex/api/company.h>
#include <iex/api/quote.h>
#include <iex/api/symbols.h>
#include <iex/api/system_status.h>
#include <iex/iex.h>

#include <filesystem>
#include <memory>

namespace fs = std::filesystem;

using Cache = inv::Cache;

const std::vector<const char*> kSymbols = {
    "aapl", "msft", "tsla", "intc", "amd", "bynd", "aig+", "brk.a", "ver-f", "mj", "qqq", "arkq", "robo",
};

fs::path GetPath() { return "/tmp/invport/cache.json"; }

void DeleteCache()
{
  const fs::path path = GetPath();
  if (fs::exists(path))
  {
    fs::remove(path);
  }
}

Cache CacheFactory() { return Cache(inv::file::Directory::TEMP); }

Cache PopulatedCacheFactory()
{
  Cache cache = CacheFactory();

  iex::json::JsonStorage storage{iex::json::Json::object()};

  iex::EndpointPtr<iex::Symbols> symbols_ptr = std::make_shared<iex::Symbols>(storage);
  iex::EndpointPtr<iex::SystemStatus> status_ptr = std::make_shared<iex::SystemStatus>(storage);

  cache.Set(symbols_ptr);
  cache.Set(status_ptr);

  for (const auto& sym : kSymbols)
  {
    iex::Symbol symbol(sym);

    iex::EndpointPtr<iex::Quote> quote_ptr = std::make_shared<iex::Quote>(storage, symbol);
    iex::EndpointPtr<iex::Company> company_ptr = std::make_shared<iex::Company>(storage, symbol);

    cache.Set(quote_ptr);
    cache.Set(company_ptr);
  }

  return cache;
}

TEST(Cache, EmptyGet)
{
  DeleteCache();

  Cache cache = CacheFactory();

  EXPECT_EQ(cache.Get<iex::Endpoint::Type::SYMBOLS>(), nullptr);
  EXPECT_EQ(cache.Get<iex::Endpoint::Type::SYSTEM_STATUS>(), nullptr);

  for (const auto& sym : kSymbols)
  {
    iex::Symbol symbol(sym);
    EXPECT_EQ(cache.Get<iex::Endpoint::Type::QUOTE>(symbol), nullptr);
    EXPECT_EQ(cache.Get<iex::Endpoint::Type::COMPANY>(symbol), nullptr);
  }
}

TEST(Cache, Set)
{
  DeleteCache();

  Cache cache = PopulatedCacheFactory();

  EXPECT_NE(cache.Get<iex::Endpoint::Type::SYMBOLS>(), nullptr);
  EXPECT_NE(cache.Get<iex::Endpoint::Type::SYSTEM_STATUS>(), nullptr);

  for (const auto& sym : kSymbols)
  {
    iex::Symbol symbol(sym);
    EXPECT_NE(cache.Get<iex::Endpoint::Type::QUOTE>(symbol), nullptr);
    EXPECT_NE(cache.Get<iex::Endpoint::Type::COMPANY>(symbol), nullptr);
  }
}

TEST(Cache, Flush)
{
  DeleteCache();

  Cache cache = PopulatedCacheFactory();

  EXPECT_EQ(cache.Flush(), iex::ErrorCode());
  EXPECT_TRUE(fs::exists(GetPath()));
}

TEST(Cache, Serialization)
{
  DeleteCache();

  {
    Cache cache = PopulatedCacheFactory();
  }

  Cache cache = CacheFactory();

  EXPECT_NE(cache.Get<iex::Endpoint::Type::SYMBOLS>(), nullptr);
  EXPECT_NE(cache.Get<iex::Endpoint::Type::SYSTEM_STATUS>(), nullptr);

  for (const auto& sym : kSymbols)
  {
    iex::Symbol symbol(sym);
    EXPECT_NE(cache.Get<iex::Endpoint::Type::QUOTE>(symbol), nullptr);
    EXPECT_NE(cache.Get<iex::Endpoint::Type::COMPANY>(symbol), nullptr);
  }
}
