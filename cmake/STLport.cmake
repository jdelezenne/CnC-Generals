# STLport 4.5.3 predates the split MSVC/UCRT header directories.
find_path(GEN_MSVC_INCLUDE_DIR vcruntime.h PATHS ENV INCLUDE REQUIRED)
find_path(GEN_UCRT_INCLUDE_DIR corecrt.h PATHS ENV INCLUDE REQUIRED)
set(stlport_support "${CMAKE_BINARY_DIR}/generated/stlport")
file(MAKE_DIRECTORY "${stlport_support}/gen_native_crt" "${stlport_support}/gen_native_cpp")
foreach(include_dir "${GEN_MSVC_INCLUDE_DIR}" "${GEN_UCRT_INCLUDE_DIR}")
    file(GLOB headers "${include_dir}/*.h")
    foreach(header IN LISTS headers)
        get_filename_component(filename "${header}" NAME)
        if(NOT EXISTS "${stlport_support}/gen_native_crt/${filename}")
            file(CONFIGURE OUTPUT "${stlport_support}/gen_native_crt/${filename}"
                CONTENT "#include \"${header}\"\n" @ONLY)
        endif()
    endforeach()
endforeach()
# Use VCRuntime facilities without importing Microsoft's container templates
# into the namespace occupied by the original STLport implementation.
foreach(header new exception typeinfo cstddef cstdlib cstring utility)
    file(CONFIGURE OUTPUT "${stlport_support}/gen_native_cpp/${header}"
        CONTENT "#include \"${GEN_MSVC_INCLUDE_DIR}/${header}\"\n" @ONLY)
endforeach()
file(CONFIGURE OUTPUT "${stlport_support}/gen_native_cpp/new.h"
    CONTENT "#include \"${GEN_UCRT_INCLUDE_DIR}/new.h\"\n" @ONLY)
file(CONFIGURE OUTPUT "${stlport_support}/gen_stlport_native.h" CONTENT
"#define _STLP_NATIVE_HEADER(header) <${GEN_MSVC_INCLUDE_DIR}/header>
#define _STLP_NATIVE_C_HEADER(header) <gen_native_crt/header>
#define _STLP_NATIVE_CPP_C_HEADER(header) <${GEN_MSVC_INCLUDE_DIR}/header>
#define _STLP_NATIVE_CPP_RUNTIME_HEADER(header) <gen_native_cpp/header>
" @ONLY)
