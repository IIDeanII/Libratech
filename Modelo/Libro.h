#ifndef LIBRO_H
#define LIBRO_H

#include <QObject>
#include <QString>
#include <QUuid>
#include <QSharedPointer>
#include <QDebug>
#include <QDateTime>

class Libro : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUuid libroId READ id CONSTANT)
    Q_PROPERTY(QString titulo READ titulo CONSTANT)
    Q_PROPERTY(QString autor READ autor CONSTANT)
    Q_PROPERTY(int anioPublicacion READ anioPublicacion CONSTANT)
    Q_PROPERTY(QString genero READ genero CONSTANT)
    Q_PROPERTY(QString imagenPath READ imagenPath CONSTANT)
    Q_PROPERTY(bool prestado READ prestado WRITE setPrestado NOTIFY prestadoChanged)

    // 👈 NUEVAS PROPIEDADES DE FECHAS
    Q_PROPERTY(QDateTime fechaPrestamo READ fechaPrestamo WRITE setFechaPrestamo NOTIFY fechaPrestamoChanged)
    Q_PROPERTY(QDateTime fechaDevolucionEsperada READ fechaDevolucionEsperada WRITE setFechaDevolucionEsperada NOTIFY fechaDevolucionEsperadaChanged)
    Q_PROPERTY(QDateTime fechaDevolucionReal READ fechaDevolucionReal WRITE setFechaDevolucionReal NOTIFY fechaDevolucionRealChanged)

public:
    explicit Libro(const QUuid &id, const QString &titulo, const QString &autor, int anio,
                   const QString &genero, const QString &imagenPath, bool prestadoInicial,
                   QObject *parent = nullptr);

    // Constructor con fechas
    explicit Libro(const QUuid &id, const QString &titulo, const QString &autor, int anio,
                   const QString &genero, const QString &imagenPath, bool prestadoInicial,
                   const QDateTime &fechaPrestamo, const QDateTime &fechaDevolucionEsperada,
                   const QDateTime &fechaDevolucionReal, QObject *parent = nullptr);

    // Getters (READ functions)
    QUuid id() const { return m_id; }
    QString titulo() const { return m_titulo; }
    QString autor() const { return m_autor; }
    int anioPublicacion() const { return m_anioPublicacion; }
    QString genero() const { return m_genero; }
    QString imagenPath() const { return m_imagenPath; }
    bool prestado() const { return m_prestado; }

    // Getters de fechas
    QDateTime fechaPrestamo() const { return m_fechaPrestamo; }
    QDateTime fechaDevolucionEsperada() const { return m_fechaDevolucionEsperada; }
    QDateTime fechaDevolucionReal() const { return m_fechaDevolucionReal; }

    // Setters
    void setPrestado(bool nuevoEstado);
    void setFechaPrestamo(const QDateTime &fecha);
    void setFechaDevolucionEsperada(const QDateTime &fecha);
    void setFechaDevolucionReal(const QDateTime &fecha);

    // Métodos de utilidad para fechas
    Q_INVOKABLE QString fechaPrestamoFormateada() const;
    Q_INVOKABLE QString fechaDevolucionEsperadaFormateada() const;
    Q_INVOKABLE int diasRestantes() const; // Días hasta la devolución esperada
    Q_INVOKABLE bool estaRetrasado() const; // Si pasó la fecha de devolución esperada

    // ========================================
    // SOBRECARGA DE OPERADORES
    // ========================================
    bool operator==(const Libro &otro) const;
    bool operator!=(const Libro &otro) const;
    bool operator<(const Libro &otro) const;
    friend QDebug operator<<(QDebug debug, const Libro &libro);

signals:
    void prestadoChanged();
    void fechaPrestamoChanged();
    void fechaDevolucionEsperadaChanged();
    void fechaDevolucionRealChanged();

private:
    QUuid m_id;
    QString m_titulo;
    QString m_autor;
    int m_anioPublicacion;
    QString m_genero;
    QString m_imagenPath;
    bool m_prestado;

    // Nuevos campos de fechas
    QDateTime m_fechaPrestamo;
    QDateTime m_fechaDevolucionEsperada;
    QDateTime m_fechaDevolucionReal;
};

using LibroPtr = QSharedPointer<Libro>;

#endif // LIBRO_H
