strip ./WebKitBuild/Release/bin/netrider
cp ./WebKitBuild/Release/bin/netrider /usr/local/bin
cd ./WebKitBuild/Release
make install
cd ../..
strip /usr/local/lib/libWebKitNix.so.0.1.0
ldconfig /usr/local/lib