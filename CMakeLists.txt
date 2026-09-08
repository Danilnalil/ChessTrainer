cmake_minimum_required(VERSION 3.16)
project(MyChessTrainer VERSION 0.1.0 LANGUAGES CXX)

# Устанавливаем современный стандарт C++ (рекомендую 20, так как он удобен)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Важно для Windows: указываем, что это приложение с графическим окном (консоль не нужна)
# Если вы хотите видеть вывод std::cout во время отладки, закомментируйте эту строку.
# set(CMAKE_WIN32_EXECUTABLE ON)

# Подключаем SFML через vcpkg. Обратите внимание на синтаксис для версии 3.1
find_package(SFML CONFIG REQUIRED COMPONENTS Graphics Window System)

# Создаём исполняемый файл из вашего главного исходника
set(SOURCES src/main.cpp src/Board.cpp src/Functions.cpp)
set(HEADERS include/Functions.h include/Board.h)
add_executable(MyChessTrainer ${SOURCES} ${HEADERS})
target_include_directories(MyChessTrainer PRIVATE include)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${SOURCES} ${HEADERS})
# Линкуем библиотеки SFML к нашему проекту
target_link_libraries(${PROJECT_NAME} PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)

# Указываем, где искать свои заголовки (папка include)
target_include_directories(${PROJECT_NAME} PRIVATE include)

# ======================================================
# Для Windows: автоматически копируем DLL-ки SFML в папку с .exe
# ======================================================
if (WIN32)
    # Получаем путь к папке с библиотеками SFML, которую установил vcpkg
    get_target_property(SFML_GRAPHICS_LOCATION SFML::Graphics IMPORTED_LOCATION_RELEASE)
    if (SFML_GRAPHICS_LOCATION)
        get_filename_component(SFML_BIN_DIR ${SFML_GRAPHICS_LOCATION} DIRECTORY)
        
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${SFML_BIN_DIR}/sfml-graphics-3.dll"
                "${SFML_BIN_DIR}/sfml-window-3.dll"
                "${SFML_BIN_DIR}/sfml-system-3.dll"
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMENT "Копирование DLL SFML в папку с программой"
        )
    endif()
endif()