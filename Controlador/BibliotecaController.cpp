#include "BibliotecaController.h"
#include "Controlador/SingletonDB.h"
#include "Controlador/LibroRepository.h"
#include "Controlador/LibraTechException.h"
#include "Controlador/HistorialController.h"
#include "Modelo/Libro.h"
#include "Controlador/BibliotecaFilterModel.h"
#include <QDebug>
#include <QUuid>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QDateTime>
#include <QtConcurrent/QtConcurrent>
#include <QUrl>

#define QRC_PATH_PREFIX "qrc:/Vista/images/"

BibliotecaController::BibliotecaController(QObject *parent)
    : QObject(parent),
    m_bibliotecaModel(new BibliotecaListModel(this)),
    m_bibliotecaFilterModel(new BibliotecaFilterModel(this)),
    m_isLoading(false),
    m_futureWatcher(new QFutureWatcher<QList<LibroPtr>>(this)),
    m_historialController(nullptr)
{
    m_bibliotecaFilterModel->setSourceModel(m_bibliotecaModel);
    connect(m_futureWatcher, &QFutureWatcher<QList<LibroPtr>>::finished,
            this, &BibliotecaController::onCargaLibrosCompletada);
}

BibliotecaFilterModel* BibliotecaController::bibliotecaFilterModel() const
{
    return m_bibliotecaFilterModel;
}

void BibliotecaController::setHistorialController(HistorialController *historialController)
{
    m_historialController = historialController;
}

bool BibliotecaController::agregarLibroDesdeUI(const QString &titulo, const QString &autor,
                                               int anio, const QString &genero, const QString &imagenUrl)
{
    if (titulo.trimmed().isEmpty()) {
        emit libroAgregado(false, "El título no puede estar vacío");
        return false;
    }
    if (autor.trimmed().isEmpty()) {
        emit libroAgregado(false, "El autor no puede estar vacío");
        return false;
    }
    if (anio < 1000 || anio > 2100) {
        emit libroAgregado(false, "El año debe estar entre 1000 y 2100");
        return false;
    }

    QString imagenPath = imagenUrl.trimmed().isEmpty() ?
                             QRC_PATH_PREFIX "default.png" : imagenUrl;

    if (imagenPath.startsWith("file://")) {
        QUrl url(imagenPath);
        imagenPath = url.toLocalFile();
    }

    try {
        agregarLibro(titulo.trimmed(), autor.trimmed(), anio, genero.trimmed(), imagenPath);
        emit libroAgregado(true, "Libro agregado exitosamente");
        return true;
    } catch (...) {
        emit libroAgregado(false, "Error al agregar el libro");
        return false;
    }
}

// 👈 MÉTODO ELIMINAR LIBRO
bool BibliotecaController::eliminarLibro(const QUuid &id)
{
    try {
        QSqlDatabase db = SingletonDB::getInstance().getDatabase();

        QSqlQuery selectQuery(db);
        selectQuery.prepare("SELECT titulo, autor FROM Libros WHERE id = :id");
        selectQuery.bindValue(":id", id.toString());

        QString titulo, autor;
        if (selectQuery.exec() && selectQuery.next()) {
            titulo = selectQuery.value("titulo").toString();
            autor = selectQuery.value("autor").toString();
        } else {
            qWarning() << "Libro no encontrado con ID:" << id.toString();
            return false;
        }

        QSqlQuery deleteQuery(db);
        deleteQuery.prepare("DELETE FROM Libros WHERE id = :id");
        deleteQuery.bindValue(":id", id.toString());

        if (!deleteQuery.exec()) {
            throw DatabaseException("Error al eliminar libro", deleteQuery.lastError().text());
        }

        m_bibliotecaModel->eliminarLibroPorId(id);

        qDebug() << "Libro eliminado exitosamente:" << titulo;
        return true;

    } catch (const DatabaseException &e) {
        qCritical() << e.mensajeCompleto();
        return false;
    }
}

