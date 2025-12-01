#ifndef USUARIOREGULAR_H
#define USUARIOREGULAR_H

#include "Usuario.h"

class UsuarioRegular : public Usuario
{
    Q_OBJECT
public:
    // **HERENCIA:** Llama al constructor base
    explicit UsuarioRegular(const QUuid &id, const QString &nombre, const QString &email, QObject *parent = nullptr);

    // **POLIMORFISMO:** Implementación concreta del método virtual puro
    bool puedeGestionarLibros() const override;
};

#endif // USUARIOREGULAR_H
