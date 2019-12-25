
cmake_minimum_required(VERSION 2.8)

if ( WIN32 AND NOT CYGWIN AND NOT ( CMAKE_SYSTEM_NAME STREQUAL "WindowsStore" ) AND NOT ANDROID)
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT" CACHE STRING "")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd" CACHE STRING "")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT" CACHE STRING "")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd" CACHE STRING "")
endif ()

project(Zstd)

if ( IOS )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fembed-bitcode")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fembed-bitcode")
endif ()

find_path(ZSTD_UNITY_PROJECT_DIR NAMES SConstruct
	PATHS
	${CMAKE_SOURCE_DIR}
	NO_DEFAULT_PATH
	)

MARK_AS_ADVANCED(ZSTD_UNITY_PROJECT_DIR)

include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	zstd/lib
	zstd/lib/common
	zstd/lib/decompress
)

set( ZSTD_SRC
	zstd/lib/common/debug.c
	zstd/lib/common/error_private.c
	zstd/lib/common/pool.c
	zstd/lib/common/xxhash.c
	zstd/lib/common/entropy_common.c
	zstd/lib/common/fse_decompress.c
	zstd/lib/common/threading.c
	zstd/lib/common/zstd_common.c
	zstd/lib/decompress/huf_decompress.c
	zstd/lib/decompress/zstd_ddict.c
	zstd/lib/decompress/zstd_decompress.c
	zstd/lib/decompress/zstd_decompress_block.c
	zstd/lib/legacy/zstd_v01.c
	zstd/lib/legacy/zstd_v02.c
	zstd/lib/legacy/zstd_v03.c
	zstd/lib/legacy/zstd_v04.c
	zstd/lib/legacy/zstd_v05.c
	zstd/lib/legacy/zstd_v06.c
	zstd/lib/legacy/zstd_v07.c

)

set( ZSTD_CORE
	zstd.c
)

macro(source_group_by_dir proj_dir source_files)
    if(MSVC OR APPLE)
        get_filename_component(sgbd_cur_dir ${proj_dir} ABSOLUTE)
        foreach(sgbd_file ${${source_files}})
			get_filename_component(sgbd_abs_file ${sgbd_file} ABSOLUTE)
            file(RELATIVE_PATH sgbd_fpath ${sgbd_cur_dir} ${sgbd_abs_file})
            string(REGEX REPLACE "\(.*\)/.*" \\1 sgbd_group_name ${sgbd_fpath})
            string(COMPARE EQUAL ${sgbd_fpath} ${sgbd_group_name} sgbd_nogroup)
            string(REPLACE "/" "\\" sgbd_group_name ${sgbd_group_name})
            if(sgbd_nogroup)
                set(sgbd_group_name "\\")
            endif(sgbd_nogroup)
            source_group(${sgbd_group_name} FILES ${sgbd_file})
        endforeach(sgbd_file)
    endif(MSVC OR APPLE)
endmacro(source_group_by_dir)

source_group_by_dir(${CMAKE_CURRENT_SOURCE_DIR} ZSTD_CORE)
source_group_by_dir(${CMAKE_CURRENT_SOURCE_DIR} ZSTD_SRC)

if (APPLE)
	if (IOS)
		set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD)")
		add_library(zstd STATIC
			${ZSTD_SRC}
			${ZSTD_CORE}
		)
		set_xcode_property (zstd IPHONEOS_DEPLOYMENT_TARGET "7.0" "all")
	else ()
		set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD_64_BIT)")
		add_library(zstd MODULE
			${ZSTD_SRC}
			${ZSTD_CORE}
		)
		set_target_properties ( zstd PROPERTIES BUNDLE TRUE )
	endif ()
else ()
	add_library(zstd SHARED
		${ZSTD_SRC}
		${ZSTD_CORE}
	)
endif ()

if ( WIN32 AND NOT CYGWIN )
	target_link_libraries(zstd
		m
	)
endif ()

