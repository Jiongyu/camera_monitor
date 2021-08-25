# aux_source_directory(. DIR_SRCS)

# # 指定生成目标
# add_executable(test ${DIR_SRCS})
# target_link_libraries (test  ${PROJECT_NAME})

add_executable(test test.cpp)
target_link_libraries (test  ${PROJECT_NAME})

add_executable(test_save_data test_save_data.cpp)
target_link_libraries (test_save_data  ${PROJECT_NAME})
