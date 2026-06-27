add_executable(bahnhof_test
    track/connection_test.cpp
)

target_link_libraries(bahnhof_test PRIVATE
    bahnhof_lib
)