# The default directory for the settings
set(RGBD_DEFAULT_SETTINGS_DIR "${DEFAULT_ORB_SETTINGS_DIR}/RGB-D/")

if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.12.0")
   add_compile_definitions(DEFAULT_RGB_SETTINGS_DIR="${RGBD_DEFAULT_SETTINGS_DIR}")
else()
  add_definitions(-DDEFAULT_RGB_SETTINGS_DIR="${RGBD_DEFAULT_SETTINGS_DIR}")
endif()


# TUM
add_executable(rgbd_tum rgbd_tum.cc)
target_link_libraries(rgbd_tum ORB_SLAM2)
set_target_properties(rgbd_tum PROPERTIES OUTPUT_NAME rgbd_tum${EXE_POSTFIX})

# Install the yaml settings files
install(TARGETS rgbd_tum RUNTIME DESTINATION ${BUILD_INSTALL_PREFIX}/bin)
file(GLOB settings_files "*.yaml")
install(FILES ${settings_files} DESTINATION ${RGBD_DEFAULT_SETTINGS_DIR})

# Install the timestamps, although there's actually no evidence they are used
install(DIRECTORY associations DESTINATION ${RGBD_DEFAULT_SETTINGS_DIR})
