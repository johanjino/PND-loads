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
        -DLLVM_ALIASHINTSPROFILEANALYSISPASS_LINK_INTO_TOOLS=ON\
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
    # configure_with_aliashints
    # configure_with_instrumentation
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

# Build Labelled Coremark
build_native_coremark() {
    echo "Building native Coremark..."
    cd benchmarks/coremark-pro
    make TARGET=linux64 -j $THREADS || true  # Continue even if make fails
    cd ../..
    # Need to copy all
}

# Build Labelled Coremark
build_labelled_coremark() {
    echo "Building Labelled Coremark..."
    cd benchmarks/coremark-pro
    make TARGET=linux64 wbuild-sha-test -j $THREADS || true  # Continue even if make fails
    cd ../..
    # Need to copy all, but only sha for now
    cp benchmarks/coremark-pro/builds/linux64/gcc64/bin/sha-test.exe bin/labelled/sha-test.exe
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
            python3 utils/parse_labelled_binary.py "$LABELLED_BIN/$file_name" "$NATIVE_BIN/$file_name" "johan_temp"
        else
            echo "File mismatch: $file_name"
        fi
    done
    echo "Done Generating address list.."

}

# Run the simulation
run() {
    echo "Run implemented only for sha..."
    export ADDR_FILE=addr_files/johan_temp/sha-test.exe
    ./gem5/build/ARM/gem5.opt --outdir=output gem5/configs/deprecated/example/se.py --cpu-type=DerivO3CPU --caches -c "bin/native/sha-test.exe" --options="-v0 -i1"
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
    "gen_addr_list")
        gen_addr_list
        ;;
    "run")
        run
        ;;
    "clean")
        clean
        ;;
    *)
        echo "Usage: $0 {configure_with_aliashints|build_llvm|build_gem5|build_native_coremark|build_labelled_coremark|gen_addr_list|run|clean} {Number of threads}"
        exit 1
        ;;
esac
