#ifndef HISTORIALLISTMODEL_H
#define HISTORIALLISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include "RegistroHistorial.h"

class HistorialListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    explicit HistorialListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    enum Roles {
        IdRole = Qt::UserRole + 1,
        LibroIdRole,
        LibroTituloRole,
        LibroAutorRole,
        AccionRole,
        FechaFormateadaRole,
        UsuarioNombreRole,
        ObservacionesRole,
        EsPrestamoRole
    };

    void appendRegistro(RegistroHistorialPtr registro);
    void clear();

signals:
    void countChanged();

private:
    QList<RegistroHistorialPtr> m_registros;
};

#endif // HISTORIALLISTMODEL_H
