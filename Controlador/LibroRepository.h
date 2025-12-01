#ifndef LIBROREPOSITORY_H
#define LIBROREPOSITORY_H

#include "Repository.h"
#include "LibraTechException.h"
#include "Modelo/Libro.h"
#include <QUuid>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>

/**
 * @brief Implementación concreta del Repository para la entidad Libro
 *
 * DEMUESTRA:
 * - Uso de templates/genéricos (Repository<LibroPtr>)
 * - Especialización de clase template
 * - Manejo de errores con excepciones personalizadas
 * - Persistencia de fechas de préstamo
 */
class LibroRepository : public Repository<LibroPtr>
{
public:
    LibroRepository() : Repository<LibroPtr>("Libros") {}

    /**
     * @brief Guarda un libro en la base de datos (INSERT o UPDATE)
     * @param libro Puntero al libro a guardar
     * @return true si se guardó correctamente
     * @throws DatabaseException si hay error en la operación
     * @throws ValidationException si el libro es nulo
     */
    bool save(const LibroPtr &libro) override
    {
        if (!libro) {
            throw ValidationException("libro", "El puntero al libro no puede ser nulo");
        }

        // Validar datos del libro
        if (libro->titulo().trimmed().isEmpty()) {
            throw ValidationException("titulo", "El título no puede estar vacío");
        }
        if (libro->autor().trimmed().isEmpty()) {
            throw ValidationException("autor", "El autor no puede estar vacío");
        }

        QSqlQuery query(m_db);

        // Si el libro ya existe, hacemos UPDATE, sino INSERT
        if (exists(libro->id())) {
            query.prepare("UPDATE Libros SET titulo = :titulo, autor = :autor, anio = :anio, "
                          "genero = :genero, imagenPath = :imagenPath, prestado = :prestado, "
                          "fechaPrestamo = :fechaPrestamo, fechaDevolucionEsperada = :fechaDevolucionEsperada, "
                          "fechaDevolucionReal = :fechaDevolucionReal "
                          "WHERE id = :id");
        } else {
            query.prepare("INSERT INTO Libros (id, titulo, autor, anio, genero, imagenPath, prestado, "
                          "fechaPrestamo, fechaDevolucionEsperada, fechaDevolucionReal) "
                          "VALUES (:id, :titulo, :autor, :anio, :genero, :imagenPath, :prestado, "
                          ":fechaPrestamo, :fechaDevolucionEsperada, :fechaDevolucionReal)");
        }

        query.bindValue(":id", libro->id().toString());
        query.bindValue(":titulo", libro->titulo());
        query.bindValue(":autor", libro->autor());
        query.bindValue(":anio", libro->anioPublicacion());
        query.bindValue(":genero", libro->genero());
        query.bindValue(":imagenPath", libro->imagenPath());
        query.bindValue(":prestado", libro->prestado() ? 1 : 0);

        // 👈 GUARDAR FECHAS (ISO 8601 format)
        query.bindValue(":fechaPrestamo",
                        libro->fechaPrestamo().isValid() ? libro->fechaPrestamo().toString(Qt::ISODate) : QVariant());
        query.bindValue(":fechaDevolucionEsperada",
                        libro->fechaDevolucionEsperada().isValid() ? libro->fechaDevolucionEsperada().toString(Qt::ISODate) : QVariant());
        query.bindValue(":fechaDevolucionReal",
                        libro->fechaDevolucionReal().isValid() ? libro->fechaDevolucionReal().toString(Qt::ISODate) : QVariant());

        if (!query.exec()) {
            throw DatabaseException("Error al guardar libro", query.lastError().text());
        }

        qDebug() << "Libro guardado exitosamente:" << libro->titulo();
        return true;
    }

    /**
     * @brief Busca un libro por ID
     * @param id Identificador único del libro
     * @return Puntero al libro encontrado
     * @throws NotFoundException si el libro no existe
     * @throws DatabaseException si hay error en la consulta
     */
    LibroPtr findById(const QUuid &id) const override
    {
        QSqlQuery query(m_db);
        query.prepare("SELECT id, titulo, autor, anio, genero, imagenPath, prestado, "
                      "fechaPrestamo, fechaDevolucionEsperada, fechaDevolucionReal "
                      "FROM Libros WHERE id = :id");
        query.bindValue(":id", id.toString());

        if (!query.exec()) {
            throw DatabaseException("Error al buscar libro por ID", query.lastError().text());
        }

        if (query.next()) {
            return crearLibroDesdeQuery(query);
        }

        throw NotFoundException("Libro", id.toString());
    }

