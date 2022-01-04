clang++ -g -O3 codegen.cc `llvm-config --cxxflags --ldflags --system-libs --libs core` -o toy
# Run
./toy