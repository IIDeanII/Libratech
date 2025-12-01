#ifndef AUTENTICACIONCONTROLLER_H
#define AUTENTICACIONCONTROLLER_H

#include <QObject>
#include <QUuid>
#include "Modelo/Usuario.h"

class AutenticacionController : public QObject
{
    Q_OBJECT

    // Propiedades expuestas a QML para controlar la UI
    Q_PROPERTY(bool isLoggedIn READ isLoggedIn NOTIFY isLoggedInChanged)
    Q_PROPERTY(bool isAdmin READ isAdmin NOTIFY userRoleChanged) // Usado para habilitar/deshabilitar UI de Admin
    Q_PROPERTY(QString currentUserNombre READ currentUserNombre NOTIFY userRoleChanged)

public:
    explicit AutenticacionController(QObject *parent = nullptr);

    // Q_INVOKABLE: Simula el proceso de inicio de sesión desde QML
    Q_INVOKABLE bool login(const QString &email, const QString &password);

    // Q_INVOKABLE: Cerrar sesión
    Q_INVOKABLE void logout();

    // Getters de propiedades
    bool isLoggedIn() const;
    bool isAdmin() const;
    QString currentUserNombre() const;

signals:
    void isLoggedInChanged();
    void userRoleChanged();

private:
    Usuario::Ptr m_currentUser; // El puntero inteligente al usuario activo
};

#endif // AUTENTICACIONCONTROLLER_H
