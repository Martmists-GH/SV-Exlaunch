function(setup_atmosphere_for subsdk modname)
    release_target(
            atmosphere
            /atmosphere/contents/${TITLE_ID}/exefs/
            /atmosphere/contents/${TITLE_ID}/romfs/
            ${subsdk}
            ${modname}
    )
    zip_target(atmosphere)
    ftp_target(atmosphere)
endfunction()

function(setup_atmosphere_for subsdk modname)
    release_target(
            atmosphere
            /atmosphere/contents/${TITLE_ID}/exefs/
            /atmosphere/contents/${TITLE_ID}/romfs/
            ${subsdk}
            ${modname}
    )
    zip_target(atmosphere)
    ftp_target(atmosphere)
endfunction()

function(setup_modmanager_for subsdk modname)
    if ("${TITLE_ID}" STREQUAL "0100A3D008C5C000")
        set(GAME_NAME "Pokemon Scarlet")
    else()
        set(GAME_NAME "Pokemon Violet")
    endif()

    release_target(
            modmanager
            "/mods/${GAME_NAME}/${MODULE_NAME}/contents/${TITLE_ID}/exefs/"
            "/mods/${GAME_NAME}/${MODULE_NAME}/contents/${TITLE_ID}/romfs/"
            ${subsdk}
            ${modname}
    )
    zip_target(modmanager)
    ftp_target(modmanager)
endfunction()

function(setup_ryujinx_for subsdk modname)
    release_target(
            ryujinx
            "/mods/contents/${TITLE_ID}/${MODULE_NAME}/exefs/"
            "/mods/contents/${TITLE_ID}/${MODULE_NAME}/romfs/"
            ${subsdk}
            ${modname}
    )
    zip_target(ryujinx)
    ftp_target(ryujinx)
endfunction()

function(setup_yuzu_for subsdk modname)
    release_target(
            yuzu
            "/load/${TITLE_ID}/${MODULE_NAME}/exefs/"
            "/load/${TITLE_ID}/${MODULE_NAME}/romfs/"
            ${subsdk}
            ${modname}
    )
    zip_target(yuzu)
    ftp_target(yuzu)
endfunction()

function(setup_targets_for subsdk modname)
    setup_atmosphere_for(${subsdk} ${modname})
    setup_modmanager_for(${subsdk} ${modname})
    setup_ryujinx_for(${subsdk} ${modname})
    setup_yuzu_for(${subsdk} ${modname})
endfunction()
