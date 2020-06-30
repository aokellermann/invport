# invport

### Description

### Install

#### Arch Linux Package
Arch Linux users may install with the included `PKGBUILD`:
```bash
mkdir build && cd build
wget https://raw.githubusercontent.com/aokellermann/invport/master/PKGBUILD
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
git clone https://github.com/aokellermann/invport.git
mkdir invport/build && cd dfa/build
cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr ..
sudo make install
```
Substitute `/usr` with your desired install location.

### Contributing
See [contributing guidelines](.github/CONTRIBUTING.md).

### Usage
