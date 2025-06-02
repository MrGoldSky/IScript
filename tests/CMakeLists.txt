include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  iscript_tests
  function_test.cpp
  types_test.cpp
  loop_and_branch_test.cpp
  lexer_test.cpp
  lexer_fib_test.cpp
  unary_operator_test.cpp
  parser_test.cpp
  slice_test.cpp
  branch_tests.cpp
  nil_test.cpp
  illegal_ops_test.cpp
  list_test.cpp
  stdlib_edge_tests.cpp
  multi_tests.cpp
)

target_link_libraries(
  iscript_tests
  iscript
  GTest::gtest_main
)

target_include_directories(iscript_tests PUBLIC ${PROJECT_SOURCE_DIR})

include(GoogleTest)

gtest_discover_tests(iscript_tests)