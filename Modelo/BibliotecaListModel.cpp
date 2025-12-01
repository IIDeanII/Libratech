#include "BibliotecaListModel.h"
#include <QDebug>

BibliotecaListModel::BibliotecaListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int BibliotecaListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_libros.count();
}

QVariant BibliotecaListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_libros.count())
        return QVariant();

    const LibroPtr libro = m_libros.at(index.row());

    switch (role) {
    case BibliotecaListModel::IdRole:
        return libro->id();
    case BibliotecaListModel::TituloRole:
        return libro->titulo();
    case BibliotecaListModel::AutorRole:
        return libro->autor();
    case BibliotecaListModel::AnioRole:
        return libro->anioPublicacion();
    case BibliotecaListModel::GeneroRole:
        return libro->genero();
    case BibliotecaListModel::ImagenRole:
        return libro->imagenPath();
    case BibliotecaListModel::PrestadoRole:
        return libro->prestado();
    case BibliotecaListModel::FechaPrestamoRole:
        return libro->fechaPrestamoFormateada();
    case BibliotecaListModel::FechaDevolucionEsperadaRole:
        return libro->fechaDevolucionEsperadaFormateada();
    case BibliotecaListModel::DiasRestantesRole:
        return libro->diasRestantes();
    case BibliotecaListModel::EstaRetrasadoRole:
        return libro->estaRetrasado();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> BibliotecaListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[BibliotecaListModel::IdRole] = "libroId";
    roles[BibliotecaListModel::TituloRole] = "titulo";
    roles[BibliotecaListModel::AutorRole] = "autor";
    roles[BibliotecaListModel::AnioRole] = "anioPublicacion";
    roles[BibliotecaListModel::GeneroRole] = "genero";
    roles[BibliotecaListModel::ImagenRole] = "imagenPath";
    roles[BibliotecaListModel::PrestadoRole] = "prestado";
    roles[BibliotecaListModel::FechaPrestamoRole] = "fechaPrestamo";
    roles[BibliotecaListModel::FechaDevolucionEsperadaRole] = "fechaDevolucionEsperada";
    roles[BibliotecaListModel::DiasRestantesRole] = "diasRestantes";
    roles[BibliotecaListModel::EstaRetrasadoRole] = "estaRetrasado";
    return roles;
}

void BibliotecaListModel::appendLibro(LibroPtr libro)
{
    beginInsertRows(QModelIndex(), m_libros.count(), m_libros.count());
    m_libros.append(libro);
    endInsertRows();
    emit countChanged();
}

bool BibliotecaListModel::setPrestado(const QUuid &id, bool prestado)
{
    for (int i = 0; i < m_libros.size(); ++i) {
        if (m_libros.at(i)->id() == id) {
            if (m_libros.at(i)->prestado() != prestado) {
                m_libros.at(i)->setPrestado(prestado);

                QModelIndex index = createIndex(i, 0);
                QVector<int> roles;
                roles << PrestadoRole;
                emit dataChanged(index, index, roles);
                return true;
            }
            return true;
        }
    }
    qWarning() << "Libro no encontrado en el modelo con ID:" << id.toString();
    return false;
}

bool BibliotecaListModel::actualizarFechas(const QUuid &id, const QDateTime &fechaPrestamo,
                                           const QDateTime &fechaDevolucionEsperada,
                                           const QDateTime &fechaDevolucionReal)
{
    for (int i = 0; i < m_libros.size(); ++i) {
        if (m_libros.at(i)->id() == id) {
            m_libros.at(i)->setFechaPrestamo(fechaPrestamo);
            m_libros.at(i)->setFechaDevolucionEsperada(fechaDevolucionEsperada);
            m_libros.at(i)->setFechaDevolucionReal(fechaDevolucionReal);

            QModelIndex index = createIndex(i, 0);
            QVector<int> roles;
            roles << FechaPrestamoRole << FechaDevolucionEsperadaRole
                  << FechaDevolucionRealRole << DiasRestantesRole << EstaRetrasadoRole;
            emit dataChanged(index, index, roles);

            qDebug() << "Fechas actualizadas para libro:" << m_libros.at(i)->titulo();
            return true;
        }
    }
    qWarning() << "Libro no encontrado en el modelo con ID:" << id.toString();
    return false;
}

// 👈 MÉTODO PARA ELIMINAR LIBRO
bool BibliotecaListModel::eliminarLibroPorId(const QUuid &id)
{
    for (int i = 0; i < m_libros.size(); ++i) {
        if (m_libros.at(i)->id() == id) {
            beginRemoveRows(QModelIndex(), i, i);
            m_libros.removeAt(i);
            endRemoveRows();
            emit countChanged();

            qDebug() << "Libro eliminado del modelo. ID:" << id.toString();
            return true;
        }
    }
    qWarning() << "Libro no encontrado para eliminar. ID:" << id.toString();
    return false;
}

QList<LibroPtr> BibliotecaListModel::getLibros(const std::function<bool(const LibroPtr&)> &predicate) const
{
    QList<LibroPtr> resultado;
    for (const LibroPtr &libro : m_libros) {
        if (predicate(libro)) {
            resultado.append(libro);
        }
    }
    return resultado;
}
