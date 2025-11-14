# FILE: backend/CMakeLists.txt
cmake_minimum_required(VERSION 3.15)
project(AutoRiaBackend VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Завантаження cpp-httplib
include(FetchContent)
FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.14.3
)
FetchContent_MakeAvailable(httplib)

# SQLite3
find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)

set(SOURCES
    src/main.cpp
    src/models/User.cpp
    src/models/Role.cpp
    src/models/Listing.cpp
    src/models/Brand.cpp
    src/database/Database.cpp
    src/repositories/UserRepository.cpp
    src/repositories/ListingRepository.cpp
    src/repositories/BrandRepository.cpp
    src/middleware/AuthMiddleware.cpp
    src/services/ModerationService.cpp
    src/services/CurrencyService.cpp
    src/services/StatisticsService.cpp
    src/api/ApiServer.cpp
)

set(HEADERS
    src/models/User.h
    src/models/Role.h
    src/models/Listing.h
    src/models/Brand.h
    src/database/Database.h
    src/repositories/UserRepository.h
    src/repositories/ListingRepository.h
    src/repositories/BrandRepository.h
    src/middleware/AuthMiddleware.h
    src/services/ModerationService.h
    src/services/CurrencyService.h
    src/services/StatisticsService.h
    src/api/ApiServer.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${httplib_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${SQLITE3_LIBRARIES}
)

target_compile_options(${PROJECT_NAME} PRIVATE
    ${SQLITE3_CFLAGS_OTHER}
)



