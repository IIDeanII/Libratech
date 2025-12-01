#include "SingletonDB.h"
#include <QDebug>
#include <QCoreApplication>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

// Inicializar el puntero estático a nullptr
SingletonDB* SingletonDB::m_instance = nullptr;

// Constructor (Ejecución Lazy)
SingletonDB::SingletonDB()
{
    // Si ya existe una conexión (ej. en tests)
    if (QSqlDatabase::contains(QSqlDatabase::defaultConnection)) {
        m_db = QSqlDatabase::database();
        return;
    }

    // Si no existe (producción)
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    QString dbPath = QCoreApplication::applicationDirPath() + "/biblioteca.sqlite";
    m_db.setDatabaseName(dbPath);
}

// Destructor
SingletonDB::~SingletonDB() {
    if (m_db.isOpen()) {
        m_db.close();
    }
}

// Método de acceso al Singleton
SingletonDB& SingletonDB::getInstance() {
    if (m_instance == nullptr) {
        m_instance = new SingletonDB();
    }
    return *m_instance;
}

// Lógica de inicialización: Abre la DB y crea las tablas si no existen
bool SingletonDB::inicializar() {
    if (!m_db.isValid()) {
        qCritical() << "Error: Driver SQLITE no válido.";
        return false;
    }

    if (!m_db.open()) {
        qCritical() << "Error al abrir la base de datos:" << m_db.lastError().text();
        return false;
    }

    QSqlQuery query(m_db);

    // ========================================
    // TABLA LIBROS
    // ========================================
    QString createLibrosTable =
        "CREATE TABLE IF NOT EXISTS Libros ("
        "id TEXT PRIMARY KEY,"
        "titulo TEXT NOT NULL,"
        "autor TEXT NOT NULL,"
        "anio INTEGER,"
        "genero TEXT,"
        "imagenPath TEXT,"
        "prestado INTEGER NOT NULL DEFAULT 0,"
        "fechaPrestamo TEXT,"
        "fechaDevolucionEsperada TEXT,"
        "fechaDevolucionReal TEXT"
        ");";

    if (!query.exec(createLibrosTable)) {
        qCritical() << "Error al crear la tabla Libros:" << query.lastError().text();
        return false;
    }

    // Migración de columnas (para DBs existentes)
    QSqlQuery checkColumns(m_db);
    checkColumns.exec("PRAGMA table_info(Libros)");

    bool tieneFechaPrestamo = false;
    bool tieneFechaDevolucionEsperada = false;
    bool tieneFechaDevolucionReal = false;

    while (checkColumns.next()) {
        QString columnName = checkColumns.value(1).toString();
        if (columnName == "fechaPrestamo") tieneFechaPrestamo = true;
        if (columnName == "fechaDevolucionEsperada") tieneFechaDevolucionEsperada = true;
        if (columnName == "fechaDevolucionReal") tieneFechaDevolucionReal = true;
    }

    if (!tieneFechaPrestamo) {
        if (!query.exec("ALTER TABLE Libros ADD COLUMN fechaPrestamo TEXT")) {
            qWarning() << "No se pudo agregar columna fechaPrestamo:" << query.lastError().text();
        } else {
            qDebug() << "Columna 'fechaPrestamo' agregada exitosamente";
        }
    }

    if (!tieneFechaDevolucionEsperada) {
        if (!query.exec("ALTER TABLE Libros ADD COLUMN fechaDevolucionEsperada TEXT")) {
            qWarning() << "No se pudo agregar columna fechaDevolucionEsperada:" << query.lastError().text();
        } else {
            qDebug() << "Columna 'fechaDevolucionEsperada' agregada exitosamente";
        }
    }

    if (!tieneFechaDevolucionReal) {
        if (!query.exec("ALTER TABLE Libros ADD COLUMN fechaDevolucionReal TEXT")) {
            qWarning() << "No se pudo agregar columna fechaDevolucionReal:" << query.lastError().text();
        } else {
            qDebug() << "Columna 'fechaDevolucionReal' agregada exitosamente";
        }
    }

    // ========================================
    // 👈 NUEVA TABLA: HISTORIAL DE PRÉSTAMOS
    // ========================================
    QString createHistorialTable =
        "CREATE TABLE IF NOT EXISTS Historial ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "libroId TEXT NOT NULL,"
        "libroTitulo TEXT NOT NULL,"
        "libroAutor TEXT NOT NULL,"
        "accion TEXT NOT NULL,"  // 'PRESTAMO' o 'DEVOLUCION'
        "fecha TEXT NOT NULL,"    // Fecha de la acción
        "usuarioNombre TEXT,"     // Nombre del usuario que realizó la acción
        "observaciones TEXT,"     // Campo opcional para notas
        "FOREIGN KEY (libroId) REFERENCES Libros(id)"
        ");";

    if (!query.exec(createHistorialTable)) {
        qCritical() << "Error al crear la tabla Historial:" << query.lastError().text();
        return false;
    }

    qDebug() << "Base de datos inicializada correctamente (Libros + Historial)";
    return true;
}

// Implementación de getDatabase()
QSqlDatabase SingletonDB::getDatabase()
{
    // Si la conexión aún no está abierta, intentar abrirla.
    if (!m_db.isOpen()) {
        if (!m_db.open()) {
            qCritical() << "Error al abrir la base de datos:" << m_db.lastError().text();
        }
    }
    return m_db;
}
