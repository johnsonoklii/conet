set(BIN_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/bin/test)

link_libraries(conet)

# logger
file(GLOB LOGGER_SRCS base/log/*.cc)
foreach(LOGGER_SRC_FILE ${LOGGER_SRCS})
    get_filename_component(LOGGER_BASE_NAME ${LOGGER_SRC_FILE} NAME_WE)  
    add_executable(${LOGGER_BASE_NAME} ${LOGGER_SRC_FILE})
    set_target_properties(${LOGGER_BASE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}/base/log)
endforeach()

# logger
file(GLOB CO_SRCS base/coroutine/*.cc)
foreach(CO_SRC_FILE ${CO_SRCS})
    get_filename_component(CO_BASE_NAME ${CO_SRC_FILE} NAME_WE)  
    add_executable(${CO_BASE_NAME} ${CO_SRC_FILE})
    set_target_properties(${CO_BASE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}/base/coroutine)
endforeach()


# net
file(GLOB NET_SRCS net/*.cc)
foreach(NET_SRC_FILE ${NET_SRCS})
    get_filename_component(NET_BASE_NAME ${NET_SRC_FILE} NAME_WE)  
    add_executable(${NET_BASE_NAME} ${NET_SRC_FILE})
    set_target_properties(${NET_BASE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}/net)
endforeach()