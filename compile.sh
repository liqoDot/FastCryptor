read -p "Enter which version to compile (v1/v2/v3/v4): " version
mkdir -p build/

echo "~~ FC-$version Compiler ~~"
if test -f "src/$version/linux/encrypt.cpp" && test -f "src/$version/linux/decrypt.cpp"; then
    g++ src/$version/linux/encrypt.cpp -o build/FC4 -std=c++11
    g++ src/$version/linux/decrypt.cpp -o build/FC4_DECRYPTOR -std=c++11
    echo "Finished compiling!"
else
    echo "Error: File not found! Please download the source code."
fi