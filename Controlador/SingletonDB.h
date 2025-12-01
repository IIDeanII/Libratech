#ifndef SINGLETONDB_H
#define SINGLETONDB_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

class SingletonDB {
private:
    static SingletonDB* m_instance;
    QSqlDatabase m_db;

    // Constructor y Destructor privados para el Singleton
    SingletonDB();
    ~SingletonDB();

public:
    // Prevenir copia y asignación
    SingletonDB(const SingletonDB&) = delete;
    SingletonDB& operator=(const SingletonDB&) = delete;

    // Métodos de acceso
    static SingletonDB& getInstance();
    bool inicializar();

    // **CORRECCIÓN** (Solo declaración, sin el cuerpo inline):
    QSqlDatabase getDatabase();
};

#endif // SINGLETONDB_H