void BibliotecaController::actualizarFechasPrestamo(const QUuid &id, bool esPrestamo)
{
    try {
        QSqlDatabase db = SingletonDB::getInstance().getDatabase();
        QSqlQuery query(db);

        if (esPrestamo) {
            QDateTime ahora = QDateTime::currentDateTime();
            QDateTime fechaDevolucion = ahora.addDays(DIAS_PRESTAMO_DEFAULT);

            query.prepare("UPDATE Libros SET "
                          "fechaPrestamo = :fechaPrestamo, "
                          "fechaDevolucionEsperada = :fechaDevolucionEsperada, "
                          "fechaDevolucionReal = NULL "
                          "WHERE id = :id");
            query.bindValue(":fechaPrestamo", ahora.toString(Qt::ISODate));
            query.bindValue(":fechaDevolucionEsperada", fechaDevolucion.toString(Qt::ISODate));
            query.bindValue(":id", id.toString());

            if (!query.exec()) {
                throw DatabaseException("Error al actualizar fechas de préstamo", query.lastError().text());
            }

            m_bibliotecaModel->actualizarFechas(id, ahora, fechaDevolucion, QDateTime());

            if (m_historialController) {
                QSqlQuery selectQuery(db);
                selectQuery.prepare("SELECT titulo, autor FROM Libros WHERE id = :id");
                selectQuery.bindValue(":id", id.toString());
                if (selectQuery.exec() && selectQuery.next()) {
                    QString titulo = selectQuery.value("titulo").toString();
                    QString autor = selectQuery.value("autor").toString();
                    m_historialController->registrarPrestamo(id.toString(), titulo, autor, "Sistema");
                }
            }

            qDebug() << "Préstamo registrado. Fecha:" << ahora.toString("dd/MM/yyyy");

        } else {
            QDateTime ahora = QDateTime::currentDateTime();

            query.prepare("UPDATE Libros SET fechaDevolucionReal = :fechaDevolucionReal WHERE id = :id");
            query.bindValue(":fechaDevolucionReal", ahora.toString(Qt::ISODate));
            query.bindValue(":id", id.toString());

            if (!query.exec()) {
                throw DatabaseException("Error al actualizar fecha de devolución", query.lastError().text());
            }

            QSqlQuery selectQuery(db);
            selectQuery.prepare("SELECT fechaPrestamo, fechaDevolucionEsperada, titulo, autor FROM Libros WHERE id = :id");
            selectQuery.bindValue(":id", id.toString());

            if (selectQuery.exec() && selectQuery.next()) {
                QDateTime fechaPrestamo = QDateTime::fromString(selectQuery.value("fechaPrestamo").toString(), Qt::ISODate);
                QDateTime fechaDevEsperada = QDateTime::fromString(selectQuery.value("fechaDevolucionEsperada").toString(), Qt::ISODate);

                m_bibliotecaModel->actualizarFechas(id, fechaPrestamo, fechaDevEsperada, ahora);

                if (m_historialController) {
                    QString titulo = selectQuery.value("titulo").toString();
                    QString autor = selectQuery.value("autor").toString();
                    m_historialController->registrarDevolucion(id.toString(), titulo, autor, "Sistema");
                }
            }

            qDebug() << "Devolución registrada. Fecha:" << ahora.toString("dd/MM/yyyy");
        }

    } catch (const DatabaseException &e) {
        qCritical() << e.mensajeCompleto();
    }
}

void BibliotecaController::actualizarEstadoPrestamo(const QUuid &id, bool nuevoEstado)
{
    try {
        QSqlDatabase db = SingletonDB::getInstance().getDatabase();
        QSqlQuery query(db);
        query.prepare("UPDATE Libros SET prestado = :prestado WHERE id = :id");
        query.bindValue(":prestado", nuevoEstado ? 1 : 0);
        query.bindValue(":id", id.toString());

        if (!query.exec()) {
            throw DatabaseException("Error al actualizar estado de préstamo", query.lastError().text());
        }

        m_bibliotecaModel->setPrestado(id, nuevoEstado);
        actualizarFechasPrestamo(id, nuevoEstado);

        qDebug() << "Estado de préstamo actualizado correctamente";

    } catch (const DatabaseException &e) {
        qCritical() << e.mensajeCompleto();
    }
}

void BibliotecaController::realizarPrestamo(const QUuid &id)
{
    qDebug() << "Realizando Préstamo para el libro:" << id.toString();
    actualizarEstadoPrestamo(id, true);
}

