#include "RecomendacionController.h"
#include "Controlador/SingletonDB.h"
#include "Controlador/LibraTechException.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QSet>

RecomendacionController::RecomendacionController(QObject *parent)
    : QObject(parent),
    m_recomendacionesModel(new BibliotecaListModel(this)),
    m_bibliotecaModelRef(nullptr)
{
}

void RecomendacionController::setBibliotecaModel(BibliotecaListModel *model)
{
    m_bibliotecaModelRef = model;
}

QStringList RecomendacionController::obtenerGenerosPreferidos(const QString &usuarioNombre)
{
    QStringList generos;

    try {
        QSqlDatabase db = SingletonDB::getInstance().getDatabase();
        QSqlQuery query(db);

        // Obtener géneros de libros que el usuario ha prestado (del historial)
        query.prepare("SELECT DISTINCT l.genero "
                      "FROM Historial h "
                      "JOIN Libros l ON h.libroId = l.id "
                      "WHERE h.usuarioNombre = :usuario AND h.accion = 'PRESTAMO' "
                      "ORDER BY h.fecha DESC "
                      "LIMIT 5");
        query.bindValue(":usuario", usuarioNombre);

        if (!query.exec()) {
            throw DatabaseException("Error al obtener géneros preferidos", query.lastError().text());
        }

        while (query.next()) {
            QString genero = query.value("genero").toString();
            if (!genero.isEmpty() && !generos.contains(genero)) {
                generos.append(genero);
            }
        }

    } catch (const LibraTechException &e) {
        qCritical() << e.mensajeCompleto();
    }

    return generos;
}

QStringList RecomendacionController::obtenerAutoresPreferidos(const QString &usuarioNombre)
{
    QStringList autores;

    try {
        QSqlDatabase db = SingletonDB::getInstance().getDatabase();
        QSqlQuery query(db);

        // Obtener autores de libros que el usuario ha prestado
        query.prepare("SELECT DISTINCT l.autor "
                      "FROM Historial h "
                      "JOIN Libros l ON h.libroId = l.id "
                      "WHERE h.usuarioNombre = :usuario AND h.accion = 'PRESTAMO' "
                      "ORDER BY h.fecha DESC "
                      "LIMIT 3");
        query.bindValue(":usuario", usuarioNombre);

        if (!query.exec()) {
            throw DatabaseException("Error al obtener autores preferidos", query.lastError().text());
        }

        while (query.next()) {
            QString autor = query.value("autor").toString();
            if (!autor.isEmpty() && !autores.contains(autor)) {
                autores.append(autor);
            }
        }

    } catch (const LibraTechException &e) {
        qCritical() << e.mensajeCompleto();
    }

    return autores;
}

QList<LibroPtr> RecomendacionController::filtrarLibrosDisponibles(const QList<LibroPtr> &libros)
{
    QList<LibroPtr> disponibles;

    for (const LibroPtr &libro : libros) {
        if (!libro->prestado()) {
            disponibles.append(libro);
        }
    }

    return disponibles;
}

void RecomendacionController::generarRecomendaciones(const QString &usuarioNombre)
{
    // Limpiar recomendaciones previas
    while (m_recomendacionesModel->rowCount() > 0) {
        m_recomendacionesModel->eliminarLibroPorId(
            m_recomendacionesModel->data(m_recomendacionesModel->index(0, 0),
                                         BibliotecaListModel::IdRole).value<QUuid>()
            );
    }

    if (!m_bibliotecaModelRef) {
        qWarning() << "No hay referencia al modelo de biblioteca";
        return;
    }

    // Obtener preferencias del usuario
    QStringList generosPreferidos = obtenerGenerosPreferidos(usuarioNombre);
    QStringList autoresPreferidos = obtenerAutoresPreferidos(usuarioNombre);

    qDebug() << "Géneros preferidos:" << generosPreferidos;
    qDebug() << "Autores preferidos:" << autoresPreferidos;

    // Si no hay historial, recomendar libros disponibles aleatorios
    if (generosPreferidos.isEmpty() && autoresPreferidos.isEmpty()) {
        qDebug() << "No hay historial, recomendando libros disponibles...";

        QList<LibroPtr> todosLibros = m_bibliotecaModelRef->getLibros([](const LibroPtr&) { return true; });
        QList<LibroPtr> disponibles = filtrarLibrosDisponibles(todosLibros);

        // Agregar hasta 5 libros disponibles
        int count = 0;
        for (const LibroPtr &libro : disponibles) {
            if (count >= 5) break;
            m_recomendacionesModel->appendLibro(libro);
            count++;
        }

        qDebug() << "Recomendaciones generadas (sin historial):" << count;
        return;
    }

    QSet<QUuid> librosYaRecomendados; // Para evitar duplicados

    // 1. RECOMENDAR POR GÉNERO
    for (const QString &genero : generosPreferidos) {
        QList<LibroPtr> librosPorGenero = m_bibliotecaModelRef->getLibros(
            [&genero](const LibroPtr &libro) {
                return libro->genero() == genero;
            }
            );

        QList<LibroPtr> disponibles = filtrarLibrosDisponibles(librosPorGenero);

        for (const LibroPtr &libro : disponibles) {
            if (!librosYaRecomendados.contains(libro->id())) {
                m_recomendacionesModel->appendLibro(libro);
                librosYaRecomendados.insert(libro->id());

                if (librosYaRecomendados.size() >= 10) break; // Máximo 10 recomendaciones
            }
        }

        if (librosYaRecomendados.size() >= 10) break;
    }

    // 2. RECOMENDAR POR AUTOR
    if (librosYaRecomendados.size() < 10) {
        for (const QString &autor : autoresPreferidos) {
            QList<LibroPtr> librosPorAutor = m_bibliotecaModelRef->getLibros(
                [&autor](const LibroPtr &libro) {
                    return libro->autor() == autor;
                }
                );

            QList<LibroPtr> disponibles = filtrarLibrosDisponibles(librosPorAutor);

            for (const LibroPtr &libro : disponibles) {
                if (!librosYaRecomendados.contains(libro->id())) {
                    m_recomendacionesModel->appendLibro(libro);
                    librosYaRecomendados.insert(libro->id());

                    if (librosYaRecomendados.size() >= 10) break;
                }
            }

            if (librosYaRecomendados.size() >= 10) break;
        }
    }

    qDebug() << "Recomendaciones generadas:" << librosYaRecomendados.size();
}