    /**
     * @brief Lista todos los libros
     * @return Lista de punteros a libros
     * @throws DatabaseException si hay error en la consulta
     */
    QList<LibroPtr> findAll() const override
    {
        QList<LibroPtr> libros;
        QSqlQuery query("SELECT id, titulo, autor, anio, genero, imagenPath, prestado, "
                        "fechaPrestamo, fechaDevolucionEsperada, fechaDevolucionReal "
                        "FROM Libros", m_db);

        if (!query.exec()) {
            throw DatabaseException("Error al listar todos los libros", query.lastError().text());
        }

        while (query.next()) {
            try {
                LibroPtr libro = crearLibroDesdeQuery(query);
                if (libro) {
                    libros.append(libro);
                }
            } catch (const LibraTechException &e) {
                qWarning() << "Error al crear libro desde query:" << e.mensajeCompleto();
            }
        }

        qDebug() << "Libros encontrados:" << libros.size();
        return libros;
    }

    /**
     * @brief Busca libros por género
     * @param genero Género a buscar
     * @return Lista de libros del género especificado
     * @throws DatabaseException si hay error en la consulta
     * @throws ValidationException si el género está vacío
     */
    QList<LibroPtr> findByGenero(const QString &genero) const
    {
        if (genero.trimmed().isEmpty()) {
            throw ValidationException("genero", "El género no puede estar vacío");
        }

        QList<LibroPtr> libros;
        QSqlQuery query(m_db);
        query.prepare("SELECT id, titulo, autor, anio, genero, imagenPath, prestado, "
                      "fechaPrestamo, fechaDevolucionEsperada, fechaDevolucionReal "
                      "FROM Libros WHERE genero = :genero");
        query.bindValue(":genero", genero);

        if (!query.exec()) {
            throw DatabaseException("Error al buscar libros por género", query.lastError().text());
        }

        while (query.next()) {
            try {
                LibroPtr libro = crearLibroDesdeQuery(query);
                if (libro) {
                    libros.append(libro);
                }
            } catch (const LibraTechException &e) {
                qWarning() << "Error al crear libro desde query:" << e.mensajeCompleto();
            }
        }

        return libros;
    }

    /**
     * @brief Busca libros prestados actualmente
     * @return Lista de libros prestados
     */
    QList<LibroPtr> findPrestados() const
    {
        QList<LibroPtr> libros;
        QSqlQuery query("SELECT id, titulo, autor, anio, genero, imagenPath, prestado, "
                        "fechaPrestamo, fechaDevolucionEsperada, fechaDevolucionReal "
                        "FROM Libros WHERE prestado = 1", m_db);

        if (!query.exec()) {
            throw DatabaseException("Error al buscar libros prestados", query.lastError().text());
        }

        while (query.next()) {
            try {
                LibroPtr libro = crearLibroDesdeQuery(query);
                if (libro) {
                    libros.append(libro);
                }
            } catch (const LibraTechException &e) {
                qWarning() << "Error al crear libro desde query:" << e.mensajeCompleto();
            }
        }

        return libros;
    }

private:
    /**
     * @brief Crea un objeto Libro desde un QSqlQuery (con fechas)
     * @param query Query con los datos del libro
     * @return Puntero al libro creado
     */
    LibroPtr crearLibroDesdeQuery(const QSqlQuery &query) const
    {
        QUuid id = QUuid(query.value("id").toString());
        QString titulo = query.value("titulo").toString();
        QString autor = query.value("autor").toString();
        int anio = query.value("anio").toInt();
        QString genero = query.value("genero").toString();
        QString imagenPath = query.value("imagenPath").toString();
        bool prestado = query.value("prestado").toBool();

        // 👈 CARGAR FECHAS desde la DB
        QDateTime fechaPrestamo = QDateTime::fromString(query.value("fechaPrestamo").toString(), Qt::ISODate);
        QDateTime fechaDevolucionEsperada = QDateTime::fromString(query.value("fechaDevolucionEsperada").toString(), Qt::ISODate);
        QDateTime fechaDevolucionReal = QDateTime::fromString(query.value("fechaDevolucionReal").toString(), Qt::ISODate);

        return LibroPtr(new Libro(id, titulo, autor, anio, genero, imagenPath, prestado,
                                  fechaPrestamo, fechaDevolucionEsperada, fechaDevolucionReal));
    }
};

#endif // LIBROREPOSITORY_H
