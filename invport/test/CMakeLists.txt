set(_link_libraries iex::iex invport_lib ${GTEST_LIBRARIES})
set(test_exec unit_test)

add_executable(${test_exec}
        unit_test.cc
        file_test.cc
        keychain_test.cc
        transaction_test.cc
        transaction_history_test.cc
        )

target_link_libraries(${test_exec} ${_link_libraries})

gtest_discover_tests(${test_exec})