#ifndef USUARIO_H
#define USUARIO_H

#include <QObject>
#include <QUuid>
#include <QSharedPointer>

// Definición de roles de usuario
enum UserRole {
    AdminRole = 1,
    RegularRole = 2
};

// Clase base abstracta o con destructor virtual
class Usuario : public QObject
{
    Q_OBJECT
public:
    // Puntero inteligente para manejar objetos Usuario
    using Ptr = QSharedPointer<Usuario>;

    // Constructor base
    explicit Usuario(const QUuid &id, const QString &nombre, const QString &email, UserRole role, QObject *parent = nullptr);
    // Destructor virtual: CRÍTICO para el polimorfismo
    virtual ~Usuario() {}

    // Getters de propiedades
    QUuid id() const { return m_id; }
    QString nombre() const { return m_nombre; }
    QString email() const { return m_email; }
    UserRole role() const { return m_role; }

    // **MÉTODO VIRTUAL PURO (POLIMORFISMO)**
    // Cada subclase debe definir qué acciones puede realizar.
    virtual bool puedeGestionarLibros() const = 0;

    // Método para obtener el rol como QString (útil para la interfaz/DB)
    QString roleString() const;

protected:
    QUuid m_id;
    QString m_nombre;
    QString m_email;
    UserRole m_role;
    // La contraseña hasheada se manejaría en una capa separada o en el controlador por seguridad.
};

#endif // USUARIO_H
