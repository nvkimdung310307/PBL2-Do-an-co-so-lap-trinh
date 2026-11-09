cmake_minimum_required(VERSION 3.16)

project(UDQL LANGUAGES CXX)

# Du an hien tai su dung Qt Widgets + file TXT, khong su dung Qt SQL/ODBC.
# RestaurantWindow.cpp dung mot so API QMouseEvent cua Qt 6, vi vay yeu cau Qt 6.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Thu thap cac lop giao dien, nghiep vu, tai khoan, OOP va xu ly du lieu TXT.
file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/oop/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/services/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/data/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/taikhoan/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/widgets/*.cpp"
)
# Ung dung giao dien chinh.
add_executable(TastyRestaurantWidget WIN32
    main.cpp
    ${APP_SOURCES}
)

target_include_directories(TastyRestaurantWidget PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(TastyRestaurantWidget PRIVATE
    Qt6::Core
    Qt6::Widgets
)

# EXE co the nam trong build/, build/Debug/... nhung du lieu luon nam
# tai mot noi duy nhat: database/ trong thu muc ma nguon.
# Dung duong dan tuyet doi duoc dong vao luc configure, khong tim fallback.
file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/database" APP_DATA_DIR_CONFIGURED)
target_compile_definitions(TastyRestaurantWidget PRIVATE
    APP_DATA_DIR=\"${APP_DATA_DIR_CONFIGURED}\"
)

set_target_properties(TastyRestaurantWidget PROPERTIES
    WIN32_EXECUTABLE TRUE
)

message(STATUS "UDQL: Qt Widgets + TXT (khong can Qt Sql/ODBC)")
message(STATUS "UDQL: NGUON DU LIEU DUY NHAT = ${APP_DATA_DIR_CONFIGURED}")
