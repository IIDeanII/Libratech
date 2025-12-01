#ifndef BIBLIOTECALISTMODEL_H
#define BIBLIOTECALISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QSharedPointer>
#include <functional>
#include "Libro.h"

using LibroPtr = QSharedPointer<Libro>;

class BibliotecaListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    explicit BibliotecaListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    enum Roles {
        IdRole = Qt::UserRole + 1,
        TituloRole,
        AutorRole,
        AnioRole,
        GeneroRole,
        ImagenRole,
        PrestadoRole,
        FechaPrestamoRole,
        FechaDevolucionEsperadaRole,
        FechaDevolucionRealRole,
        DiasRestantesRole,
        EstaRetrasadoRole
    };

    void appendLibro(LibroPtr libro);
    bool setPrestado(const QUuid &id, bool prestado);
    bool actualizarFechas(const QUuid &id, const QDateTime &fechaPrestamo,
                          const QDateTime &fechaDevolucionEsperada,
                          const QDateTime &fechaDevolucionReal);

    // 👈 MÉTODO PARA ELIMINAR (sin override)
    bool eliminarLibroPorId(const QUuid &id);

    QList<LibroPtr> getLibros(const std::function<bool(const LibroPtr&)> &predicate) const;

signals:
    void countChanged();

private:
    QList<LibroPtr> m_libros;
};

#endif // BIBLIOTECALISTMODEL_H
