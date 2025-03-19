# qt-soa-testing
Simple utility for testing SOA.

# Requirements
 - Qt 6.8+
 - CMake 3.21.1+

# How to build
Just install all the dependencies using native package manager.
Then run the following command:
```
cmake . -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -B ./build/ -DCMAKE_BUILD_TYPE=Debug && cmake --build ./build/ -j 12 --target all
```
