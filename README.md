# LibraTech  
Sistema de Gestión de Biblioteca Virtual

## Descripción
**LibraTech** es un sistema de gestión de biblioteca virtual desarrollado en **C++17** utilizando el **framework Qt 6**. El proyecto aplica una arquitectura **Modelo–Vista–Controlador (MVC)** y patrones de diseño orientados a mejorar la mantenibilidad, extensibilidad y organización del código.  
Está orientado a fines **académicos** y demuestra la integración entre lógica de negocio en C++ e interfaces gráficas declarativas en QML.

## Funcionalidades
- Gestión de libros (registro, consulta y estado).
- Préstamos y devoluciones con control de fechas.
- Registro de historial de operaciones.
- Sistema básico de recomendaciones.
- Interfaz gráfica moderna desarrollada en QML.
- Persistencia de datos mediante SQLite.
- Ejecución de tareas en segundo plano con Qt Concurrent.

## Arquitectura
El sistema sigue el patrón **MVC**:
- **Modelo**: entidades de dominio y acceso a datos mediante repositorios.
- **Vista**: interfaz gráfica en QML, sin lógica de negocio.
- **Controlador**: coordinación entre vista y modelo, manejo de eventos y operaciones.

## Tecnologías Utilizadas
- **Lenguaje**: C++17  
- **Framework**: Qt 6 (Qt Core, Qt Quick, Qt SQL, Qt Concurrent)  
- **Base de datos**: SQLite  
- **Build system**: CMake  
- **IDE**: Qt Creator  

## Compilación
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Estado del Proyecto
Proyecto funcional y completo en el contexto académico. No está orientado a uso en producción sin extensiones adicionales (seguridad, backend, sincronización).

## Licencia
Licencia MIT.
