file(GLOB_RECURSE SRCS ${PROJECT_SOURCE_DIR}/src/*.cc )

set(COXTX ${PROJECT_SOURCE_DIR}/src/conet/base/coroutine/coctx_swap.S)

ADD_LIBRARY(conet SHARED ${SRCS} ${COXTX})
target_link_libraries(conet dl)

SET_TARGET_PROPERTIES(conet PROPERTIES OUTPUT_NAME "conet")
SET_TARGET_PROPERTIES(conet PROPERTIES CLEAN_DIRECT_OUTPUT 1)

set(CONET_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/src/include/conet")
install(
    DIRECTORY ${CONET_INCLUDE_DIR}/
    DESTINATION include/conet  
    FILES_MATCHING PATTERN "*.h"
)

install(
    TARGETS conet                
    LIBRARY DESTINATION lib
)