#ifndef HISTORIALCONTROLLER_H
#define HISTORIALCONTROLLER_H

#include <QObject>
#include <QString>
#include <QUuid>
#include <QDateTime>
#include "Modelo/HistorialListModel.h"

class HistorialController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(HistorialListModel* historialModel READ historialModel CONSTANT)

public:
    explicit HistorialController(QObject *parent = nullptr);

    HistorialListModel* historialModel() const { return m_historialModel; }

    // Métodos para registrar acciones en el historial
    Q_INVOKABLE void registrarPrestamo(const QString &libroId, const QString &libroTitulo,
                                       const QString &libroAutor, const QString &usuarioNombre);

    Q_INVOKABLE void registrarDevolucion(const QString &libroId, const QString &libroTitulo,
                                         const QString &libroAutor, const QString &usuarioNombre);

    // Cargar historial desde la DB
    Q_INVOKABLE void cargarHistorial();

    // Limpiar el historial (solo para admin)
    Q_INVOKABLE void limpiarHistorial();

private:
    HistorialListModel *m_historialModel;

    void registrarAccion(const QString &libroId, const QString &libroTitulo,
                         const QString &libroAutor, const QString &accion,
                         const QString &usuarioNombre);
};

#endif // HISTORIALCONTROLLER_H
