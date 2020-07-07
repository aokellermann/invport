/**
 * @file main.cc
 * @author author
 * @copyright 2020 author
 */

#include <spdlog/spdlog.h>

int main(int /* argc */, char** /* argv */)
{
  spdlog::set_pattern("%Y-%m-%d %H:%M:%S.%e %l : %v");
  spdlog::info("Starting invport.");
  return 0;
}
