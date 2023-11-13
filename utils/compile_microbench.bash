script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
clang="$script_dir/../llvm/build/bin/clang"
opt="$script_dir/../llvm/build/bin/opt"
name="$(echo $(basename "$1") | cut -d. -f 1 -)"

$clang -O0 -S -emit-llvm --target=aarch64-linux-gnu -static $name.c
sed -i 's/optnone//g' $name.ll 
$opt -S -passes=mem2reg $name.ll -o $name.ll
$clang -O0 -S --target=aarch64-linux-gnu -static $name.ll
