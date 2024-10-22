/**
 * @file file_serializable.cc
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#include "invport/detail/file_serializable.h"

#include <fstream>
#include <stdexcept>
#include <string>

#include "invport/detail/env.h"

namespace inv::file
{
namespace
{
// region Reading and Writing

/**
 * Creates a directory with the given path. Does nothing if a directory already exists at the location.
 * @param path the target path to create a directory at
 * @return ErrorCode denoting success or failure
 */
ErrorCode CreateDirectory(const Path &path)
{
  if (fs::is_directory(path))
  {
    return {};
  }

  if (fs::exists(path))
  {
    return ErrorCode("Path is a file", {"path", ErrorCode(path.string())});
  }

  bool success;
  try
  {
    success = fs::create_directory(path);
  }
  catch (const std::exception &e)
  {
    return ErrorCode("fs::create_directory failed",
                     {{"path", ErrorCode(path.string())}, {"error", ErrorCode(e.what())}});
  }

  if (!success)
  {
    return ErrorCode("fs::create_directory failed", {"path", ErrorCode(path.string())});
  }

  return {};
}

/**
 * Opens the given fstream with the given openmode.
 * @param path the path to which stream is associated
 * @param stream the fstream to open
 * @param om the openmode to use on stream
 * @return ErrorCode denoting success or failure
 */
ErrorCode OpenFileStream(const Path &path, std::fstream &stream, std::ios_base::openmode om)
{
  try
  {
    stream.open(path, om);
  }
  catch (const std::exception &e)
  {
    return ErrorCode("fstream::open failed",
                     {{"path", ErrorCode(path.string())},
                      {"error", ErrorCode(e.what())},
                      {"openmode", ErrorCode(std::to_string(static_cast<int64_t>(om)))}});
  }

  if (!stream)
  {
    return ErrorCode(
        "fstream::open failed",
        {{"path", ErrorCode(path.string())}, {"openmode", ErrorCode(std::to_string(static_cast<int64_t>(om)))}});
  }

  return {};
}

/**
 * Writes contents to the given ostream.
 * @param path the path associated with stream
 * @param contents the contents to write to stream
 * @param stream the ostream to write to
 * @return ErrorCode denoting success or failure
 */
ErrorCode WriteStream(const Path &path, const std::string &contents, std::ostream &stream)
{
  try
  {
    stream.write(contents.c_str(), contents.size());
  }
  catch (const std::exception &e)
  {
    return ErrorCode("ostream::write failed", {{"path", ErrorCode(path.string())}, {"error", ErrorCode(e.what())}});
  }

  if (!stream)
  {
    return ErrorCode("ostream::write failed", {{"path", ErrorCode(path.string())}});
  }

  return {};
}

/**
 * Reads all data from the given istream.
 * @param path the path associated with stream
 * @param stream the istream to read from
 * @return file data if success and ErrorCode denoting success or failure
 */
ValueWithErrorCode<std::string> ReadStream(const std::filesystem::path &path, std::istream &stream)
{
  if (!fs::exists(path))
  {
    return {};
  }

  if (fs::is_directory(path))
  {
    return {{},
            ErrorCode("ReadStream failed",
                      {{"error", ErrorCode("path is a directory")}, {"path", ErrorCode(path.string())}})};
  }

  const auto size = fs::file_size(path);
  std::string data(size, '\0');
  try
  {
    stream.read(data.data(), size);
  }
  catch (const std::exception &e)
  {
    return {{},
            ErrorCode("istream::read failed", {{"path", ErrorCode(path.string())}, {"error", ErrorCode(e.what())}})};
  }

  if (!stream)
  {
    return {{}, ErrorCode("istream::read failed", {{"path", ErrorCode(path.string())}})};
  }

  return {data, {}};
}

/**
 * Writes contents to the given path.
 * @param path the path to write to
 * @param contents the data to write to path
 * @return ErrorCode denoting success or failure
 */
ErrorCode WriteFile(const Path &path, const std::string &contents)
{
  std::fstream out;
  OpenFileStream(path, out, std::ios_base::out);
  return WriteStream(path, contents, out);
}

/**
 * Reads all data from the given path.
 * @param path the path to read from
 * @return file data if success and ErrorCode denoting success or failure
 */
ValueWithErrorCode<std::string> ReadFile(const Path &path)
{
  std::fstream in;
  OpenFileStream(path, in, std::ios_base::in);
  return ReadStream(path, in);
}

// endregion Reading and Writing

}  // namespace

FileIoBase::FileIoBase(const Path &relative_path, const Directory directory, const Extension extension)
    : directory_path_(GetDirectoryPath(directory)),
      full_path_((directory_path_ / relative_path).string() + GetExtensionString(extension))
{
  if (ec_.Success())
  {
    ec_ = CreateDirectory(directory_path_);
  }
}

Path FileIoBase::GetDirectoryPath(Directory directory)
{
  switch (directory)
  {
    case Directory::HOME:
    {
      const auto res = env::GetEnv("HOME");
      ec_ = res.second;
      return res.first / Path(".invport");
    }

    case Directory::TEMP:
    {
      return "/tmp/invport";
    }

    default:
      ec_ = ErrorCode("Invalid Directory");
      return Path();
  }
}

std::string FileIoBase::GetExtensionString(Extension extension)
{
  switch (extension)
  {
    case Extension::TEXT:
    {
      return ".txt";
    }
    case Extension::JSON:
    {
      return ".json";
    }
    default:
    {
      ec_ = ErrorCode("Invalid Extension");
      return "";
    }
  }
}

ErrorCode FileIoBase::WriteFile(const std::string &contents) const
{
  return ::inv::file::WriteFile(full_path_, contents);
}

ValueWithErrorCode<std::string> FileIoBase::ReadFile() const { return ::inv::file::ReadFile(full_path_); }

}  // namespace inv::file
