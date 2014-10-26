clear
rm ./tuto.out
g++ src/*.cpp -o tuto.out -Iinclude -L. -ltcod -ltcodxx -Wl,-rpath=. -Wall 2>&1 >/dev/null |less
