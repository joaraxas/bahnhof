add_library(localization_lib
    src/localization.cpp
)

target_include_directories(localization_lib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
