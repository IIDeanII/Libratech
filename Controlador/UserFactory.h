#ifndef USERFACTORY_H
#define USERFACTORY_H

#include <QObject>
#include "Modelo/Usuario.h"
#include <QUuid>

class UserFactory
{
public:
    // **MÉTODO CLAVE DEL PATRÓN FACTORY**
    // Decide qué tipo de objeto crear (Administrador o UsuarioRegular) basado en el rol.
    static Usuario::Ptr createUser(const QUuid &id, const QString &nombre,
                                   const QString &email, UserRole role,
                                   QObject *parent = nullptr);

    // Nota: El Factory se implementa sin estado (métodos estáticos) ya que solo crea objetos.
};

#endif // USERFACTORY_H
