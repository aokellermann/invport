/**
 * @file common.h
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#include <iex/detail/common.h>
#include <iex/detail/json_serializer.h>

#pragma once

namespace inv
{
using ErrorCode = iex::ErrorCode;

template <typename T>
using ValueWithErrorCode = iex::ValueWithErrorCode<T>;

using Timestamp = iex::Timestamp;

namespace json = iex::json;  // NOLINT For some reason it doesn't think this is being used.
}  // namespace inv
