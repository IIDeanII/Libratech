#include "Usuario.h"

Usuario::Usuario(const QUuid &id, const QString &nombre, const QString &email, UserRole role, QObject *parent)
    : QObject(parent), m_id(id), m_nombre(nombre), m_email(email), m_role(role)
{}

QString Usuario::roleString() const
{
    switch (m_role) {
    case AdminRole:
        return "Administrador";
    case RegularRole:
        return "Usuario Regular";
    default:
        return "Desconocido";
    }
}
