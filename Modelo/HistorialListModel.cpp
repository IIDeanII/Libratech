#include "HistorialListModel.h"
#include <QDebug>

HistorialListModel::HistorialListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int HistorialListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_registros.count();
}

QVariant HistorialListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_registros.count())
        return QVariant();

    const RegistroHistorialPtr registro = m_registros.at(index.row());

    switch (role) {
    case IdRole:
        return registro->id();
    case LibroIdRole:
        return registro->libroId();
    case LibroTituloRole:
        return registro->libroTitulo();
    case LibroAutorRole:
        return registro->libroAutor();
    case AccionRole:
        return registro->accion();
    case FechaFormateadaRole:
        return registro->fechaFormateada();
    case UsuarioNombreRole:
        return registro->usuarioNombre();
    case ObservacionesRole:
        return registro->observaciones();
    case EsPrestamoRole:
        return registro->esPrestamo();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> HistorialListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "registroId";
    roles[LibroIdRole] = "libroId";
    roles[LibroTituloRole] = "libroTitulo";
    roles[LibroAutorRole] = "libroAutor";
    roles[AccionRole] = "accion";
    roles[FechaFormateadaRole] = "fechaFormateada";
    roles[UsuarioNombreRole] = "usuarioNombre";
    roles[ObservacionesRole] = "observaciones";
    roles[EsPrestamoRole] = "esPrestamo";
    return roles;
}

void HistorialListModel::appendRegistro(RegistroHistorialPtr registro)
{
    beginInsertRows(QModelIndex(), 0, 0); // Insertar al inicio (más recientes primero)
    m_registros.prepend(registro);
    endInsertRows();
    emit countChanged();
}

void HistorialListModel::clear()
{
    beginResetModel();
    m_registros.clear();
    endResetModel();
    emit countChanged();
}
