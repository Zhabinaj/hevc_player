# MIT License
# Copyright (c) 2022 craig-barecpper@crog.uk
# Distributed under the MIT License. See accompanying LICENSE or https://cmake.org/licensing for details.

# @note 3.20 required for `GENERATED` attribute to be project-wide i.e. Version.h isn't build until build-time
cmake_minimum_required(VERSION 3.0)

#TODO? if ( DEFINED VERSION_SEMANTIC )
    #return()
#endif()

message(CHECK_START " GitVersioner.cmake")

list(APPEND CMAKE_MESSAGE_INDENT "  ")

set(VERSION_OUT_DIR "${CMAKE_BINARY_DIR}" CACHE PATH "Destination directory into which `Version.cmake` shall genrate Versioning header files")
set(VERSION_SOURCE_DIR "${CMAKE_SOURCE_DIR}" CACHE PATH "Repositroy directory used for `Version.cmake` repo versioning")

# Get cmakeVersion information
message(CHECK_START " Find git")
find_package( Git )
if( NOT Git_FOUND )
    message(CHECK_FAIL "Not found in PATH")
else()
    message(CHECK_PASS " Found: '${GIT_EXECUTABLE}'")
endif()

# Git describe
# @note Exclude 'tweak' tags in the form v0.1.2-30 i.e. with the '-30' to avoid a second suffix being appended e.g v0.1.2-30-12
set(GIT_VERSION_COMMAND "${GIT_EXECUTABLE}" -C "${VERSION_SOURCE_DIR}" --no-pager describe --tags --exclude "v[0-9]*[._][0-9]*[._][0-9]*-[0-9]*" --always --dirty --long)

# Git count
# @note We only count commits on the current branch and not comits in merge branches via '--first-parent'. The count is never unique but the Sha will be!
set(GIT_COUNT_COMMAND "${GIT_EXECUTABLE}" -C "${VERSION_SOURCE_DIR}" rev-list --count  --first-parent HEAD)

# Git cache path
set(GIT_CACHE_PATH_COMMAND "${GIT_EXECUTABLE}" -C "${VERSION_SOURCE_DIR}" rev-parse --git-dir)

macro(version_parseSemantic semVer)
    message ("VER: " ${semVer})
    if( "${semVer}" MATCHES "^v?([0-9]+)[._]([0-9]+)[._]?([0-9]+)?[-]([0-9]+)[-][g]([._0-9A-Fa-f]+)[-]?(dirty)?$")
        set( _VERSION_SET TRUE)
        math( EXPR _VERSION_MAJOR  "${CMAKE_MATCH_1}+0")
        math( EXPR _VERSION_MINOR  "${CMAKE_MATCH_2}+0")
        math( EXPR _VERSION_PATCH  "${CMAKE_MATCH_3}+0")
        math( EXPR _VERSION_COMMIT "${CMAKE_MATCH_4}+0")
        set( _VERSION_SHA   "${CMAKE_MATCH_5}")
        set( _VERSION_DIRTY "${CMAKE_MATCH_6}")
        set( _VERSION_SEMANTIC ${_VERSION_MAJOR}.${_VERSION_MINOR}.${_VERSION_PATCH}.${_VERSION_COMMIT} )
        set( _VERSION_FULL ${git_describe} )
    elseif( "${semVer}" MATCHES "[-]?(release)?([0-9]+)[._]([0-9]+)[._]?([0-9]+)?[-]([0-9]+)[-][g]([._0-9A-Fa-f]+)[-]?(dirty)?$")
        set( _VERSION_SET TRUE)
        math( EXPR _VERSION_MAJOR  "${CMAKE_MATCH_2}+0")
        math( EXPR _VERSION_MINOR  "${CMAKE_MATCH_3}+0")
        math( EXPR _VERSION_PATCH  "${CMAKE_MATCH_4}+0")
        math( EXPR _VERSION_COMMIT "${CMAKE_MATCH_5}+0")
        set( _VERSION_SHA   "${CMAKE_MATCH_6}")
        set( _VERSION_DIRTY "${CMAKE_MATCH_7}")
        set( _VERSION_SEMANTIC ${_VERSION_MAJOR}.${_VERSION_MINOR}.${_VERSION_PATCH}.${_VERSION_COMMIT} )
        set( _VERSION_FULL ${git_describe} )
    else()
        set( _VERSION_SET FALSE)
    endif()
endmacro()

macro(version_export_variables)
    set( VERSION_SET      ${_VERSION_SET} CACHE INTERNAL "" FORCE)
    set( VERSION_MAJOR    ${_VERSION_MAJOR} CACHE INTERNAL "" FORCE)
    set( VERSION_MINOR    ${_VERSION_MINOR} CACHE INTERNAL "" FORCE)
    set( VERSION_PATCH    ${_VERSION_PATCH} CACHE INTERNAL "" FORCE)
    set( VERSION_COMMIT   ${_VERSION_COMMIT} CACHE INTERNAL "" FORCE)
    set( VERSION_SHA      ${_VERSION_SHA} CACHE INTERNAL "" FORCE)
    set( VERSION_DIRTY    ${_VERSION_DIRTY} CACHE INTERNAL "" FORCE)
    set( VERSION_SEMANTIC ${_VERSION_SEMANTIC} CACHE INTERNAL "" FORCE)
    set( VERSION_FULL     ${_VERSION_FULL} CACHE INTERNAL "" FORCE)
endmacro()



message(CHECK_START " Git Describe")

execute_process(
    COMMAND           ${GIT_VERSION_COMMAND}
    RESULT_VARIABLE   _GIT_RESULT
    OUTPUT_VARIABLE   git_describe
    ERROR_VARIABLE    _GIT_ERROR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_STRIP_TRAILING_WHITESPACE
    ${capture_output}
)

if( NOT _GIT_RESULT EQUAL 0 )
    message( CHECK_FAIL "Failed: ${GIT_VERSION_COMMAND}\nResult:'${_GIT_RESULT}' Error:'${_GIT_ERROR}'")
    if ( "${_GIT_ERROR}" STREQUAL "fatal: bad revision 'HEAD'")
        set(_VERSION_NOT_GIT_REPO TRUE) # Flag that we don't have a valid git-repository'
    endif()
else()
    message(CHECK_PASS " Success '${git_describe}'")

    message(CHECK_START " Parse version")
    version_parseSemantic(${git_describe})
    if( ${_VERSION_SET} )
        message(CHECK_PASS " Tag '${git_describe}' is a valid semantic version [${_VERSION_SEMANTIC}]")
    else()
        message(CHECK_FAIL "'${git_describe}' is not a valid semantic-version e.g. 'v0.1.2-30'")
    endif()
endif()


version_export_variables()

string(TIMESTAMP date "%Y%m%d" UTC)

message(${date})

