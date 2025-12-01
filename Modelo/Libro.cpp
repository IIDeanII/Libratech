#include "Libro.h"

// Constructor original (mantiene compatibilidad)
Libro::Libro(const QUuid &id, const QString &titulo, const QString &autor, int anio,
             const QString &genero, const QString &imagenPath, bool prestadoInicial,
             QObject *parent)
    : QObject(parent),
    m_id(id),
    m_titulo(titulo),
    m_autor(autor),
    m_anioPublicacion(anio),
    m_genero(genero),
    m_imagenPath(imagenPath),
    m_prestado(prestadoInicial),
    m_fechaPrestamo(QDateTime()),
    m_fechaDevolucionEsperada(QDateTime()),
    m_fechaDevolucionReal(QDateTime())
{
}

// Constructor con fechas
Libro::Libro(const QUuid &id, const QString &titulo, const QString &autor, int anio,
             const QString &genero, const QString &imagenPath, bool prestadoInicial,
             const QDateTime &fechaPrestamo, const QDateTime &fechaDevolucionEsperada,
             const QDateTime &fechaDevolucionReal, QObject *parent)
    : QObject(parent),
    m_id(id),
    m_titulo(titulo),
    m_autor(autor),
    m_anioPublicacion(anio),
    m_genero(genero),
    m_imagenPath(imagenPath),
    m_prestado(prestadoInicial),
    m_fechaPrestamo(fechaPrestamo),
    m_fechaDevolucionEsperada(fechaDevolucionEsperada),
    m_fechaDevolucionReal(fechaDevolucionReal)
{
}

// Setter de prestado
void Libro::setPrestado(bool nuevoEstado)
{
    if (m_prestado == nuevoEstado)
        return;
    m_prestado = nuevoEstado;
    emit prestadoChanged();
}

// Setters de fechas
void Libro::setFechaPrestamo(const QDateTime &fecha)
{
    if (m_fechaPrestamo == fecha)
        return;
    m_fechaPrestamo = fecha;
    emit fechaPrestamoChanged();
}

void Libro::setFechaDevolucionEsperada(const QDateTime &fecha)
{
    if (m_fechaDevolucionEsperada == fecha)
        return;
    m_fechaDevolucionEsperada = fecha;
    emit fechaDevolucionEsperadaChanged();
}

void Libro::setFechaDevolucionReal(const QDateTime &fecha)
{
    if (m_fechaDevolucionReal == fecha)
        return;
    m_fechaDevolucionReal = fecha;
    emit fechaDevolucionRealChanged();
}

// ========================================
// MÉTODOS DE UTILIDAD PARA FECHAS
// ========================================

QString Libro::fechaPrestamoFormateada() const
{
    if (!m_fechaPrestamo.isValid())
        return "N/A";
    return m_fechaPrestamo.toString("dd/MM/yyyy");
}

QString Libro::fechaDevolucionEsperadaFormateada() const
{
    if (!m_fechaDevolucionEsperada.isValid())
        return "N/A";
    return m_fechaDevolucionEsperada.toString("dd/MM/yyyy");
}

int Libro::diasRestantes() const
{
    if (!m_fechaDevolucionEsperada.isValid())
        return -1;

    QDateTime ahora = QDateTime::currentDateTime();
    return ahora.daysTo(m_fechaDevolucionEsperada);
}

bool Libro::estaRetrasado() const
{
    if (!m_prestado || !m_fechaDevolucionEsperada.isValid())
        return false;

    // Si no se ha devuelto y la fecha esperada ya pasó
    if (!m_fechaDevolucionReal.isValid()) {
        return QDateTime::currentDateTime() > m_fechaDevolucionEsperada;
    }

    // Si ya se devolvió, verificar si fue después de la fecha esperada
    return m_fechaDevolucionReal > m_fechaDevolucionEsperada;
}

// ========================================
// SOBRECARGA DE OPERADORES
// ========================================

bool Libro::operator==(const Libro &otro) const
{
    return m_id == otro.m_id;
}

bool Libro::operator!=(const Libro &otro) const
{
    return !(*this == otro);
}

bool Libro::operator<(const Libro &otro) const
{
    return m_titulo.toLower() < otro.m_titulo.toLower();
}

QDebug operator<<(QDebug debug, const Libro &libro)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "Libro("
                    << "ID: " << libro.m_id.toString() << ", "
                    << "Título: " << libro.m_titulo << ", "
                    << "Autor: " << libro.m_autor << ", "
                    << "Año: " << libro.m_anioPublicacion << ", "
                    << "Género: " << libro.m_genero << ", "
                    << "Prestado: " << (libro.m_prestado ? "Sí" : "No");

    if (libro.m_fechaPrestamo.isValid()) {
        debug.nospace() << ", Fecha Préstamo: " << libro.m_fechaPrestamo.toString("dd/MM/yyyy");
    }

    debug.nospace() << ")";
    return debug;
}
