#!/bin/bash



#!/bin/bash

# Variables
CMAKE_BUILD_TYPE="MinSizeRel"
LLVM_PROJECTS="clang"
LLVM_TARGETS="AArch64"
BUILD_DIR="build"
SRC_DIR="llvm"
OUT_DIR="output"
CMAKE="cmake"
NATIVE_BIN="bin/native"
LABELLED_BIN="bin/labelled"
LABELLED_WITH_PROFILE_BIN="bin/labelled_with_profile"
RUN_SCRIPT="./run_simulation.sh"  # Script for running the compiled binaries
THREADS=1

# Functions

# Configure the CMake build system
configure_with_aliashints() {
    cd llvm/
    echo "Configuring the LLVM build system..."
    $CMAKE -B $BUILD_DIR -S $SRC_DIR \
        -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE \
        -DLLVM_ENABLE_PROJECTS="$LLVM_PROJECTS" \
        -DLLVM_TARGETS_TO_BUILD=$LLVM_TARGETS \
        -DLLVM_ALIASHINTSPASS_LINK_INTO_TOOLS=ON \
        -DLLVM_ALIASHINTSINSTRUMENTATIONPASS_LINK_INTO_TOOLS=OFF
    cd ..
}

configure_with_instrumentation() {
    cd llvm/
    echo "Configuring the LLVM build system for instrumentation..."
    $CMAKE -B $BUILD_DIR -S $SRC_DIR \
        -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE \
        -DLLVM_ENABLE_PROJECTS="$LLVM_PROJECTS" \
        -DLLVM_TARGETS_TO_BUILD=$LLVM_TARGETS \
        -DLLVM_ALIASHINTSPASS_LINK_INTO_TOOLS=OFF \
        -DLLVM_ALIASHINTSINSTRUMENTATIONPASS_LINK_INTO_TOOLS=ON
    cd ..
}

# Build llvm
build_llvm() {
    #configure_with_aliashints
    #configure_with_instrumentation
    cd llvm
    echo "Building LLVM..."
    $CMAKE --build $BUILD_DIR -j $THREADS
    cd ..
}

# Build Gem5
build_gem5() {
    echo "Building Gem5..."
    cd gem5
    scons build/ARM/gem5.opt --ignore-style -j $THREADS
    cd ..
}

