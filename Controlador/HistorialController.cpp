#include "HistorialController.h"
#include "Controlador/SingletonDB.h"
#include "Controlador/LibraTechException.h"
#include "Modelo/RegistroHistorial.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

HistorialController::HistorialController(QObject *parent)
    : QObject(parent),
    m_historialModel(new HistorialListModel(this))
{
}

void HistorialController::registrarAccion(const QString &libroId, const QString &libroTitulo,
                                          const QString &libroAutor, const QString &accion,
                                          const QString &usuarioNombre)
{
    try {
        QSqlDatabase db = SingletonDB::getInstance().getDatabase();
        QSqlQuery query(db);

        query.prepare("INSERT INTO Historial (libroId, libroTitulo, libroAutor, accion, fecha, usuarioNombre) "
                      "VALUES (:libroId, :libroTitulo, :libroAutor, :accion, :fecha, :usuarioNombre)");

        query.bindValue(":libroId", libroId);
        query.bindValue(":libroTitulo", libroTitulo);
        query.bindValue(":libroAutor", libroAutor);
        query.bindValue(":accion", accion);
        query.bindValue(":fecha", QDateTime::currentDateTime().toString(Qt::ISODate));
        query.bindValue(":usuarioNombre", usuarioNombre);

        if (!query.exec()) {
            throw DatabaseException("Error al registrar acción en historial", query.lastError().text());
        }

        // Agregar al modelo inmediatamente
        int lastId = query.lastInsertId().toInt();
        RegistroHistorialPtr registro(new RegistroHistorial(
            lastId, libroId, libroTitulo, libroAutor, accion,
            QDateTime::currentDateTime(), usuarioNombre
            ));
        m_historialModel->appendRegistro(registro);

        qDebug() << "Acción registrada en historial:" << accion << "- Libro:" << libroTitulo;

    } catch (const DatabaseException &e) {
        qCritical() << e.mensajeCompleto();
    }
}

void HistorialController::registrarPrestamo(const QString &libroId, const QString &libroTitulo,
                                            const QString &libroAutor, const QString &usuarioNombre)
{
    registrarAccion(libroId, libroTitulo, libroAutor, "PRESTAMO", usuarioNombre);
}

void HistorialController::registrarDevolucion(const QString &libroId, const QString &libroTitulo,
                                              const QString &libroAutor, const QString &usuarioNombre)
{
    registrarAccion(libroId, libroTitulo, libroAutor, "DEVOLUCION", usuarioNombre);
}

void HistorialController::cargarHistorial()
{
    try {
        m_historialModel->clear();

        QSqlDatabase db = SingletonDB::getInstance().getDatabase();
        QSqlQuery query("SELECT id, libroId, libroTitulo, libroAutor, accion, fecha, usuarioNombre, observaciones "
                        "FROM Historial ORDER BY fecha DESC", db);

        if (!query.exec()) {
            throw DatabaseException("Error al cargar historial", query.lastError().text());
        }

        while (query.next()) {
            int id = query.value("id").toInt();
            QString libroId = query.value("libroId").toString();
            QString libroTitulo = query.value("libroTitulo").toString();
            QString libroAutor = query.value("libroAutor").toString();
            QString accion = query.value("accion").toString();
            QDateTime fecha = QDateTime::fromString(query.value("fecha").toString(), Qt::ISODate);
            QString usuarioNombre = query.value("usuarioNombre").toString();
            QString observaciones = query.value("observaciones").toString();

            RegistroHistorialPtr registro(new RegistroHistorial(
                id, libroId, libroTitulo, libroAutor, accion, fecha, usuarioNombre, observaciones
                ));
            m_historialModel->appendRegistro(registro);
        }

        qDebug() << "Historial cargado. Total de registros:" << m_historialModel->rowCount();

    } catch (const DatabaseException &e) {
        qCritical() << e.mensajeCompleto();
    }
}

void HistorialController::limpiarHistorial()
{
    try {
        QSqlDatabase db = SingletonDB::getInstance().getDatabase();
        QSqlQuery query(db);

        if (!query.exec("DELETE FROM Historial")) {
            throw DatabaseException("Error al limpiar historial", query.lastError().text());
        }

        m_historialModel->clear();
        qDebug() << "Historial limpiado correctamente";

    } catch (const DatabaseException &e) {
        qCritical() << e.mensajeCompleto();
    }
}
