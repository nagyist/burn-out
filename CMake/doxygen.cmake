find_package(Doxygen 1.8.1)

add_custom_target(doxygen)

function (create_doxygen outputdir inputdir name)
    foreach (tag ${ARGN})
        string(REGEX REPLACE "=.*" "" tagfile ${tag})
        set(tagdeps
            ${tagdeps}
            ${tagfile})
    endforeach (tag)

    add_custom_command(
        OUTPUT  ${outputdir}/${name}
        COMMAND cmake -E make_directory ${outputdir}/${name}
        COMMENT "Creating documentation directory for ${name}")
    add_custom_command(
        OUTPUT  ${outputdir}/${name}/Doxyfile
        COMMAND ${CMAKE_COMMAND}
                -D "DOXY_INCLUDE_ROOT=${vidtk_all_SOURCE_DIR}"
                -D "DOXYGEN_TEMPLATE=${vidtk_all_SOURCE_DIR}/Doxyfile.in"
                -D "DOXY_PROJECT_SOURCE_DIR=${inputdir}"
                -D "DOXY_DOCUMENTATION_OUTPUT_PATH=${outputdir}"
                -D "DOXY_PROJECT_NAME=${name}"
                -D "DOXY_TAG_FILES=${ARGN}"
                -P "${vidtk_all_SOURCE_DIR}/CMake/doxygen-script.cmake"
        DEPENDS ${vidtk_all_SOURCE_DIR}/Doxyfile.in
                ${outputdir}/${name}
        WORKING_DIRECTORY
                ${outputdir}/${name}
        COMMENT "Generating Doxyfile for ${name}")
    add_custom_command(
        OUTPUT  ${outputdir}/${name}.tag
        COMMAND ${DOXYGEN_EXECUTABLE}
        DEPENDS ${outputdir}/${name}/Doxyfile
        WORKING_DIRECTORY
                ${outputdir}/${name}
        COMMENT "Creating tag for ${name}")
    add_custom_command(
        OUTPUT  ${outputdir}/${name}/index.html
        COMMAND ${DOXYGEN_EXECUTABLE}
        DEPENDS ${outputdir}/${name}.tag
                ${tagdeps}
        WORKING_DIRECTORY
                ${outputdir}/${name}
        COMMENT "Creating HTML documentation for ${name}")
    add_custom_target(doxygen-${name}
        DEPENDS ${outputdir}/${name}/index.html)
    add_dependencies(doxygen
        doxygen-${name})

#     FIXME
#     install(
#         DIRECTORY   ${outputdir}/${name}
#         DESTINATION ${CMAKE_INSTALL_PREFIX}/share/doc/${CMAKE_PROJECT_NAME}-${vidtk_VERSION}/${name}
#         COMPONENT   documentation)
endfunction (create_doxygen)
