add_executable(NBT_example main.cpp)
target_link_libraries(NBT_example PRIVATE NBT)

if (TARGET NBT_warnings)
    target_link_libraries(NBT_example PRIVATE NBT_warnings)
endif()