include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/switch/SwitchTools.cmake)

add_custom_target(all_modules)

function(add_module index name)
    cmake_parse_arguments(MODULE_ARGS "" "" "INCLUDE;SOURCE" ${ARGN})

    set(ALL_INCLUDE)
    foreach(include ${MODULE_ARGS_INCLUDE})
        list(APPEND ALL_INCLUDE ${include})
    endforeach()

    set(ALL_SOURCE)
    foreach(source ${MODULE_ARGS_SOURCE})
        file(GLOB_RECURSE SOURCE_FILES ${source}/*.c ${source}/*.cpp ${source}/*.s)
        list(APPEND ALL_INCLUDE ${source})
        list(APPEND ALL_SOURCE ${SOURCE_FILES})
    endforeach()

    add_custom_target(${name})
    add_dependencies(all_modules ${name})

    set_property(
        TARGET ${name}
        PROPERTY MODULE_INCLUDE ${ALL_INCLUDE}
    )
    set_property(
        TARGET ${name}
        PROPERTY MODULE_SOURCE ${ALL_SOURCE}
    )
    set_property(
        TARGET ${name}
        PROPERTY SUBSDK "subsdk${index}"
    )

    create_releases_main(${name})
endfunction()

function(add_module_variant module variant title_id game)
    cmake_parse_arguments(VARIANT_ARGS "" "NPDM_TEMPLATE;LINKER_SCRIPT;SPECS_TEMPLATE" "INCLUDE;SOURCE" ${ARGN})

    if (NOT DEFINED VARIANT_ARGS_NPDM_TEMPLATE)
        set(VARIANT_ARGS_NPDM_TEMPLATE ${CMAKE_CURRENT_SOURCE_DIR}/module/npdm.json.template)
    endif()

    if (NOT DEFINED VARIANT_ARGS_LINKER_SCRIPT)
        set(VARIANT_ARGS_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/module/module.ld)
    endif()

    if (NOT DEFINED VARIANT_ARGS_SPECS_TEMPLATE)
        set(VARIANT_ARGS_SPECS_TEMPLATE ${CMAKE_CURRENT_SOURCE_DIR}/module/module.specs.template)
    endif()

    if (NOT TARGET ${module})
        error(FATAL_ERROR "Module ${module} not found")
    endif()

    set(ALL_INCLUDE)
    foreach(include ${VARIANT_ARGS_INCLUDE})
        list(APPEND ALL_INCLUDE ${include})
    endforeach()

    set(ALL_SOURCE)
    foreach(source ${VARIANT_ARGS_SOURCE})
        file(GLOB_RECURSE SOURCE_FILES ${source}/*.c ${source}/*.cpp ${source}/*.asm)
        list(APPEND ALL_SOURCE ${SOURCE_FILES})
    endforeach()

    set(variant ${module}_${variant})

    get_target_property(PARENT_INCLUDE ${module} MODULE_INCLUDE)
    get_target_property(PARENT_SOURCE ${module} MODULE_SOURCE)

    add_executable(${variant} ${ALL_SOURCE} ${PARENT_SOURCE})
    target_include_directories(${variant} PUBLIC ${ALL_INCLUDE} ${PARENT_INCLUDE})

    string(LENGTH ${module} MODULE_NAME_LEN)
    target_compile_definitions(
        ${variant}
        PUBLIC
            EXL_PROGRAM_ID=0x${title_id}
            NNSDK=1
            EXL_LOAD_KIND=Module
            EXL_LOAD_KIND_ENUM=EXL_LOAD_KIND_MODULE
            MODULE_NAME="${module}"
            MODULE_NAME_LEN=${MODULE_NAME_LEN}
            TITLE_ID=0x${title_id}
            LOGGER_IP="${LOGGER_IP}"
            LOGGER_PORT=${LOGGER_PORT}
    )

    string(TOLOWER "0x${title_id}" CONFIG_TITLE_ID)
    set(SUBSDK_LD_PATH ${VARIANT_ARGS_LINKER_SCRIPT})
    if (MINGW OR WIN32)
        string(REGEX REPLACE "^/([a-zA-Z])/" "\\1:/" SUBSDK_LD_PATH "${SUBSDK_LD_PATH}")
    endif ()

    configure_file(
            ${VARIANT_ARGS_NPDM_TEMPLATE} ${CMAKE_CURRENT_BINARY_DIR}/${variant}.npdm.json
    )
    configure_file(
            ${VARIANT_ARGS_SPECS_TEMPLATE} ${CMAKE_CURRENT_BINARY_DIR}/${variant}.specs
    )

    if (NOT DEFINED ${variant}_npdm)
        create_npdm(${variant} ${title_id})
    endif()

    get_target_property(SUBSDK_NAME ${module} SUBSDK)
    create_nso(${variant} ${SUBSDK_NAME})

    add_custom_target(${variant}_all ALL DEPENDS ${variant}_npdm ${variant}_nso)
    add_dependencies(${module} ${variant}_all)

    create_releases(${module} ${game} ${variant} ${title_id})
endfunction()

function(create_releases_main module)
    add_custom_target(${module}_release_all)
    add_custom_target(${module}_release_variants)
    add_custom_target(${module}_zip_all)

    function(create_release target with_ftp)
        add_custom_target(${module}_release_${target} DEPENDS ${module}_release_variants)
        add_custom_command(
                TARGET ${module}_release_${target}
                COMMAND mkdir -p "${CMAKE_CURRENT_BINARY_DIR}/${module}_releases/${target}/"
                COMMAND sh -c 'cp -r ${CMAKE_CURRENT_BINARY_DIR}/${module}_*_releases/${target}/* ${CMAKE_CURRENT_BINARY_DIR}/${module}_releases/${target}/'
        )

        add_dependencies(${module}_release_all ${module}_release_${target})

        add_custom_target(${module}_zip_${target} DEPENDS ${module}_release_${target})
        add_custom_command(
                TARGET ${module}_zip_${target}
                COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/zips/
                COMMAND zip -r "${CMAKE_CURRENT_BINARY_DIR}/zips/${module}_${target}.zip" *
                WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${module}_releases/${target}/"
        )

        add_dependencies(${module}_zip_all ${module}_zip_${target})

        if (${with_ftp})
            add_custom_target(
                ${module}_ftp_${target}
                DEPENDS ${module}_release_${target}
                COMMAND python3 "${CMAKE_CURRENT_SOURCE_DIR}/scripts/send_patch.py" ${FTP_IP} ${FTP_PORT} ${FTP_USER} ${FTP_PASS} "${CMAKE_CURRENT_BINARY_DIR}/${module}_releases/${target}/"
            )
        endif()
    endfunction()

    create_release(atmosphere TRUE)
    create_release(modmanager TRUE)
    create_release(ryujinx FALSE)
    create_release(yuzu FALSE)
endfunction()

function(create_releases module game_name variant title_id)
    add_custom_target(${variant}_release_all)
    add_dependencies(${module}_release_variants ${variant}_release_all)

    function(create_release target folder)
        add_custom_target(${variant}_release_${target} DEPENDS ${variant}_all)
        add_custom_command(
                TARGET ${variant}_release_${target}
                COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/${variant}_releases/${target}/${folder}
                COMMAND sh -c \"cp -r ${CMAKE_CURRENT_BINARY_DIR}/${variant}_out/* ${CMAKE_CURRENT_BINARY_DIR}/${variant}_releases/${target}/${folder}\"
        )

        add_dependencies(${variant}_release_all ${variant}_release_${target})
    endfunction()

    create_release(atmosphere "atmosphere/contents/${title_id}/exefs/")
    create_release(modmanager "mods/${game_name}/${module}/contents/${title_id}/exefs/")
    create_release(ryujinx "mods/contents/${title_id}/${module}/exefs/")
    create_release(yuzu "load/${title_id}/${module}/exefs/")
endfunction()

function(create_npdm variant title_id)
    add_custom_target(
            ${variant}_npdm
            COMMAND mkdir -p "${CMAKE_CURRENT_BINARY_DIR}/${variant}_out"
            COMMAND ${npdmtool} "${CMAKE_CURRENT_BINARY_DIR}/${variant}.npdm.json" "${CMAKE_CURRENT_BINARY_DIR}/${variant}_out/main.npdm"
            DEPENDS ${variant}
            VERBATIM
    )
endfunction()

function(create_nso variant subsdk)
    add_custom_command(
            OUTPUT ${CMAKE_BINARY_DIR}/${variant}_out/${subsdk}
            COMMAND mkdir -p "${CMAKE_CURRENT_BINARY_DIR}/${variant}_out"
            COMMAND ${elf2nso} $<TARGET_FILE:${variant}> "${CMAKE_CURRENT_BINARY_DIR}/${variant}_out/${subsdk}"
            DEPENDS ${variant}
            VERBATIM
    )

    # Add the respective NSO target and set the required linker flags for the original target.
    add_custom_target(${variant}_nso SOURCES ${CMAKE_BINARY_DIR}/${variant}_out/${subsdk})
    set_target_properties(${variant} PROPERTIES LINK_FLAGS "-specs=${CMAKE_CURRENT_BINARY_DIR}/${variant}.specs -g3 ${ARCH} -nostartfiles")
endfunction()
