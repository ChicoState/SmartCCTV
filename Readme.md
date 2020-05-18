
# Smart CCTV Team Project <br> [![Build Status](https://travis-ci.org/ChicoState/SmartCCTV.svg?branch=master)](https://travis-ci.org/ChicoState/SmartCCTV.svg?branch=master)  [![Coverage Status](https://coveralls.io/repos/github/ChicoState/SmartCCTV/badge.svg?branch=master)](https://coveralls.io/github/ChicoState/SmartCCTV?branch=master)
</br>

## Team Product Description: 
Our product will be a replacement for traditional CCTV software that uses image comparison to help save space and money for a company. It will be a smart move for any company that would be interested in potentially using the software for analytics, as it will label objects on the images it receives and log each event with the objects identified, potentially saving the consumer thousands.
<br><img src="https://i.imgur.com/YaPYtXW.png"
     alt="Product Demonstration of human tracking"
     width="450" height="350" /> <img src="https://i.imgur.com/E63OZOZ.png"
     alt="Product Demonstration of UI and live viewer"
     width="450" height="350" />

## Installing ##
### Prerequisites

OpenCV (Version 3.4.0)
```
epel-release git gcc gcc-c++ cmake3 qt5-qtbase-devel python 
python-devel python-pip cmake python-devel 
python34-numpy gtk2-devel libpng-devel jasper-devel openexr-devel 
libwebp-devel libjpeg-turbo-devel libtiff-devel libdc1394-devel tbb-devel 
numpy eigen3-devel gstreamer-plugins-base-devel freeglut-devel mesa-libGL 
mesa-libGL-devel boost boost-thread boost-devel libv4l-devel

Default installation instructions:
https://www.learnopencv.com/install-opencv3-on-ubuntu/

Test your installation by compiling and running an OpenCV sample:
cd /path/to/opencv/samples/cpp/
pkg-config --cflags --libs opencv
g++ -ggdb facedetect.cpp -o facedetect `pkg-config --cflags --libs opencv`
./facedetect

You may need to fix a linking error when running the OpenCVcode:
sudo sh -c 'echo "/usr/local/lib" >> /etc/ld.so.conf.d/opencv.conf' 
sudo ldconfig
``` 

QT (Version 5.14.1)
```
Based on Qt 5.14.1 (GCC 5.3.1 20160406 (Red Hat 5.3.1-6), 64 bit)
https://download.qt.io/official_releases/qt/5.14/5.14.1/
qt5-qmake
libqt5-dev
```

SDL2
```
libsdl2-2.0-0
libsdl2-dbg
libsdl2-dev
libsdl2-image-2.0-0
libsdl2-image-dbg
libsdl2-image-dev
```

R
```
blas (openblas-lapack-git, openblas-git, blas-tmg, flexiblas, blas-git, openblas-lapack-static, atlas-lapack, openblas-lapack, openblas-cblas-git, openblas)
bzip2 (bzip2-git, bzip2-rustify-git, bzip2-with-lbzip2-symlinks)
desktop-file-utils (desktop-file-utils-git)
gcc-libs (gcc-libs-multilib-x32, fastgcc, gcc-libs-git, gcc-libs-multilib-git)
lapack (openblas-lapack-git, lapack-tmg, flexiblas, lapack-git, openblas-lapack-static, atlas-lapack, openblas-lapack)
libjpeg (libjpeg-droppatch, mozjpeg-git, mozjpeg, libjpeg-turbo)
libpng (libpng-git)
libtiff (libtiff-git)
libxmu
libxt
ncurses (ncurses-git, ncurses-nohex)
pango (pango-ubuntu, pango-git)
pcre (pcre-svn)
perl (perl-git)
readline (readline-athame-git)
unzip (unzip-natspec, unzip-iconv)
xz (xz-git, xz-static-git)
zip (zip-natspec)
zlib (zlib-static, zlib-git, zlib-asm, zlib-ng-git)
gcc-fortran (gcc-fortran-multilib-x32, gcc-fortran-git, gcc-fortran-multilib-git) (make)
tk (tk85) (make)
```

`remove` command (used in `bootstrap.sh`)
```
mysql-server-5.7
```

### Getting started

#### Installing the application from souce code:

There are two steps, first to initalize the Makefile and then use the bootstrap script</br>
to swap out the nessecary lines into the Makefile.

```
./scripts/bootstrap.sh
make
```

