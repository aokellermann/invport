# Maintainer: Antony Kellermann <aokellermann@gmail.com>

_pkgname=invport
pkgname="${_pkgname}-git"
pkgver=0.0.1
pkgrel=1
pkgdesc="C++ investments tracker."
arch=("x86_64")
url="https://github.com/aokellermann/${_pkgname}"
license=("MIT")
depends=("iex" "spdlog")
optdepends=("doxygen: documentation")
makedepends=("cmake" "git")
provides=("${_pkgname}")
conflicts=("${_pkgname}")
source=("git://github.com/aokellermann/${_pkgname}")
md5sums=("SKIP")

prepare() {
  mkdir -p "${_pkgname}/build"
}

build() {
  cd "${_pkgname}/build" || exit 1
  cmake -DCMAKE_INSTALL_PREFIX=/usr ..
  cmake --build .
}

package() {
  cmake --build "${_pkgname}/build" --target install -- DESTDIR="${pkgdir}"
}