# Build Native Coremark
build_native_coremark() {
    echo "Building native Coremark..."
    cd benchmarks/coremark-pro
    make TARGET=linux64 -j $THREADS || true  # Continue even if make fails
    cd ../..
    cp benchmarks/coremark-pro/builds/linux64/gcc64/bin/*.exe bin/native/
}

# Build Labelled Coremark
build_labelled_coremark() {
    echo "Building Labelled Coremark..."
    cd benchmarks/coremark-pro
    # make TARGET=linux64 wbuild-sha-test -j $THREADS || true  # Continue even if make fails
    make TARGET=linux64 -j $THREADS || true  # Continue even if make fails
    cd ../..
    cp benchmarks/coremark-pro/builds/linux64/gcc64/bin/*.exe bin/labelled/
}

build_labelled_coremark_with_profile() {
    echo "Building Labelled Coremark with profile information..."
    
    sed -i 's|\(std::string PNDProfileFilename = "\)[^"]*\(";\)|\1/rds/general/user/jj21/home/fyp/pnd-loads/profile_files/processed/sha-test.exe-profile-filtered.txt\2|g' llvm/llvm/lib/Transforms/AliasHints/AliasHints.cpp
    build_llvm
    cd benchmarks/coremark-pro
    make TARGET=linux64 wbuild-sha-test -j $THREADS || true  # Continue even if make fails
    cd ../..

    sed -i 's|\(std::string PNDProfileFilename = "\)[^"]*\(";\)|\1/rds/general/user/jj21/home/fyp/pnd-loads/profile_files/processed/radix2-big-64k.exe-profile-filtered.txt\2|g' llvm/llvm/lib/Transforms/AliasHints/AliasHints.cpp
    build_llvm
    cd benchmarks/coremark-pro
    make TARGET=linux64 wbuild-radix2-big-64k -j $THREADS || true  # Continue even if make fails
    cd ../..

    sed -i 's|\(std::string PNDProfileFilename = "\)[^"]*\(";\)|\1/rds/general/user/jj21/home/fyp/pnd-loads/profile_files/processed/linear_alg-mid-100x100-sp.exe-profile-filtered.txt\2|g' llvm/llvm/lib/Transforms/AliasHints/AliasHints.cpp
    build_llvm
    cd benchmarks/coremark-pro
    make TARGET=linux64 wbuild-linear_alg-mid-100x100-sp -j $THREADS || true  # Continue even if make fails
    cd ../..

    sed -i 's|\(std::string PNDProfileFilename = "\)[^"]*\(";\)|\1/rds/general/user/jj21/home/fyp/pnd-loads/profile_files/processed/parser-125k.exe-profile-filtered.txt\2|g' llvm/llvm/lib/Transforms/AliasHints/AliasHints.cpp
    build_llvm
    cd benchmarks/coremark-pro
    make TARGET=linux64 wbuild-parser-125k -j $THREADS || true  # Continue even if make fails
    cd ../..

    sed -i 's|\(std::string PNDProfileFilename = "\)[^"]*\(";\)|\1/rds/general/user/jj21/home/fyp/pnd-loads/profile_files/processed/nnet_test.exe-profile-filtered.txt\2|g' llvm/llvm/lib/Transforms/AliasHints/AliasHints.cpp
    build_llvm
    cd benchmarks/coremark-pro
    make TARGET=linux64 wbuild-nnet_test -j $THREADS || true  # Continue even if make fails
    cd ../..

    sed -i 's|\(std::string PNDProfileFilename = "\)[^"]*\(";\)|\1/rds/general/user/jj21/home/fyp/pnd-loads/profile_files/processed/cjpeg-rose7-preset.exe-profile-filtered.txt\2|g' llvm/llvm/lib/Transforms/AliasHints/AliasHints.cpp
    build_llvm
    cd benchmarks/coremark-pro
    make TARGET=linux64 wbuild-cjpeg-rose7-preset -j $THREADS || true  # Continue even if make fails
    cd ../..

    sed -i 's|\(std::string PNDProfileFilename = "\)[^"]*\(";\)|\1/rds/general/user/jj21/home/fyp/pnd-loads/profile_files/processed/core.exe-profile-filtered.txt\2|g' llvm/llvm/lib/Transforms/AliasHints/AliasHints.cpp
    build_llvm
    cd benchmarks/coremark-pro
    make TARGET=linux64 wbuild-core -j $THREADS || true  # Continue even if make fails
    cd ../..

    sed -i 's|\(std::string PNDProfileFilename = "\)[^"]*\(";\)|\1/rds/general/user/jj21/home/fyp/pnd-loads/profile_files/processed/loops-all-mid-10k-sp.exe-profile-filtered.txt\2|g' llvm/llvm/lib/Transforms/AliasHints/AliasHints.cpp
    build_llvm
    cd benchmarks/coremark-pro
    make TARGET=linux64 wbuild-loops-all-mid-10k-sp -j $THREADS || true  # Continue even if make fails
    cd ../..

    sed -i 's|\(std::string PNDProfileFilename = "\)[^"]*\(";\)|\1/rds/general/user/jj21/home/fyp/pnd-loads/profile_files/processed/zip-test.exe-profile-filtered.txt\2|g' llvm/llvm/lib/Transforms/AliasHints/AliasHints.cpp
    build_llvm
    cd benchmarks/coremark-pro
    make TARGET=linux64 wbuild-zip-test -j $THREADS || true  # Continue even if make fails
    cd ../..


    cp benchmarks/coremark-pro/builds/linux64/gcc64/bin/*.exe bin/labelled_with_profile/
}

# Build Instrumented Coremark
build_instrumented_coremark() {
    echo "Building Labelled Coremark..."
    cd benchmarks/coremark-pro
    make TARGET=linux64 -j $THREADS || true  # Continue even if make fails
    cd ../..
    cp benchmarks/coremark-pro/builds/linux64/gcc64/bin/*.exe bin/instrumented/
}

# Genereted Address list
gen_addr_list(){
    echo "Generating address list..."

    export PATH=/rds/general/user/jj21/home/fyp/radare2//bin:$PATH

    # Iterate over all files in the directory
    for file in "$LABELLED_BIN"/*; do
        file_name=$(basename "$file")
        if [ -f "$NATIVE_BIN/$file_name" ] && [ -f "$LABELLED_BIN/$file_name" ]; then 
            echo "Processing file: $file_name"
            python3 utils/parse_labelled_binary.py "$LABELLED_BIN/$file_name" "$NATIVE_BIN/$file_name" "johan_temp" &
        else
            echo "File mismatch: $file_name"
        fi
    done
    wait
    echo "Done Generating address list.."

}

# Genereted Address list
gen_addr_list_with_profile(){
    echo "Generating address list..."

    export PATH=/rds/general/user/jj21/home/fyp/radare2//bin:$PATH

    # Iterate over all files in the directory
    for file in "$LABELLED_WITH_PROFILE_BIN"/*; do
        file_name=$(basename "$file")
        if [ -f "$NATIVE_BIN/$file_name" ] && [ -f "$LABELLED_WITH_PROFILE_BIN/$file_name" ]; then 
            echo "Processing file: $file_name"
            python3 utils/parse_labelled_binary.py "$LABELLED_WITH_PROFILE_BIN/$file_name" "$NATIVE_BIN/$file_name" "johan_temp" &
        else
            echo "File mismatch: $file_name"
        fi
    done
    wait
    echo "Done Generating address list.."

}

gen_profile(){
    export export PATH=/rds/general/user/jj21/home/fyp/qemu-7.0.0/build_artifacts:$PATH
    for exe in bin/instrumented/*.exe; do
        if [ -f "$exe" ]; then
            filename=$(basename "$exe")
            echo "Executing $exe..."
            qemu-aarch64 "$exe" -v0 -i1 > "profile_files/raw/$filename-profile.txt" &
        fi
    done
    wait
    echo "Done emulating all"
}

process_profile(){
    python3 filter_profile_output.py 
}

# Run the simulation
run() { 
    #Run simulation with profile information"

    export ADDR_FILE=addr_files/johan_temp/sha-test.exe
    ./gem5/build/ARM/gem5.opt --outdir=output/with_profile/sha-test gem5/configs/deprecated/example/se.py --cpu-type=DerivO3CPU --caches -c "bin/native/sha-test.exe" --options="-v0 -i1" &

    export ADDR_FILE=addr_files/johan_temp/radix2-big-64k.exe
    ./gem5/build/ARM/gem5.opt --outdir=output/with_profile/radix2-big-64k gem5/configs/deprecated/example/se.py --cpu-type=DerivO3CPU --caches -c "bin/native/radix2-big-64k.exe" --options="-v0 -i1" &

    export ADDR_FILE=addr_files/johan_temp/linear_alg-mid-100x100-sp.exe
    ./gem5/build/ARM/gem5.opt --outdir=output/with_profile/linear_alg-mid-100x100-sp gem5/configs/deprecated/example/se.py --cpu-type=DerivO3CPU --caches -c "bin/native/linear_alg-mid-100x100-sp.exe" --options="-v0 -i1" &

    export ADDR_FILE=addr_files/johan_temp/parser-125k.exe
    ./gem5/build/ARM/gem5.opt --outdir=output/with_profile/parser-125k gem5/configs/deprecated/example/se.py --cpu-type=DerivO3CPU --caches -c "bin/native/parser-125k.exe" --options="-v0 -i1" &

    export ADDR_FILE=addr_files/johan_temp/nnet_test.exe
    ./gem5/build/ARM/gem5.opt --outdir=output/with_profile/nnet_test gem5/configs/deprecated/example/se.py --cpu-type=DerivO3CPU --caches -c "bin/native/nnet_test.exe" --options="-v0 -i1" &

    export ADDR_FILE=addr_files/johan_temp/loops-all-mid-10k-sp.exe
    ./gem5/build/ARM/gem5.opt --outdir=output/with_profile/loops-all-mid-10k-sp gem5/configs/deprecated/example/se.py --cpu-type=DerivO3CPU --caches -c "bin/native/loops-all-mid-10k-sp.exe" --options="-v0 -i1" &

    export ADDR_FILE=addr_files/johan_temp/core.exe
    ./gem5/build/ARM/gem5.opt --outdir=output/with_profile/core gem5/configs/deprecated/example/se.py --cpu-type=DerivO3CPU --caches -c "bin/native/core.exe" --options="-v0 -i1" &

    export ADDR_FILE=addr_files/johan_temp/zip-test.exe
    ./gem5/build/ARM/gem5.opt --outdir=output/with_profile/zip-test gem5/configs/deprecated/example/se.py --cpu-type=DerivO3CPU --caches -c "bin/native/zip-test.exe" --options="-v0 -i1" &

    export ADDR_FILE=addr_files/johan_temp/cjpeg-rose7-preset.exe
    ./gem5/build/ARM/gem5.opt --outdir=output/with_profile/cjpeg-rose7-preset gem5/configs/deprecated/example/se.py --cpu-type=DerivO3CPU --caches -c "bin/native/cjpeg-rose7-preset.exe" --options="-v0 -i1" &



    # Run simulation without any label

    # export ADDR_FILE=addr_files/johan_temp/empty.exe
    # ./gem5/build/ARM/gem5.opt --outdir=output/no_labels/radix2-big-64k gem5/configs/deprecated/example/se.py --cpu-type=DerivO3CPU --caches -c "bin/native/radix2-big-64k.exe" --options="-v0 -i1" &

    # export ADDR_FILE=addr_files/johan_temp/empty.exe
    # ./gem5/build/ARM/gem5.opt --outdir=output/no_labels/linear_alg-mid-100x100-sp gem5/configs/deprecated/example/se.py --cpu-type=DerivO3CPU --caches -c "bin/native/linear_alg-mid-100x100-sp.exe" --options="-v0 -i1" &

    # export ADDR_FILE=addr_files/johan_temp/empty.exe
    # ./gem5/build/ARM/gem5.opt --outdir=output/no_labels/sha-test gem5/configs/deprecated/example/se.py --cpu-type=DerivO3CPU --caches -c "bin/native/sha-test.exe" --options="-v0 -i1" &

    # export ADDR_FILE=addr_files/johan_temp/empty.exe
    # ./gem5/build/ARM/gem5.opt --outdir=output/no_labels/cjpeg-rose7-preset gem5/configs/deprecated/example/se.py --cpu-type=DerivO3CPU --caches -c "bin/native/cjpeg-rose7-preset.exe" --options="-v0 -i1" &

    # export ADDR_FILE=addr_files/johan_temp/empty.exe
    # ./gem5/build/ARM/gem5.opt --outdir=output/no_labels/parser-125k gem5/configs/deprecated/example/se.py --cpu-type=DerivO3CPU --caches -c "bin/native/parser-125k.exe" --options="-v0 -i1" &

    # export ADDR_FILE=addr_files/johan_temp/empty.exe
    # ./gem5/build/ARM/gem5.opt --outdir=output/no_labels/nnet_test gem5/configs/deprecated/example/se.py --cpu-type=DerivO3CPU --caches -c "bin/native/nnet_test.exe" --options="-v0 -i1" &

    # export ADDR_FILE=addr_files/johan_temp/empty.exe
    # ./gem5/build/ARM/gem5.opt --outdir=output/no_labels/loops-all-mid-10k-sp gem5/configs/deprecated/example/se.py --cpu-type=DerivO3CPU --caches -c "bin/native/loops-all-mid-10k-sp.exe" --options="-v0 -i1" &

    # export ADDR_FILE=addr_files/johan_temp/empty.exe
    # ./gem5/build/ARM/gem5.opt --outdir=output/no_labels/zip-test gem5/configs/deprecated/example/se.py --cpu-type=DerivO3CPU --caches -c "bin/native/zip-test.exe" --options="-v0 -i1" &

    # export ADDR_FILE=addr_files/johan_temp/empty.exe
    # ./gem5/build/ARM/gem5.opt --outdir=output/no_labels/core gem5/configs/deprecated/example/se.py --cpu-type=DerivO3CPU --caches -c "bin/native/core.exe" --options="-v0 -i1" &

    wait
}

# Clean build artifacts
clean() {
    echo "Cleaning directories not fully implemented..."
    cd benchmarks/coremark-pro
    make clean
    cd ../..
}


# Main
if [ -z "$2" ]; then
  echo "Number of threads not provided. Default to 1"
else
  THREADS=$(expr "$2")
fi

case "$1" in
    "configure_with_aliashints")
        configure_with_aliashints
        ;;
    # "configure_with_instrumentation")
    #     configure_with_instrumentation
    #     ;;
    "build_llvm")
        build_llvm
        ;;
    "build_gem5")
        build_gem5
        ;;
    "build_native_coremark")
        build_native_coremark
        ;;
    "build_labelled_coremark")
        build_labelled_coremark
        ;;
    "build_labelled_coremark_with_profile")
        build_labelled_coremark_with_profile
        ;;
    "build_instrumented_coremark")
        build_instrumented_coremark
        ;;
    "gen_addr_list")
        gen_addr_list
        ;;
    "gen_addr_list_with_profile")
        gen_addr_list_with_profile
        ;;
    "gen_profile")
        gen_profile
        ;;
    "process_profile")
        process_profile
        ;;
    "run")
        run
        ;;
    "clean")
        clean
        ;;
    *)
        echo "Usage: $0 {configure_with_*|build_llvm|build_gem5|build_*_coremark|gen_addr_list*|gen_profile|process_profile|run|clean} {Number of threads}"
        exit 1
        ;;
esac
