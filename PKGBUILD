pkgname=openzone-git
pkgver=20091021
pkgrel=1
pkgdesc="A simple cross-platform 3D engine"
url="http://github.com/ducakar/openzone/"
arch=('i686' 'x86_64')
license=('GPL3')
depends=('lua' 'sdl' 'libpng' 'libjpeg' 'sdl_image' 'sdl_net' 'sdl_ttf'
  'libgl' 'openal' 'freealut' 'libvorbis')
makedepends=('git' 'cmake' 'mesa')
source=()
md5sums=()

_gitroot='git://github.com/ducakar/openzone.git'

build() {
  #cd ${srcdir}

  #msg "Connecting to GIT server...."
  #if [ ! -d openzone ]; then
  #  git clone ${_gitroot}
  #fi
  #cd openzone
  #git pull
  #msg2 "GIT checkout done or server timeout"
  #msg "Starting make..."

  #./autogen.sh
  #make || return 1
  #install -m 755 -D ${srcdir}/openzone/src/client/openzone ${pkgdir}/usr/bin
  cd ${pkgdir}/../linux
  make install DESTDIR=${pkgdir}
}
