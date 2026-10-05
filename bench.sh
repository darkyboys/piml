g++ build.cc -o build
./build bench
g++ objects/* -o exec_test
./exec_test
