
cmake_minimum_required(VERSION 3.16)

project(UDQL LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Sql Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Sql Widgets)

# Chuong trinh kiem tra ket noi SQL
add_executable(kiemtra_sql
    kiemtra_sql.cpp
    DatabaseManager.cpp
    DatabaseManager.h
)

target_link_libraries(kiemtra_sql PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Sql
)

# Ung dung quan ly nha hang
file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/oop/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/services/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/chucnang/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/taikhoan/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/admin/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/nhanvien/*.cpp"
)

add_executable(TastyRestaurant
    main.cpp
    DatabaseManager.cpp
    DatabaseManager.h
    ${APP_SOURCES}
)

target_link_libraries(TastyRestaurant PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Sql
)

# Copy default configuration only on first configure; preserve values saved by the app.
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/database.ini")
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/database.ini"
        "${CMAKE_CURRENT_BINARY_DIR}/database.ini"
        COPYONLY
    )
endif()

# Ung dung giao dien Qt Widgets (xanh la pastel)
add_executable(TastyRestaurantWidget    WIN32
    main_widget.cpp
    DatabaseManager.cpp
    DatabaseManager.h
    ${APP_SOURCES}
)
target_link_libraries(TastyRestaurantWidget PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Widgets
)
# Copy default configuration only on first configure; preserve values saved by the app.
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/database.ini")
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/database.ini"
        "${CMAKE_CURRENT_BINARY_DIR}/database.ini"
        COPYONLY
    )
endif()
