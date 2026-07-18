export CFLAGS="-Wall -Wextra -Wpedantic -g `pkg-config --cflags gobject-2.0`"
export LIBS="`pkg-config --libs gobject-2.0`"
