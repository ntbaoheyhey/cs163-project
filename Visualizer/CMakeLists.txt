cmake_minimum_required(VERSION 3.15)
project(Visualizer)

# SFML 3 yêu cầu bắt buộc tối thiểu là C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Gọi phiên bản 3 và Viết Hoa chữ cái đầu các components
find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

# Liệt kê tất cả các file mã nguồn của project
file(GLOB SRC_FILES "src/*.cpp" "objects/*.cpp")
# Tạo file thực thi
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Khai báo các thư mục chứa file .h để compiler tìm thấy
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/headers
    ${CMAKE_CURRENT_SOURCE_DIR}/objects
)

# Link với thư viện SFML (Dùng Namespace SFML::)
target_link_libraries(${PROJECT_NAME} PRIVATE 
    SFML::Graphics 
    SFML::Window 
    SFML::System
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    VISUALIZER_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    COMMENT "Copying Visualizer assets next to the executable"
)

