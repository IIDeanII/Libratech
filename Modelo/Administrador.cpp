#include "Administrador.h"

Administrador::Administrador(const QUuid &id, const QString &nombre, const QString &email, QObject *parent)
    // **HERENCIA:** Asigna el rol fijo en el constructor base
    : Usuario(id, nombre, email, AdminRole, parent)
{}

// **POLIMORFISMO:** El administrador SÍ puede gestionar libros
bool Administrador::puedeGestionarLibros() const
{
    return true;
}
