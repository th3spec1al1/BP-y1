add_executable(${PROJECT_NAME} main.cpp
        ../lib/Scheduler.h
        ../lib/SomeType.h
        )

target_link_libraries(${PROJECT_NAME} scheduler)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})