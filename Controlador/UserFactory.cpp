#include "UserFactory.h"
#include "Modelo/Administrador.h"
#include "Modelo/UsuarioRegular.h"
#include <QDebug>

Usuario::Ptr UserFactory::createUser(const QUuid &id, const QString &nombre, const QString &email, UserRole role, QObject *parent)
{
    switch (role) {
    case AdminRole:
        qDebug() << "Creando Administrador:" << nombre;
        // Crea y devuelve un puntero a Administrador (que es un Usuario::Ptr)
        return Usuario::Ptr(new Administrador(id, nombre, email, parent));

    case RegularRole:
        qDebug() << "Creando Usuario Regular:" << nombre;
        // Crea y devuelve un puntero a UsuarioRegular
        return Usuario::Ptr(new UsuarioRegular(id, nombre, email, parent));

    default:
        qWarning() << "Error: Rol de usuario desconocido. No se puede crear el usuario.";
        return nullptr;
    }
}
