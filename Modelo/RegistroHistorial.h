#ifndef REGISTROHISTORIAL_H
#define REGISTROHISTORIAL_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QSharedPointer>

/**
 * @brief Clase que representa un registro en el historial de préstamos
 */
class RegistroHistorial : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id CONSTANT)
    Q_PROPERTY(QString libroId READ libroId CONSTANT)
    Q_PROPERTY(QString libroTitulo READ libroTitulo CONSTANT)
    Q_PROPERTY(QString libroAutor READ libroAutor CONSTANT)
    Q_PROPERTY(QString accion READ accion CONSTANT)
    Q_PROPERTY(QString fechaFormateada READ fechaFormateada CONSTANT)
    Q_PROPERTY(QString usuarioNombre READ usuarioNombre CONSTANT)
    Q_PROPERTY(QString observaciones READ observaciones CONSTANT)

public:
    explicit RegistroHistorial(int id, const QString &libroId, const QString &libroTitulo,
                               const QString &libroAutor, const QString &accion,
                               const QDateTime &fecha, const QString &usuarioNombre,
                               const QString &observaciones = "", QObject *parent = nullptr)
        : QObject(parent),
        m_id(id),
        m_libroId(libroId),
        m_libroTitulo(libroTitulo),
        m_libroAutor(libroAutor),
        m_accion(accion),
        m_fecha(fecha),
        m_usuarioNombre(usuarioNombre),
        m_observaciones(observaciones)
    {}

    // Getters
    int id() const { return m_id; }
    QString libroId() const { return m_libroId; }
    QString libroTitulo() const { return m_libroTitulo; }
    QString libroAutor() const { return m_libroAutor; }
    QString accion() const { return m_accion; }
    QDateTime fecha() const { return m_fecha; }
    QString usuarioNombre() const { return m_usuarioNombre; }
    QString observaciones() const { return m_observaciones; }

    QString fechaFormateada() const {
        return m_fecha.toString("dd/MM/yyyy HH:mm");
    }

    // Método de utilidad
    bool esPrestamo() const {
        return m_accion == "PRESTAMO";
    }

private:
    int m_id;
    QString m_libroId;
    QString m_libroTitulo;
    QString m_libroAutor;
    QString m_accion;       // "PRESTAMO" o "DEVOLUCION"
    QDateTime m_fecha;
    QString m_usuarioNombre;
    QString m_observaciones;
};

using RegistroHistorialPtr = QSharedPointer<RegistroHistorial>;

#endif // REGISTROHISTORIAL_H
