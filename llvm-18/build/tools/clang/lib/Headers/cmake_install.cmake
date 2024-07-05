# Install script for directory: /home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "MinSizeRel")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "clang-resource-headers" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/18/include" TYPE FILE FILES
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/builtins.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/float.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/inttypes.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/iso646.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/limits.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/module.modulemap"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/stdalign.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/stdarg.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__stdarg___gnuc_va_list.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__stdarg___va_copy.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__stdarg_va_arg.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__stdarg_va_copy.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__stdarg_va_list.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/stdatomic.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/stdbool.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/stdckdint.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/stddef.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__stddef_max_align_t.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__stddef_null.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__stddef_nullptr_t.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__stddef_offsetof.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__stddef_ptrdiff_t.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__stddef_rsize_t.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__stddef_size_t.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__stddef_unreachable.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__stddef_wchar_t.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__stddef_wint_t.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/stdint.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/stdnoreturn.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/tgmath.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/unwind.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/varargs.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/arm_acle.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/arm_cmse.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/armintr.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/arm64intr.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/arm_neon_sve_bridge.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__clang_cuda_builtin_vars.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__clang_cuda_math.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__clang_cuda_cmath.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__clang_cuda_complex_builtins.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__clang_cuda_device_functions.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__clang_cuda_intrinsics.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__clang_cuda_texture_intrinsics.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__clang_cuda_libdevice_declares.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__clang_cuda_math_forward_declares.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__clang_cuda_runtime_wrapper.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/hexagon_circ_brev_intrinsics.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/hexagon_protos.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/hexagon_types.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/hvx_hexagon_protos.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__clang_hip_libdevice_declares.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__clang_hip_cmath.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__clang_hip_math.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__clang_hip_stdlib.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__clang_hip_runtime_wrapper.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/larchintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/lasxintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/lsxintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/msa.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/opencl-c.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/opencl-c-base.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/altivec.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/htmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/htmxlintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/riscv_bitmanip.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/riscv_crypto.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/riscv_ntlh.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/sifive_vector.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/s390intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/vecintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/velintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/velintrin_gen.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/velintrin_approx.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/adcintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/adxintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/ammintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/amxcomplexintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/amxfp16intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/amxintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx2intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512bf16intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512bitalgintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512bwintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512cdintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512dqintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512erintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512fintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512fp16intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512ifmaintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512ifmavlintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512pfintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vbmi2intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vbmiintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vbmivlintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vlbf16intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vlbitalgintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vlbwintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vlcdintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vldqintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vlfp16intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vlintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vlvbmi2intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vlvnniintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vlvp2intersectintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vnniintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vp2intersectintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vpopcntdqintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vpopcntdqvlintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avxifmaintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avxintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avxneconvertintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avxvnniint16intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avxvnniint8intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avxvnniintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/bmi2intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/bmiintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/cetintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/cldemoteintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/clflushoptintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/clwbintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/clzerointrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/cmpccxaddintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/crc32intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/emmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/enqcmdintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/f16cintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/fma4intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/fmaintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/fxsrintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/gfniintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/hresetintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/ia32intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/immintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/invpcidintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/keylockerintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/lwpintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/lzcntintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/mm3dnow.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/mmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/movdirintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/mwaitxintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/nmmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/pconfigintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/pkuintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/pmmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/popcntintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/prfchiintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/prfchwintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/ptwriteintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/raointintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/rdpruintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/rdseedintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/rtmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/serializeintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/sgxintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/sha512intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/shaintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/sm3intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/sm4intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/smmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/tbmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/tmmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/tsxldtrkintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/uintrintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/usermsrintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/vaesintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/vpclmulqdqintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/waitpkgintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/wbnoinvdintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__wmmintrin_aes.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/wmmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__wmmintrin_pclmul.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/x86gprintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/x86intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/xmmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/xopintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/xsavecintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/xsaveintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/xsaveoptintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/xsavesintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/xtestintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/cet.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/cpuid.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/wasm_simd128.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/vadefs.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/mm_malloc.h"
    "/home/muke/Programming/PND-Loads/llvm-18/build/tools/clang/lib/Headers/arm_neon.h"
    "/home/muke/Programming/PND-Loads/llvm-18/build/tools/clang/lib/Headers/arm_fp16.h"
    "/home/muke/Programming/PND-Loads/llvm-18/build/tools/clang/lib/Headers/arm_sve.h"
    "/home/muke/Programming/PND-Loads/llvm-18/build/tools/clang/lib/Headers/arm_sme.h"
    "/home/muke/Programming/PND-Loads/llvm-18/build/tools/clang/lib/Headers/arm_bf16.h"
    "/home/muke/Programming/PND-Loads/llvm-18/build/tools/clang/lib/Headers/arm_mve.h"
    "/home/muke/Programming/PND-Loads/llvm-18/build/tools/clang/lib/Headers/arm_cde.h"
    "/home/muke/Programming/PND-Loads/llvm-18/build/tools/clang/lib/Headers/arm_vector_types.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "clang-resource-headers" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/18/include/cuda_wrappers" TYPE FILE FILES
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/cuda_wrappers/algorithm"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/cuda_wrappers/cmath"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/cuda_wrappers/complex"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/cuda_wrappers/new"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "clang-resource-headers" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/18/include/cuda_wrappers/bits" TYPE FILE FILES
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/cuda_wrappers/bits/shared_ptr_base.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/cuda_wrappers/bits/basic_string.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/cuda_wrappers/bits/basic_string.tcc"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "clang-resource-headers" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/18/include/ppc_wrappers" TYPE FILE FILES
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/ppc_wrappers/mmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/ppc_wrappers/xmmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/ppc_wrappers/mm_malloc.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/ppc_wrappers/emmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/ppc_wrappers/pmmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/ppc_wrappers/tmmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/ppc_wrappers/smmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/ppc_wrappers/nmmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/ppc_wrappers/bmiintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/ppc_wrappers/bmi2intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/ppc_wrappers/immintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/ppc_wrappers/x86intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/ppc_wrappers/x86gprintrin.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "clang-resource-headers" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/18/include/llvm_libc_wrappers" TYPE FILE FILES
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/llvm_libc_wrappers/assert.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/llvm_libc_wrappers/stdio.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/llvm_libc_wrappers/stdlib.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/llvm_libc_wrappers/string.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/llvm_libc_wrappers/ctype.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/llvm_libc_wrappers/inttypes.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/llvm_libc_wrappers/time.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "clang-resource-headers" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/18/include/openmp_wrappers" TYPE FILE FILES
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/openmp_wrappers/math.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/openmp_wrappers/cmath"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/openmp_wrappers/complex.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/openmp_wrappers/complex"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/openmp_wrappers/__clang_openmp_device_functions.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/openmp_wrappers/complex_cmath.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/openmp_wrappers/new"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "core-resource-headers")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/18/include" TYPE FILE FILES
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/builtins.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/float.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/inttypes.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/iso646.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/limits.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/module.modulemap"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/stdalign.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/stdarg.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__stdarg___gnuc_va_list.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__stdarg___va_copy.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__stdarg_va_arg.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__stdarg_va_copy.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__stdarg_va_list.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/stdatomic.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/stdbool.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/stdckdint.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/stddef.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__stddef_max_align_t.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__stddef_null.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__stddef_nullptr_t.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__stddef_offsetof.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__stddef_ptrdiff_t.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__stddef_rsize_t.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__stddef_size_t.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__stddef_unreachable.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__stddef_wchar_t.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__stddef_wint_t.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/stdint.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/stdnoreturn.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/tgmath.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/unwind.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/varargs.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "arm-common-resource-headers")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/18/include" TYPE FILE FILES
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/arm_acle.h"
    "/home/muke/Programming/PND-Loads/llvm-18/build/tools/clang/lib/Headers/arm_neon.h"
    "/home/muke/Programming/PND-Loads/llvm-18/build/tools/clang/lib/Headers/arm_fp16.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "arm-resource-headers")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/18/include" TYPE FILE FILES
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/arm_cmse.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/armintr.h"
    "/home/muke/Programming/PND-Loads/llvm-18/build/tools/clang/lib/Headers/arm_mve.h"
    "/home/muke/Programming/PND-Loads/llvm-18/build/tools/clang/lib/Headers/arm_cde.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "aarch64-resource-headers")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/18/include" TYPE FILE FILES
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/arm64intr.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/arm_neon_sve_bridge.h"
    "/home/muke/Programming/PND-Loads/llvm-18/build/tools/clang/lib/Headers/arm_sve.h"
    "/home/muke/Programming/PND-Loads/llvm-18/build/tools/clang/lib/Headers/arm_sme.h"
    "/home/muke/Programming/PND-Loads/llvm-18/build/tools/clang/lib/Headers/arm_bf16.h"
    "/home/muke/Programming/PND-Loads/llvm-18/build/tools/clang/lib/Headers/arm_vector_types.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "cuda-resource-headers")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/18/include/cuda_wrappers" TYPE FILE FILES
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/cuda_wrappers/algorithm"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/cuda_wrappers/cmath"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/cuda_wrappers/complex"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/cuda_wrappers/new"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "cuda-resource-headers")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/18/include/cuda_wrappers/bits" TYPE FILE FILES
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/cuda_wrappers/bits/shared_ptr_base.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/cuda_wrappers/bits/basic_string.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/cuda_wrappers/bits/basic_string.tcc"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "cuda-resource-headers")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/18/include" TYPE FILE FILES
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__clang_cuda_builtin_vars.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__clang_cuda_math.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__clang_cuda_cmath.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__clang_cuda_complex_builtins.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__clang_cuda_device_functions.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__clang_cuda_intrinsics.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__clang_cuda_texture_intrinsics.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__clang_cuda_libdevice_declares.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__clang_cuda_math_forward_declares.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__clang_cuda_runtime_wrapper.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "hexagon-resource-headers")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/18/include" TYPE FILE FILES
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/hexagon_circ_brev_intrinsics.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/hexagon_protos.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/hexagon_types.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/hvx_hexagon_protos.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "hip-resource-headers")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/18/include" TYPE FILE FILES
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__clang_hip_libdevice_declares.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__clang_hip_cmath.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__clang_hip_math.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__clang_hip_stdlib.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__clang_hip_runtime_wrapper.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "loongarch-resource-headers")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/18/include" TYPE FILE FILES
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/larchintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/lasxintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/lsxintrin.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "mips-resource-headers")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/18/include" TYPE FILE FILES "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/msa.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "ppc-resource-headers")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/18/include/ppc_wrappers" TYPE FILE FILES
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/ppc_wrappers/mmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/ppc_wrappers/xmmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/ppc_wrappers/mm_malloc.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/ppc_wrappers/emmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/ppc_wrappers/pmmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/ppc_wrappers/tmmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/ppc_wrappers/smmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/ppc_wrappers/nmmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/ppc_wrappers/bmiintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/ppc_wrappers/bmi2intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/ppc_wrappers/immintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/ppc_wrappers/x86intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/ppc_wrappers/x86gprintrin.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "ppc-resource-headers")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/18/include" TYPE FILE FILES "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/altivec.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "ppc-htm-resource-headers")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/18/include" TYPE FILE FILES
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/htmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/htmxlintrin.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "riscv-resource-headers")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/18/include" TYPE FILE FILES
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/riscv_bitmanip.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/riscv_crypto.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/riscv_ntlh.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/sifive_vector.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "systemz-resource-headers")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/18/include" TYPE FILE FILES
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/s390intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/vecintrin.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "ve-resource-headers")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/18/include" TYPE FILE FILES
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/velintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/velintrin_gen.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/velintrin_approx.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "webassembly-resource-headers")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/18/include" TYPE FILE FILES "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/wasm_simd128.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "x86-resource-headers")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/18/include" TYPE FILE FILES
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/adcintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/adxintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/ammintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/amxcomplexintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/amxfp16intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/amxintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx2intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512bf16intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512bitalgintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512bwintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512cdintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512dqintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512erintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512fintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512fp16intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512ifmaintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512ifmavlintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512pfintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vbmi2intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vbmiintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vbmivlintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vlbf16intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vlbitalgintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vlbwintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vlcdintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vldqintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vlfp16intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vlintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vlvbmi2intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vlvnniintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vlvp2intersectintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vnniintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vp2intersectintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vpopcntdqintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avx512vpopcntdqvlintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avxifmaintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avxintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avxneconvertintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avxvnniint16intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avxvnniint8intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/avxvnniintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/bmi2intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/bmiintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/cetintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/cldemoteintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/clflushoptintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/clwbintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/clzerointrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/cmpccxaddintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/crc32intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/emmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/enqcmdintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/f16cintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/fma4intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/fmaintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/fxsrintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/gfniintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/hresetintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/ia32intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/immintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/invpcidintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/keylockerintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/lwpintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/lzcntintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/mm3dnow.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/mmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/movdirintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/mwaitxintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/nmmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/pconfigintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/pkuintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/pmmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/popcntintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/prfchiintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/prfchwintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/ptwriteintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/raointintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/rdpruintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/rdseedintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/rtmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/serializeintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/sgxintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/sha512intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/shaintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/sm3intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/sm4intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/smmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/tbmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/tmmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/tsxldtrkintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/uintrintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/usermsrintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/vaesintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/vpclmulqdqintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/waitpkgintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/wbnoinvdintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__wmmintrin_aes.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/wmmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/__wmmintrin_pclmul.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/x86gprintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/x86intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/xmmintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/xopintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/xsavecintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/xsaveintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/xsaveoptintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/xsavesintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/xtestintrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/cet.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/cpuid.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "hlsl-resource-headers")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/18/include" TYPE FILE FILES "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/hlsl.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "hlsl-resource-headers")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/18/include/hlsl" TYPE FILE FILES
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/hlsl/hlsl_basic_types.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/hlsl/hlsl_intrinsics.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "opencl-resource-headers")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/18/include" TYPE FILE FILES
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/opencl-c.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/opencl-c-base.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "openmp-resource-headers")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/18/include/openmp_wrappers" TYPE FILE FILES
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/openmp_wrappers/math.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/openmp_wrappers/cmath"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/openmp_wrappers/complex.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/openmp_wrappers/complex"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/openmp_wrappers/__clang_openmp_device_functions.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/openmp_wrappers/complex_cmath.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/openmp_wrappers/new"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "openmp-resource-headers")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/18/include/openmp_wrappers" TYPE FILE FILES
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/openmp_wrappers/math.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/openmp_wrappers/cmath"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/openmp_wrappers/complex.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/openmp_wrappers/complex"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/openmp_wrappers/__clang_openmp_device_functions.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/openmp_wrappers/complex_cmath.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/openmp_wrappers/new"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "utility-resource-headers")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/18/include" TYPE FILE FILES "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/mm_malloc.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "windows-resource-headers")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/clang/18/include" TYPE FILE FILES
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/intrin.h"
    "/home/muke/Programming/PND-Loads/llvm-18/clang/lib/Headers/vadefs.h"
    )
endif()

