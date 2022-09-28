cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
include(CMakeParseArguments)

function(prefix_all OUTPUT)
    cmake_parse_arguments(PARSE_ARGV 1 "ARG" "" "PREFIX" "LIST")
    list(APPEND RESULT)
    foreach (ITEM ${ARG_LIST})
        list(APPEND RESULT "${ARG_PREFIX}${ITEM}")
    endforeach ()
    set("${OUTPUT}" ${RESULT} PARENT_SCOPE)
endfunction()

