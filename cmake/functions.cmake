function(add_subsdk_target subsdk modname titleid gameversion)
    set(ALL_SOURCE)

    foreach(PATH ${ARGV})
        include_directories(${PATH})
        file(GLOB_RECURSE SOURCES_ASM ${PATH}/*.s)
        file(GLOB_RECURSE SOURCES_C ${PATH}/*.c)
        file(GLOB_RECURSE SOURCES_CXX ${PATH}/*.cpp)

        list(APPEND ALL_SOURCE ${SOURCES_ASM} ${SOURCES_C} ${SOURCES_CXX})
    endforeach()

    add_executable(${subsdk} ${ALL_SOURCE})
    string(LENGTH ${modname} MODULE_NAME_LEN)
    add_compile_definitions(EXL_LOAD_KIND=Module EXL_LOAD_KIND_ENUM=EXL_LOAD_KIND_MODULE MODULE_NAME="${modname}" MODULE_NAME_LEN=${MODULE_NAME_LEN} TITLE_ID=0x${titleid} GAME_VERSION=0x${gameversion})

    set(SUBSDK_LD_PATH ${CMAKE_CURRENT_SOURCE_DIR}/module/${subsdk}.ld)
    if (MINGW OR WIN32)
        string(REGEX REPLACE "^/([a-zA-Z])/" "\\1:/" SUBSDK_LD_PATH "${SUBSDK_LD_PATH}")
    endif ()
    if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${subsdk}.specs)
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/module/${subsdk}.specs.template ${CMAKE_CURRENT_BINARY_DIR}/${subsdk}.specs)
    endif ()
    if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/npdm.json)
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/module/npdm.json.template ${CMAKE_CURRENT_BINARY_DIR}/npdm.json)
    endif ()

    if(NOT TARGET create_npdm)
        __generate_npdm(${CMAKE_CURRENT_BINARY_DIR}/npdm.json)
    endif()

    nso_target(${subsdk})
    setup_targets_for(${subsdk} ${modname})
endfunction()

function(nso_target target)
    add_nso_target_subsdk(${target})
    set_target_properties(${target} PROPERTIES CONFIG_JSON "${CMAKE_CURRENT_BINARY_DIR}/${target}.json")
    set_target_properties(${target} PROPERTIES LINK_FLAGS "-specs ${CMAKE_CURRENT_BINARY_DIR}/${target}.specs -g3 ${ARCH} -nostartfiles")
endfunction()

function(release_target target exefs romfs subsdk modname)
    add_custom_command(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/release/${target}/${modname}
                   ${CMAKE_CURRENT_BINARY_DIR}/release/${target}/${modname}/${exefs}
                   ${CMAKE_CURRENT_BINARY_DIR}/release/${target}/${modname}/${romfs}
            DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${subsdk} ${subsdk}_nso create_npdm
            # Create release file structure
            COMMAND mkdir -p "${CMAKE_CURRENT_BINARY_DIR}/release/${target}/${modname}/${exefs}"
            COMMAND mkdir -p "${CMAKE_CURRENT_BINARY_DIR}/release/${target}/${modname}/${romfs}"
            # Copy exefs
            COMMAND cp "${CMAKE_CURRENT_BINARY_DIR}/${subsdk}" "${CMAKE_CURRENT_BINARY_DIR}/release/${target}/${modname}/${exefs}/${subsdk}"
            COMMAND cp "${CMAKE_CURRENT_BINARY_DIR}/main.npdm" "${CMAKE_CURRENT_BINARY_DIR}/release/${target}/${modname}/${exefs}/main.npdm"
    )

    add_custom_target(
            release_${target}_${subsdk}
            DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/release/${target}/${modname}
            COMMAND mkdir -p "${CMAKE_CURRENT_BINARY_DIR}/release/${target}/${modname}/${exefs}"
            COMMAND mkdir -p "${CMAKE_CURRENT_BINARY_DIR}/release/${target}/${modname}/${romfs}"
    )
endfunction()

function(zip_target target)
    add_custom_command(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/release/${target}/mod_${modname}.zip
            DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/release/${target}/${modname}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/release/${target}/${modname}
            COMMAND sh -c 'zip -r ../mod_${modname}.zip *'
    )
    add_custom_target(
            zip_${target}_${subsdk}
            DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/release/${target}/mod_${modname}.zip
    )
endfunction()

function(ftp_target target)
    add_custom_target(
            send_${target}_${subsdk}
            DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/release/${target}/${modname}
            COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/scripts/send_patch.py ${FTP_IP} ${FTP_PORT} ${FTP_USER} ${FTP_PASS} ${CMAKE_CURRENT_BINARY_DIR}/release/${target}/${modname}
    )
endfunction()

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/releases.cmake)
