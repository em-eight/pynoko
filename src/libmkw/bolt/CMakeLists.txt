add_subdirectory(util EXCLUDE_FROM_ALL)
add_subdirectory(math EXCLUDE_FROM_ALL)
add_subdirectory(gfx EXCLUDE_FROM_ALL)

# ----------- Create package
include(CMakePackageConfigHelpers)

# set version
set(version 1.0.0)

# generate the version file for the config file
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/boltConfigVersion.cmake"
  VERSION "${version}"
  COMPATIBILITY AnyNewerVersion
)

# create config file
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/boltConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bolt
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

# install config files
install(FILES
          "${CMAKE_CURRENT_BINARY_DIR}/boltConfig.cmake"
          "${CMAKE_CURRENT_BINARY_DIR}/boltConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bolt
)
# ----------- End Create package
