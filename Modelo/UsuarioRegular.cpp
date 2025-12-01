#include "UsuarioRegular.h"

UsuarioRegular::UsuarioRegular(const QUuid &id, const QString &nombre, const QString &email, QObject *parent)
    // **HERENCIA:** Asigna el rol fijo en el constructor base
    : Usuario(id, nombre, email, RegularRole, parent)
{}

// **POLIMORFISMO:** El usuario regular NO puede gestionar libros
bool UsuarioRegular::puedeGestionarLibros() const
{
    return false;
}
