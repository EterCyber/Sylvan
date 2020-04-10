FROM ubuntu:trusty

RUN rm /bin/sh && ln -s /bin/bash /bin/sh
RUN apt-get update && apt-get install -y software-properties-common python-software-properties && add-apt-repository ppa:beineri/opt-qt571-trusty
RUN apt-get update && apt-get install -y git qt57base qt57svg build-essential mesa-common-dev libglu1-mesa-dev && rm -rf /var/lib/apt/lists/*

ENV QT_BASE_DIR=/opt/qt57
ENV QTDIR=$QT_BASE_DIR
ENV PATH=$QT_BASE_DIR/bin:$PATH
ENV LD_LIBRARY_PATH=$QT_BASE_DIR/lib/x86_64-linux-gnu:$QT_BASE_DIR/lib:$LD_LIBRARY_PATH
ENV PKG_CONFIG_PATH=$QT_BASE_DIR/lib/pkgconfig:$PKG_CONFIG_PATH

# Install dependencies and build tools
RUN \
  apt-get update && apt-get install -y \
    ruby-dev \
    rpm \
    wget \
    bsdmainutils \
    zlib1g-dev && \
  gem install fpm && \
  wget http://mdocml.bsd.lv/snapshots/mandoc.tar.gz && \
  tar fzx mandoc.tar.gz && \
  cd mandoc-* && \
  ./configure && make && make install

# Download and build sylvan
RUN \
  cd / && \
  git clone https://github.com/EterCyber/Sylvan.git && \
  cd /sylvan && \
  qmake && \
  make -j4 && \
  make doc-txt && \
  make doc-html

# Create .deb and .rpm packages
RUN \
  mkdir -p /sylvan_pkg && \
  cd /sylvan_pkg && \
  mkdir -p usr/games && \
  mkdir -p usr/share/menu && \
  mkdir -p usr/share/pixmaps && \
  mkdir -p usr/share/applications && \
  mkdir -p usr/share/man/man6 && \
  mkdir -p usr/share/man/man5 && \
  mkdir -p usr/share/doc/sylvan && \
  cp /sylvan/projects/cli/sylvan-cli usr/games && \
  cp /sylvan/projects/gui/sylvan usr/games && \
  cp /sylvan/dist/linux/menu/sylvan usr/share/menu && \
  cp /sylvan/projects/gui/res/icons/sylvan_128x128.png usr/share/pixmaps/sylvan.png && \
  cp /sylvan/projects/gui/res/icons/sylvan_32x32.xpm usr/share/pixmaps/sylvan.xpm && \
  cp /sylvan/dist/linux/sylvan.desktop usr/share/applications && \
  cp /sylvan/docs/sylvan-cli.6 usr/share/man/man6 && \
  gzip usr/share/man/man6/sylvan-cli.6 && \
  cp /sylvan/docs/engines.json.5 usr/share/man/man5 && \
  gzip usr/share/man/man5/engines.json.5 && \
  cp /sylvan/COPYING usr/share/doc/sylvan/copyright && \
  cp /sylvan/README.md usr/share/doc/sylvan/README && \
  mkdir /finished_pkg && \
  export SYLVAN_CLI_VERSION=$(grep "^SYLVAN_CLI_VERSION" /sylvan/projects/cli/cli.pro | awk '{print $NF}') && \
  export SYLVAN_GUI_VERSION=$(grep "^SYLVAN_VERSION" /sylvan/projects/gui/gui.pro | awk '{print $NF}') && \
  export TODAY=$(date +%Y%m%d) && \
  fpm -s dir -t deb -C /sylvan_pkg \
    -a "amd64" \
    --license "GPLv3" \
    --url "https://github.com/EterCyber/Sylvan" \
    -n "sylvan" \
    -v "$TODAY+$SYLVAN_CLI_VERSION+$SYLVAN_GUI_VERSION" \
    --iteration 1 \
    --category "games" \
    -m "Ilari Pihlajisto <ilari.pihlajisto@mbnet.fi>" \
    --description "Commandline and graphical interface for playing chess" \
    -d "libc6 (>= 2.19)" \
    -d "libgcc1 (>= 1:4.9)" \
    -d "libqt5svg5 (>= 5.7.0)" \
    -d "libqt5core5a (>= 5.7.0)" \
    -d "libqt5gui5 (>= 5.7.0)" \
    -d "libqt5widgets5 (>= 5.7.0)" \
    -d "libqt5printsupport5 (>= 5.7.0)" \
    -d "libqt5concurrent5 (>= 5.7.0)" \
    -d "libstdc++6 (>= 4.8.4)" && \
  mv /sylvan_pkg/*.deb /finished_pkg/ && \
  fpm -s dir -t rpm -C /sylvan_pkg \
    -a "x86_64" \
    --license "GPLv3" \
    --url "https://github.com/EterCyber/Sylvan" \
    -n "sylvan" \
    -v "$TODAY+$SYLVAN_CLI_VERSION+$SYLVAN_GUI_VERSION" \
    --iteration 1 \
    --category "Amusements/Games/Board/Chess" \
    -m "Ilari Pihlajisto <ilari.pihlajisto@mbnet.fi>" \
    --description "Commandline and graphical interface for playing chess" \
    -d "qt5-qtbase >= 5.7.0" \
    -d "qt5-qtsvg >= 5.7.0" && \
  mv /sylvan_pkg/*.rpm /finished_pkg/

# Create .tar.gz package for sylvan-cli
RUN \
  cd /sylvan && \
  mkdir -p /sylvan_pkg/sylvan-cli && \
  cd /sylvan_pkg && \
  mkdir -p ./sylvan-cli/lib && \
  cp $QT_BASE_DIR/lib/libQt5Core.so.5 sylvan-cli/lib/ && \
  cp /sylvan/projects/cli/sylvan-cli sylvan-cli/ && \
  cp /sylvan/COPYING sylvan-cli/ && \
  cp /sylvan/docs/man-style.css sylvan-cli/ && \
  cp /sylvan/tools/clop-sylvan-cli.py sylvan-cli/ && \
  cp /sylvan/dist/linux/sylvan-cli.sh sylvan-cli/ && \
  cp /sylvan/docs/sylvan-cli.6.html sylvan-cli/ && \
  cp /sylvan/docs/sylvan-cli.6.txt sylvan-cli/ && \
  cp /sylvan/docs/engines.json.5.html sylvan-cli/ && \
  cp /sylvan/docs/engines.json.5.txt sylvan-cli/ && \
  tar -zcvf sylvan-cli-linux64.tar.gz sylvan-cli && \
  export SYLVAN_CLI_VERSION=$(grep "^SYLVAN_CLI_VERSION" /sylvan/projects/cli/cli.pro | awk '{print $NF}') && \
  mv sylvan-cli-linux64.tar.gz /finished_pkg/sylvan-cli-$SYLVAN_CLI_VERSION-linux64.tar.gz

# Copy the .deb package to the host
CMD cp /finished_pkg/sylvan*.* /package

