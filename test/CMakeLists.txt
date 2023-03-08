file(GLOB test_resources *.cc)

foreach(test_source ${test_resources})
    # Create binary names from filenames
    message(STATUS "test file path ${test_source}")

    get_filename_component(test_filename ${test_source} NAME)
    message(STATUS "test filename ${test_filename}")

    string(REPLACE ".cc" "" test_name ${test_filename})
    message(STATUS "test name ${test_name}")

    add_executable(${test_name} ${test_source})
    target_link_libraries(${test_name} server_shared pthread)
endforeach(test_source ${test_resources})
