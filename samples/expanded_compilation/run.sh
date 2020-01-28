rm -r intermediate &> /dev/null
mkdir intermediate
set -x

# Auto compilation
echo "Running normal auto compilation..." > /dev/null
clang add.c -o intermediate/auto
./intermediate/auto

# Manual compilation
echo "Running compilation in manual mode without opt..." > /dev/null
clang -O0 -emit-llvm -c add.c -o intermediate/manual.O0.bc
llvm-dis intermediate/manual.O0.bc
llc intermediate/manual.O0.bc -filetype=obj
ld -dynamic-linker /lib/ld-linux-x86-64.so.2 -o \
    intermediate/manual.O0 /usr/lib/crt1.o /usr/lib/crti.o \
    -lc intermediate/manual.O0.o /usr/lib/crtn.o
chmod +x intermediate/manual.O0
./intermediate/manual.O0

# Manual compilation take 2
echo "Running compilation in manual mode with opt..." > /dev/null
clang -O1 -emit-llvm -c add.c -o intermediate/manual.O1.bc
llvm-dis intermediate/manual.O1.bc
llc intermediate/manual.O1.bc -filetype=obj
clang intermediate/manual.O1.o -o intermediate/manual.O1
./intermediate/manual.O1