void BibliotecaController::realizarDevolucion(const QUuid &id)
{
    qDebug() << "Realizando Devolución para el libro:" << id.toString();
    actualizarEstadoPrestamo(id, false);
}

void BibliotecaController::agregarLibro(const QString &titulo, const QString &autor, int anio, const QString &genero, const QString &imagenPath)
{
    try {
        QUuid id = QUuid::createUuid();
        LibroPtr nuevoLibro(new Libro(id, titulo, autor, anio, genero, imagenPath, false));

        LibroRepository repo;
        if (repo.save(nuevoLibro)) {
            m_bibliotecaModel->appendLibro(nuevoLibro);
            qDebug() << "Libro agregado exitosamente:" << titulo;
        }

    } catch (const ValidationException &e) {
        qCritical() << "Error de validación:" << e.mensajeCompleto();
        throw;
    } catch (const DatabaseException &e) {
        qCritical() << "Error de base de datos:" << e.mensajeCompleto();
        throw;
    } catch (const LibraTechException &e) {
        qCritical() << "Error:" << e.mensajeCompleto();
        throw;
    }
}

QList<LibroPtr> BibliotecaController::cargarLibrosDesdeDB()
{
    qDebug() << "[THREAD SECUNDARIO] Iniciando carga de libros desde DB...";

    QList<LibroPtr> libros;

    try {
        LibroRepository repo;
        libros = repo.findAll();
    } catch (const LibraTechException &e) {
        qCritical() << "[THREAD SECUNDARIO]" << e.mensajeCompleto();
    }

    qDebug() << "[THREAD SECUNDARIO] Carga completada. Total:" << libros.size();
    return libros;
}

void BibliotecaController::onCargaLibrosCompletada()
{
    qDebug() << "[THREAD PRINCIPAL] Recibiendo libros del thread secundario...";

    QList<LibroPtr> libros = m_futureWatcher->result();

    for (const LibroPtr &libro : libros) {
        m_bibliotecaModel->appendLibro(libro);
    }

    m_isLoading = false;
    emit isLoadingChanged();
    emit cargaCompletada(libros.size());

    qDebug() << "[THREAD PRINCIPAL] Carga completada. Total:" << m_bibliotecaModel->rowCount();
}

void BibliotecaController::inicializarDatos()
{
    try {
        LibroRepository libroRepo;
        qDebug() << "=== PRUEBA DE REPOSITORY TEMPLATE ===";
        qDebug() << "Total de libros en DB:" << libroRepo.count();

        QSqlDatabase db = SingletonDB::getInstance().getDatabase();

        if (!db.isOpen()) {
            throw DatabaseException("La base de datos no está abierta");
        }

        QSqlQuery countQuery(db);
        if (!countQuery.exec("SELECT COUNT(*) FROM Libros")) {
            throw DatabaseException("Error al contar libros", countQuery.lastError().text());
        }

        countQuery.next();
        int rowCount = countQuery.value(0).toInt();

        if (rowCount == 0) {
            qDebug() << "Base de datos vacía. Insertando datos de ejemplo...";

            agregarLibro("Cien Años de Soledad", "Gabriel García Márquez", 1967, "Realismo Mágico", QRC_PATH_PREFIX "cientosoledad.jpg");
            agregarLibro("El Señor de los Anillos", "J.R.R. Tolkien", 1954, "Fantasía Épica", QRC_PATH_PREFIX "sr-anillos.jpg");
            agregarLibro("1984", "George Orwell", 1949, "Distopía", QRC_PATH_PREFIX "1984.jpg");

            qDebug() << "Inserción completada. Total:" << m_bibliotecaModel->rowCount();

        } else {
            qDebug() << "=== INICIANDO CARGA ASÍNCRONA ===";

            m_isLoading = true;
            emit isLoadingChanged();

            QFuture<QList<LibroPtr>> future = QtConcurrent::run(cargarLibrosDesdeDB);
            m_futureWatcher->setFuture(future);

            qDebug() << "Carga asíncrona iniciada...";
        }

    } catch (const LibraTechException &e) {
        qCritical() << e.mensajeCompleto();
    }
}
