
# The default directory for the settings
set(STEREO_DEFAULT_SETTINGS_DIR "${DEFAULT_ORB_SETTINGS_DIR}/Stereo/")

if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.12.0")
   add_compile_definitions(DEFAULT_STEREO_SETTINGS_DIR="${STEREO_DEFAULT_SETTINGS_DIR}")
else()
  add_definitions(-DDEFAULT_STEREO_SETTINGS_DIR="${STEREO_DEFAULT_SETTINGS_DIR}")
endif()


# EuRoC
add_executable(stereo_euroc stereo_euroc.cc)
target_link_libraries(stereo_euroc ORB_SLAM2)
set_target_properties(stereo_euroc PROPERTIES OUTPUT_NAME stereo_euroc${EXE_POSTFIX})

# KITTI
add_executable(stereo_kitti stereo_kitti.cc)
target_link_libraries(stereo_kitti ORB_SLAM2)
set_target_properties(stereo_kitti PROPERTIES OUTPUT_NAME stereo_kitti${EXE_POSTFIX})

# Install the yaml settings files
install(TARGETS stereo_euroc stereo_kitti RUNTIME DESTINATION ${BUILD_INSTALL_PREFIX}/bin)
file(GLOB settings_files "*.yaml")
install(FILES ${settings_files} DESTINATION ${STEREO_DEFAULT_SETTINGS_DIR})

# Install the timestamps, although there's actually no evidence they are used
install(DIRECTORY EuRoC_TimeStamps DESTINATION ${STEREO_DEFAULT_SETTINGS_DIR})
