cmake_minimum_required(VERSION 3.16)
project(LibraTechTO VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Quick Sql QuickControls2 Concurrent Test)

set(MODELO_DIR Modelo)
set(CONTROLADOR_DIR Controlador)
set(VISTA_DIR Vista)

set(PROJECT_SOURCES
    main.cpp
    ${MODELO_DIR}/Libro.cpp
    ${MODELO_DIR}/BibliotecaListModel.cpp
    ${MODELO_DIR}/HistorialListModel.cpp
    ${CONTROLADOR_DIR}/SingletonDB.cpp
    ${CONTROLADOR_DIR}/BibliotecaController.cpp
    ${CONTROLADOR_DIR}/HistorialController.cpp
    ${CONTROLADOR_DIR}/RecomendacionController.cpp
)

qt_add_resources(RESOURCES
    ${VISTA_DIR}/bibliotecavirtual.qrc
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${RESOURCES}
    Controlador/BibliotecaFilterModel.h
    Controlador/BibliotecaFilterModel.cpp
    Modelo/Usuario.h
    Modelo/Usuario.cpp
    Modelo/Administrador.h
    Modelo/Administrador.cpp
    Modelo/UsuarioRegular.h
    Modelo/UsuarioRegular.cpp
    Controlador/UserFactory.h
    Controlador/UserFactory.cpp
    Controlador/AutenticacionController.h
    Controlador/AutenticacionController.cpp
    Controlador/Repository.h
    Controlador/LibroRepository.h
    Controlador/LibraTechException.h
    Modelo/RegistroHistorial.h
    Modelo/HistorialListModel.h
    Controlador/HistorialController.h
    Controlador/RecomendacionController.cpp
    Controlador/RecomendacionController.h

)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${MODELO_DIR}
    ${CONTROLADOR_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt::Core
        Qt::Quick
        Qt::Sql
        Qt::QuickControls2
        Qt::Concurrent
)

file(GLOB_RECURSE PROJECT_HEADERS
    "${MODELO_DIR}/*.h"
    "${CONTROLADOR_DIR}/*.h"
)

set_source_files_properties(${PROJECT_HEADERS} PROPERTIES HEADER_FILE_ONLY TRUE)
source_group("Header Files" FILES ${PROJECT_HEADERS})

# ========================================
# PRUEBAS UNITARIAS
# ========================================
enable_testing()

# Test 1: TestLibro
add_executable(TestLibro
    tests/TestLibro.cpp
    ${MODELO_DIR}/Libro.cpp
)

target_link_libraries(TestLibro
    PRIVATE
        Qt::Core
        Qt::Test
)

target_include_directories(TestLibro PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${MODELO_DIR}
)

add_test(NAME TestLibro COMMAND TestLibro)

# Test 2: TestLibroRepository
add_executable(TestLibroRepository
    tests/TestLibroRepository.cpp
    ${MODELO_DIR}/Libro.cpp
    ${CONTROLADOR_DIR}/SingletonDB.cpp
)

target_link_libraries(TestLibroRepository
    PRIVATE
        Qt::Core
        Qt::Test
        Qt::Sql
)

target_include_directories(TestLibroRepository PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${MODELO_DIR}
    ${CONTROLADOR_DIR}
)

add_test(NAME TestLibroRepository COMMAND TestLibroRepository)
