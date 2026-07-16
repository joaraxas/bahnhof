add_executable(bahnhof_test
    track/connection_test.cpp
)

add_executable(text_test
    ui/text_test.cpp
)

target_link_libraries(bahnhof_test PRIVATE
    bahnhof_lib
)

target_link_libraries(text_test PRIVATE
    bahnhof_lib
)