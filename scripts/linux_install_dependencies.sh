#!/bin/bash

sudo apt update

# collect all the package names in an array
PKGS=(
  # — Build & SCM tools
  build-essential
  autoconf autoconf-archive automake libtool pkg-config
  cmake git unzip ninja-build curl bison flex

  # — Security & crypto
  libssl-dev

  # — Compression & archiving
  zlib1g-dev libbz2-dev liblzma-dev xz-utils

  # — Scripting & data backends
  libreadline-dev    # readline for interactive shells
  libsqlite3-dev     # embedded SQL engine
  libncursesw5-dev   # terminal UI library
  libffi-dev         # foreign-function interface
  libgdbm-dev        # GNU dbm (key/value store)
  tk-dev             # Tk GUI support

  # — Miscellaneous utilities
  uuid-dev           # unique IDs
  wget               # file downloader
  libgomp1           # OpenMP runtime

  # — OpenGL & Mesa
  libgl1-mesa-dev    # core OpenGL dev files
  libglu1-mesa-dev   # GLU utility library

  # — X11 core
  libx11-dev         # core X11 client library
  libxrandr-dev      # Resize & rotate extension
  libxi-dev          # Input extension
  libxmu-dev         # X miscellaneous utilities
  libxxf86vm-dev     # video mode extension

  # — XCB & XKB (Qt’s low-level X11 stack)
  '^libxcb.*-dev'    # all libxcb-*-dev packages
  libx11-xcb-dev     # Xlib/XCB bridge
  libxrender-dev     # rendering extension
  libxkbcommon-dev   # keymap library
  libxkbcommon-x11-dev # X11 bindings

  # — Explicit individual XCB pieces
  libxcb1-dev libxext-dev libxcb-glx0-dev
  libxcb-keysyms1-dev libxcb-image0-dev libxcb-shm0-dev
  libxcb-icccm4-dev libxcb-sync-dev libxcb-xfixes0-dev
  libxcb-shape0-dev libxcb-randr0-dev
  libxcb-render-util0-dev libxcb-xinerama0-dev
  libxcb-xkb-dev

  # — Font & text rendering
  libfontconfig1-dev # font discovery/configuration
  libfreetype6-dev   # TrueType/OpenType engine

  # — IPC & service bus
  libdbus-1-dev      # D-Bus message bus library
)

# now install them all in one shot
sudo apt install -y "${PKGS[@]}"
