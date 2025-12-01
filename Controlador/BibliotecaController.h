#ifndef BIBLIOTECACONTROLLER_H
#define BIBLIOTECACONTROLLER_H

#include <QObject>
#include <QUuid>
#include <QFuture>
#include <QFutureWatcher>
#include <QDateTime>
#include "Modelo/BibliotecaListModel.h"
#include "Controlador/BibliotecaFilterModel.h"

class HistorialController; // Forward declaration

class BibliotecaController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(BibliotecaFilterModel* bibliotecaFilterModel READ bibliotecaFilterModel CONSTANT)
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)

public:
    explicit BibliotecaController(QObject *parent = nullptr);

    BibliotecaFilterModel* bibliotecaFilterModel() const;
    bool isLoading() const { return m_isLoading; }

    void inicializarDatos();

    // 👈 NUEVO: Método público para agregar libro (llamado desde QML)
    Q_INVOKABLE bool agregarLibroDesdeUI(const QString &titulo, const QString &autor,
                                         int anio, const QString &genero, const QString &imagenUrl);

    // Métodos de préstamo/devolución
    Q_INVOKABLE void realizarPrestamo(const QUuid &id);
    Q_INVOKABLE void realizarDevolucion(const QUuid &id);

    // 👈 NUEVO: Establecer el controlador de historial
    void setHistorialController(HistorialController *historialController);

    Q_INVOKABLE bool eliminarLibro(const QUuid &id);

signals:
    void isLoadingChanged();
    void cargaCompletada(int totalLibros);
    void libroAgregado(bool exito, const QString &mensaje); // 👈 NUEVA SEÑAL

private slots:
    void onCargaLibrosCompletada();

private:
    BibliotecaListModel *m_bibliotecaModel;
    BibliotecaFilterModel *m_bibliotecaFilterModel;
    bool m_isLoading;
    QFutureWatcher<QList<LibroPtr>> *m_futureWatcher;
    HistorialController *m_historialController; // 👈 NUEVO

    static constexpr int DIAS_PRESTAMO_DEFAULT = 14;

    void actualizarEstadoPrestamo(const QUuid &id, bool nuevoEstado);
    void actualizarFechasPrestamo(const QUuid &id, bool esPrestamo);
    void agregarLibro(const QString &titulo, const QString &autor, int anio,
                      const QString &genero, const QString &imagenPath);

    static QList<LibroPtr> cargarLibrosDesdeDB();
};

#endif // BIBLIOTECACONTROLLER_H
