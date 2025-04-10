set(BIN_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/bin/test)

link_libraries(conet)

# logger
file(GLOB LOGGER_SRCS base/log/*.cc)
foreach(LOGGER_SRC_FILE ${LOGGER_SRCS})
    get_filename_component(LOGGER_BASE_NAME ${LOGGER_SRC_FILE} NAME_WE)  
    add_executable(${LOGGER_BASE_NAME} ${LOGGER_SRC_FILE})
    set_target_properties(${LOGGER_BASE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}/base/log)
endforeach()