#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QList>
#include <QDebug>
#include <functional>
#include "Controlador/SingletonDB.h"

/**
 * @brief Clase Template genérica para operaciones CRUD en base de datos
 * @tparam T Tipo de entidad a gestionar
 *
 * DEMUESTRA: Uso de templates/genéricos (requisito del curso)
 *
 * Proporciona métodos abstractos para:
 * - Guardar entidades (INSERT/UPDATE)
 * - Buscar por ID
 * - Listar todas las entidades
 * - Eliminar por ID
 * - Contar registros
 */
template<typename T>
class Repository
{
public:
    // Constructor: Recibe el nombre de la tabla
    explicit Repository(const QString &tableName)
        : m_tableName(tableName)
    {
        m_db = SingletonDB::getInstance().getDatabase();
    }

    virtual ~Repository() = default;

    // ========================================
    // MÉTODOS CRUD GENÉRICOS
    // ========================================

    /**
     * @brief Cuenta el número de registros en la tabla
     * @return Número de registros, o -1 si hay error
     */
    int count() const
    {
        QSqlQuery query(m_db);
        QString sql = QString("SELECT COUNT(*) FROM %1").arg(m_tableName);

        if (!query.exec(sql)) {
            qCritical() << "Error al contar registros en" << m_tableName << ":" << query.lastError().text();
            return -1;
        }

        if (query.next()) {
            return query.value(0).toInt();
        }

        return 0;
    }

    /**
     * @brief Verifica si existe un registro con el ID dado
     * @param id Identificador único
     * @return true si existe, false en caso contrario
     */
    bool exists(const QUuid &id) const
    {
        QSqlQuery query(m_db);
        query.prepare(QString("SELECT COUNT(*) FROM %1 WHERE id = :id").arg(m_tableName));
        query.bindValue(":id", id.toString());

        if (!query.exec()) {
            qCritical() << "Error al verificar existencia en" << m_tableName << ":" << query.lastError().text();
            return false;
        }

        if (query.next()) {
            return query.value(0).toInt() > 0;
        }

        return false;
    }

    /**
     * @brief Elimina un registro por ID
     * @param id Identificador único
     * @return true si se eliminó correctamente, false en caso contrario
     */
    bool deleteById(const QUuid &id)
    {
        QSqlQuery query(m_db);
        query.prepare(QString("DELETE FROM %1 WHERE id = :id").arg(m_tableName));
        query.bindValue(":id", id.toString());

        if (!query.exec()) {
            qCritical() << "Error al eliminar registro de" << m_tableName << ":" << query.lastError().text();
            return false;
        }

        qDebug() << "Registro eliminado de" << m_tableName << "con ID:" << id.toString();
        return true;
    }

    /**
     * @brief Ejecuta una consulta personalizada
     * @param sql Consulta SQL
     * @param bindings Mapa de parámetros para binding (opcional)
     * @return true si se ejecutó correctamente
     */
    bool executeQuery(const QString &sql, const QMap<QString, QVariant> &bindings = {})
    {
        QSqlQuery query(m_db);
        query.prepare(sql);

        // Bind de parámetros
        for (auto it = bindings.constBegin(); it != bindings.constEnd(); ++it) {
            query.bindValue(it.key(), it.value());
        }

        if (!query.exec()) {
            qCritical() << "Error al ejecutar consulta:" << query.lastError().text();
            return false;
        }

        return true;
    }

    // ========================================
    // MÉTODOS ABSTRACTOS (para implementar en subclases)
    // ========================================

    /**
     * @brief Guarda una entidad en la base de datos
     * Debe ser implementado por las subclases específicas
     */
    virtual bool save(const T &entity) = 0;

    /**
     * @brief Busca una entidad por ID
     * Debe ser implementado por las subclases específicas
     */
    virtual T findById(const QUuid &id) const = 0;

    /**
     * @brief Lista todas las entidades
     * Debe ser implementado por las subclases específicas
     */
    virtual QList<T> findAll() const = 0;

protected:
    QString m_tableName;
    QSqlDatabase m_db;
};

#endif // REPOSITORY_H
