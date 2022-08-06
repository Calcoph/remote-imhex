## Building

mkdir -p build
cd build
CC=gcc-12 CXX=g++-12 cmake                    \
    -DCMAKE_BUILD_TYPE=Release                \
    -DCMAKE_INSTALL_PREFIX="/usr" 	          \
    -DCMAKE_C_COMPILER_LAUNCHER=ccache        \
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache      \
    -DCMAKE_C_FLAGS="-fuse-ld=lld"            \
    -DCMAKE_CXX_FLAGS="-fuse-ld=lld"          \
    -DCMAKE_OBJC_COMPILER_LAUNCHER=ccache     \
    -DCMAKE_OBJCXX_COMPILER_LAUNCHER=ccache   \
    -DRUST_PATH="$HOME/.cargo/bin/"           \
    ..
sudo make -j 4 install