rm -r intermediate &> /dev/null
mkdir intermediate
set -x

# Manual compilation
echo "Running compilation in manual mode without opt..." > /dev/null
clang -O0 -emit-llvm -c ssa_phi.c -o intermediate/manual.O0.bc
llvm-dis intermediate/manual.O0.bc
llc intermediate/manual.O0.bc -filetype=obj
clang intermediate/manual.O0.o -o intermediate/manual.O0
./intermediate/manual.O0

# Manual compilation take 2
echo "Running compilation in manual mode with opt..." > /dev/null
clang -O1 -emit-llvm -c ssa_phi.c -o intermediate/manual.O1.bc
llvm-dis intermediate/manual.O1.bc
llc intermediate/manual.O1.bc -filetype=obj
clang intermediate/manual.O1.o -o intermediate/manual.O1
./intermediate/manual.O1

