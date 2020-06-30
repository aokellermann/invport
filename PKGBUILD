# Maintainer: Antony Kellermann <aokellermann@gmail.com>

_pkgname=__cpptemplate_name__
pkgname="${_pkgname}-git"
pkgver=0.0.1
pkgrel=1
pkgdesc="C++17 DFA simulator."
arch=("x86_64")
url="https://github.com/__cpptemplate_username__/${_pkgname}"
license=("MIT")
depends=()
optdepends=("doxygen: documentation")
makedepends=("cmake" "git")
provides=("${_pkgname}")
conflicts=("${_pkgname}")
source=("git://github.com/__cpptemplate_username__/${_pkgname}")
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
  install -Dm644 "${_pkgname}/LICENSE" "${pkgdir}/usr/share/licenses/${_pkgname}/LICENSE"
}