### Code Style
C++ code in this repository follows [Google C++ Style](https://google.github.io/styleguide/cppguide.html) fairly closely. Divergences and further specifications are laid out below:
* Maximum line length is 120 characters.
* Use `#pragma once` instead of header guards.
* Braces go on a new line, unless the entire expression can fit on one line.
* File names never use dashes.
* Doxygen is used for documentation.
* Exceptions may be thrown.
When in doubt, follow conventions in preexisting code.

You code's conformity to this repo's style can be checked with `style.sh`. Any nonconformities will be printed to output. Your code must be built before running this script due to the way that `clang-tidy` works. Due to the usage of GoogleTest, 100,000+ suppressed warnings will be generated, but you don't have to worry about this. Dependencies for this script are:
* clang-tidy 10.0.0+
* clang-format 10.0.0+

### Building
Ensure all dependencies listed [here](../README.md#Dependencies) are installed.

The project's `CMakeLists.txt` supports the following options:
* `ENABLE_CXX_WARNINGS`: Turn on GCC/Clang compatible compiler warnings. Note: warnings will cause build to fail due to addition of `Werror`.
* `BUILD_LIBRARY`: Builds library target.
* `BUILD_EXECUTABLE`: Builds executable target.
* `BUILD_TESTING`: Build unit test target.
* `BUILD_DOCUMENTATION`: Build documentation using `doxygen`.

To perform a weed whack build, run:
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=ON -DBUILD_EXECUTABLE:BOOL=ON -DBUILD_LIBRARY:BOOL=ON -DBUILD_DOCUMENTATION:BOOL=ON -DBUILD_TESTING:BOOL=ON -DENABLE_CXX_WARNINGS:BOOL=ON ..
cd ..
cmake --build build/
```
After, you can run unit tests:
```bash
././build/__cpptemplate_name__/test/unit_test
```