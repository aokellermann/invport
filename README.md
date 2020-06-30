Everything below the following text is a template, not information about this template!

This is a C++ project template that gets all the annoying parts about starting a project
out of the way. It includes the following features:
* `CMakeLists.txt`
  * Shared library target
  * Executable target
  * Unit test target (Googletest)
  * Documentation target (Doxygen)
  * Compiler warnings
  * CMake config
* Linting/formatting
  * Formatting config (clang-format)
  * Linting config (clang-tidy)
  * Lint script
* Package script (Arch Linux `PKGBUILD`)
* MIT license
* `.gitignore` for common ignored directories
* Source/header file structure
  * Main executable
  * Library source/header
  * Test source
* CI config (CircleCI)
  * Linting
  * CMake build all targets
  * Run unit test

To use this template, follow these steps:
1. Click generate on GitHub.
2. Enter your project's name and description and click create.
3. Clone your new repository. 
4. Edit the variable declarations in `setup.sh` with information about your project
  * `author`: your name
  * `username`: your GitHub username
  * `name`: your library's name
  * `execname`: the executable's name
  * `description`: description of the project
5. Run `setup.sh`.
6. Remove `setup.sh` from git and delete this description of how to use the template.
7. Change the `LICENSE` if you want to use a different one (change the license in `PKGBUILD` as well).

# __cpptemplate_name__

### Description

### Install

#### Arch Linux Package
Arch Linux users may install with the included `PKGBUILD`:
```bash
mkdir build && cd build
wget https://raw.githubusercontent.com/__cpptemplate_username__/__cpptemplate_name__/master/PKGBUILD
makepkg -si
```

#### Manual Build

##### Dependencies
Building from source requires the installation of all necessary dependencies:
* [cmake](https://github.com/Kitware/CMake) (build only)
* [doxygen](https://github.com/doxygen/doxygen) (optional documentation)

##### Clone and Build
After dependencies are installed, run from a shell:
```bash
git clone https://github.com/__cpptemplate_username__/__cpptemplate_name__.git
mkdir __cpptemplate_name__/build && cd dfa/build
cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr ..
sudo make install
```
Substitute `/usr` with your desired install location.

### Contributing
See [contributing guidelines](.github/CONTRIBUTING.md).

### Usage
