add_library(framework-tests
    aliaser_tests.cpp
    shallow_aliaser_tests.cpp
)

target_link_libraries(framework-tests framework)
