g++ build.cc -o build
./build cpubench
g++ objects/* -o bench
g++ cpubench/cpubench.cc -O2 -o piml_cpubench
./piml_cpubench -t 6 -d 500 -- ./bench
