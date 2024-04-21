target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu_arithmetic_instructions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu_bit_instructions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu_general_instructions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu_jump_instructions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu_load_instructions.cpp
)