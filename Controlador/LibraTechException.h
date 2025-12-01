#ifndef LIBRATECHEXCEPTION_H
#define LIBRATECHEXCEPTION_H

#include <exception>
#include <QString>
#include <QDebug>

/**
 * @brief Clase base para todas las excepciones personalizadas del sistema
 *
 * DEMUESTRA: Manejo de errores avanzado con excepciones personalizadas (requisito del curso)
 */
class LibraTechException : public std::exception
{
public:
    explicit LibraTechException(const QString &mensaje, const QString &contexto = "")
        : m_mensaje(mensaje), m_contexto(contexto)
    {
        m_mensajeCompleto = construirMensaje();
    }

    virtual ~LibraTechException() noexcept = default;

    // Sobrescribe el método what() de std::exception
    const char* what() const noexcept override
    {
        return m_mensajeCompleto.toStdString().c_str();
    }

    QString mensaje() const { return m_mensaje; }
    QString contexto() const { return m_contexto; }
    QString mensajeCompleto() const { return m_mensajeCompleto; }

protected:
    QString m_mensaje;
    QString m_contexto;
    QString m_mensajeCompleto;

    virtual QString construirMensaje() const
    {
        if (m_contexto.isEmpty()) {
            return QString("[LibraTech Error] %1").arg(m_mensaje);
        }
        return QString("[LibraTech Error - %1] %2").arg(m_contexto, m_mensaje);
    }
};

// ========================================
// EXCEPCIONES ESPECÍFICAS
// ========================================

/**
 * @brief Excepción para errores relacionados con la base de datos
 */
class DatabaseException : public LibraTechException
{
public:
    explicit DatabaseException(const QString &mensaje, const QString &queryError = "")
        : LibraTechException(mensaje, "Database"), m_queryError(queryError)
    {
        m_mensajeCompleto = construirMensaje();
    }

    QString queryError() const { return m_queryError; }

protected:
    QString m_queryError;

    QString construirMensaje() const override
    {
        QString msg = QString("[Database Error] %1").arg(m_mensaje);
        if (!m_queryError.isEmpty()) {
            msg += QString(" | SQL Error: %1").arg(m_queryError);
        }
        return msg;
    }
};

/**
 * @brief Excepción para errores de validación de datos
 */
class ValidationException : public LibraTechException
{
public:
    explicit ValidationException(const QString &campo, const QString &razon)
        : LibraTechException(QString("Validación fallida en campo '%1': %2").arg(campo, razon), "Validation"),
        m_campo(campo)
    {}

    QString campo() const { return m_campo; }

private:
    QString m_campo;
};

/**
 * @brief Excepción para cuando no se encuentra un recurso
 */
class NotFoundException : public LibraTechException
{
public:
    explicit NotFoundException(const QString &recurso, const QString &identificador)
        : LibraTechException(QString("%1 no encontrado: %2").arg(recurso, identificador), "NotFound"),
        m_recurso(recurso), m_identificador(identificador)
    {}

    QString recurso() const { return m_recurso; }
    QString identificador() const { return m_identificador; }

private:
    QString m_recurso;
    QString m_identificador;
};

/**
 * @brief Excepción para operaciones no permitidas
 */
class OperationNotAllowedException : public LibraTechException
{
public:
    explicit OperationNotAllowedException(const QString &operacion, const QString &razon)
        : LibraTechException(QString("Operación '%1' no permitida: %2").arg(operacion, razon), "Permission")
    {}
};

/**
 * @brief Excepción para errores de autenticación
 */
class AuthenticationException : public LibraTechException
{
public:
    explicit AuthenticationException(const QString &mensaje)
        : LibraTechException(mensaje, "Authentication")
    {}
};

#endif // LIBRATECHEXCEPTION_H
