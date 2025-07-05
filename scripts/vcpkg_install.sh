export VCPKG_BUILD_TYPE=release

# Determine host OS and pick the vcpkg triplet
case "$(uname -s)" in
  Darwin)
    TRIPLET="x64-osx"
    ;;
  Linux)
    TRIPLET="x64-linux"
    ;;
  *)
    echo "Unsupported OS: $(uname -s)" >&2
    exit 1
    ;;
esac

vcpkg install --classic \
  boost-asio \
  boost-crc \
  boost-filesystem \
  boost-interprocess \
  boost-optional \
  boost-process \
  boost-property-tree \
  boost-python \
  boost-signals2 \
  boost-system \
  boost-thread \
  boost-chrono \
  brotli \
  bzip2 \
  expat \
  freeglut \
  gtest \
  libffi \
  liblzma \
  openssl \
  python3 \
  sqlite3 \
  zlib \
  zstd \
  cpprestsdk \
  --triplet "${TRIPLET}"

# Clear cached files and intermediate results
./scripts/vcpkg_clean_intermediate.sh