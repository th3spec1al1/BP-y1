add_library(mylib STATIC
    dir_adapter/dir_adapter.h 
    filter_adapter/filter_adapter.h
    open_files_adapter/open_files_adapter.h
    out_adapter/out_adapter.h
    as_data_flow_adapter/as_data_flow_adapter.h
    as_vector_adapter/as_vector_adapter.h
    write_adapter/write_adapter.h
    transform_adapter/transform_adapter.h
    dropnullopt_adapter/dropnullopt_adapter.h
    split_expected_adapter/split_expected_adapter.h)

set_target_properties(mylib PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(mylib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)