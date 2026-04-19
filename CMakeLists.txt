cmake_minimum_required(VERSION 3.16)
project(PhotoManager VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_PREFIX_PATH "D:/Qt/6.10.0/mingw_64")

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Test)

# automatic handling of Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(SOURCES
        "Infrastructure layer/MetricsCollector_APP.h"
        "Infrastructure layer/Aspect_APP.h"
        "Infrastructure layer/Logger_APP.h"
        "Infrastructure layer/PhotoRepository_APP.h"
        "Infrastructure layer/UserRepository_APP.h"
        "Infrastructure layer/StorageStrategy_APP.h"
        "Infrastructure layer/AuthenticationService_APP.h"
        main_APP.cpp
        Domain/User.cpp
        Domain/Photo.cpp
        Domain/Album.h
        "Infrastructure layer/UserRepository_APP.cpp"
        "Infrastructure layer/PhotoRepository_APP.cpp"
        "Infrastructure layer/Logger_APP.cpp"
        "Infrastructure layer/AuthenticationService_APP.cpp"
        "Infrastructure layer/AuthenticationFactory.cpp"
        "Infrastructure layer/AuthenticationFactory_APP.h"
        "Infrastructure layer/StorageStrategy_APP.cpp"
        "Business Logic Layer/ImageProcessor_APP.cpp"
        "Business Logic Layer/PhotoManagementFacade_APP.cpp"
        "Business Logic Layer/Command_APP.cpp"
        "Business Logic Layer/PhotoSearchCriteria_APP.cpp"
        "Business Logic Layer/SearchEngine_APP.h"
        "Presentation Layer (QT GUI)/LoginWindow_APP.cpp"
        "Presentation Layer (QT GUI)/MainWindow_APP.cpp"
        "Presentation Layer (QT GUI)/UploadDialog_APP.cpp"
        "Presentation Layer (QT GUI)/SearchDialog_APP.cpp"
        "Presentation Layer (QT GUI)/PhotoDetailsDialog_APP.cpp"
        "Presentation Layer (QT GUI)/AdminPanel_APP.cpp"
        "Presentation Layer (QT GUI)/SettingsDialog.cpp"
        "Presentation Layer (QT GUI)/SearchResultsWindow_APP.cpp"
        "Presentation Layer (QT GUI)/Theme/ThemeManager.cpp"
        "Business Logic Layer/ChainOfResponsability_APP.cpp"
        "Business Logic Layer/ChainOfResponsability.h"
        "Business Logic Layer/PhotoMemento.cpp"
        "Business Logic Layer/PhotoMemento.h"
        "Business Logic Layer/EditHistory.cpp"
        "Business Logic Layer/EditHistory.h"
        "Business Logic Layer/PhotoVisitor_APP.cpp"
        "Business Logic Layer/FileSizeFormatter_APP.h"
)

# Header files
set(HEADERS
        "Infrastructure layer/UserRepository_APP.h"
        "Infrastructure layer/PhotoRepository_APP.h"
        "Infrastructure layer/Logger_APP.h"
        "Infrastructure layer/StorageStrategy_APP.h"
        "Infrastructure layer/AuthenticationService_APP.h"
        "Infrastructure layer/AuthenticationFactory_APP.h"
        "Business Logic Layer/ImageProcessor_APP.h"
        "Business Logic Layer/PhotoManagementFacade_APP.h"
        "Business Logic Layer/Command_APP.h"
        "Business Logic Layer/PhotoSearchCriteria_APP.h"
        "Presentation Layer (QT GUI)/LoginWindow_APP.h"
        "Presentation Layer (QT GUI)/MainWindow_APP.h"
        "Presentation Layer (QT GUI)/UploadDialog_APP.h"
        "Presentation Layer (QT GUI)/SearchDialog_APP.h"
        "Presentation Layer (QT GUI)/SearchResultsWindow_APP.h"
        "Presentation Layer (QT GUI)/PhotoDetailsDialog_APP.h"
        "Presentation Layer (QT GUI)/AdminPanel.h"
        "Presentation Layer (QT GUI)/SettingsDialog.h"
        "Presentation Layer (QT GUI)/Theme/ThemeManager.h"
)

# Create executable
add_executable(PhotoManager ${SOURCES})

# ===== Tests =====
set(TEST_SOURCES
        tests/main_APP.cpp
        tests/test_user_APP.cpp
        tests/test_photo_APP.cpp
        tests/test_search_APP.cpp
        tests/test_repository_APP.cpp
        tests/test_facade_APP.cpp
        tests/test_aspect_metrics_APP.cpp
        tests/test_ui_APP.cpp
        tests/test_theme_APP.cpp
        tests/test_search_dialog_APP.cpp
        tests/test_upload_dialog_APP.cpp
)

add_executable(PhotoManagerTests ${TEST_SOURCES} ${SOURCES})

# Add include directories
target_include_directories(PhotoManager PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Domain
        "${CMAKE_CURRENT_SOURCE_DIR}/Infrastructure layer"
        "${CMAKE_CURRENT_SOURCE_DIR}/Business Logic Layer"
        "${CMAKE_CURRENT_SOURCE_DIR}/Presentation Layer (QT GUI)"
)


# Qt libraries
target_link_libraries(PhotoManager PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
)

target_include_directories(PhotoManagerTests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Domain
        "${CMAKE_CURRENT_SOURCE_DIR}/Infrastructure layer"
        "${CMAKE_CURRENT_SOURCE_DIR}/Business Logic Layer"
        "${CMAKE_CURRENT_SOURCE_DIR}/Presentation Layer (QT GUI)"
)

target_link_libraries(PhotoManagerTests PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Test
)

# output directory
set_target_properties(PhotoManager PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
