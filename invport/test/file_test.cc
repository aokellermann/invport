/**
 * @file file_test.cc
 * @author Antony Kellermann
 * @copyright 2020 Antony Kellermann
 */

#include <gtest/gtest.h>

#include <ctime>
#include <string>

#include "invport/detail/common.h"
#include "invport/detail/file_serializable.h"

namespace file = inv::file;

struct FileImpl : file::FileIoBase
{
  FileImpl(const std::string& file_name, file::Directory directory, file::Extension extension)
      : file::FileIoBase(file_name, directory, extension)
  {
  }

  [[nodiscard]] inv::ErrorCode Write(const std::string& contents) const { return FileIoBase::WriteFile(contents); }
  [[nodiscard]] inv::ValueWithErrorCode<std::string> Read() const { return FileIoBase::ReadFile(); }
  [[nodiscard]] inv::ErrorCode Valid() const { return FileIoBase::Validity(); }
};

TEST(File, ReadWrite)
{
  std::string file_name = std::to_string(std::time(nullptr)) + "test";
  FileImpl impl(file_name, file::Directory::TEMP, file::Extension::TEXT);
  EXPECT_EQ(impl.Valid(), inv::ErrorCode());

  std::string test_text("Testing text:\nTesting");
  const auto write_ec = impl.Write(test_text);
  EXPECT_EQ(write_ec, inv::ErrorCode());

  const auto read_response = impl.Read();
  EXPECT_EQ(read_response.first, test_text);
  EXPECT_EQ(read_response.second, inv::ErrorCode());
}
