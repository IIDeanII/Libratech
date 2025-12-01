#ifndef ADMINISTRADOR_H
#define ADMINISTRADOR_H

#include "Usuario.h"

class Administrador : public Usuario
{
    Q_OBJECT
public:
    // **HERENCIA:** Llama al constructor base
    explicit Administrador(const QUuid &id, const QString &nombre, const QString &email, QObject *parent = nullptr);

    // **POLIMORFISMO:** Implementación concreta del método virtual puro
    bool puedeGestionarLibros() const override;
};

#endif // ADMINISTRADOR_H
