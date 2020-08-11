/**
 * @file vanguard.h
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#pragma once

#include <filesystem>

#include "invport/detail/transaction_history.h"

namespace inv::vanguard
{
namespace fs = std::filesystem;

TransactionHistory Parse(const fs::path& path);
}
