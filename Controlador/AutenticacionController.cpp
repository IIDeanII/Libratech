#include "AutenticacionController.h"
#include "Controlador/UserFactory.h" // Necesario para crear usuarios
#include <QDebug>
#include <QTimer> // Para simular un delay en el login (opcional)

AutenticacionController::AutenticacionController(QObject *parent)
    : QObject(parent), m_currentUser(nullptr)
{
    // El usuario inicia con sesión cerrada
}

bool AutenticacionController::isLoggedIn() const
{
    return m_currentUser != nullptr;
}

bool AutenticacionController::isAdmin() const
{
    // **POLIMORFISMO EN ACCIÓN:** Llamamos al método virtual de la clase base.
    // El objeto real (Administrador o UsuarioRegular) ejecuta su propia versión.
    return isLoggedIn() && m_currentUser->puedeGestionarLibros();
}

QString AutenticacionController::currentUserNombre() const
{
    return isLoggedIn() ? m_currentUser->nombre() : "Invitado";
}

bool AutenticacionController::login(const QString &email, const QString &password)
{
    // --- LÓGICA DE SIMULACIÓN DE AUTENTICACIÓN ---

    // NOTA: En un sistema real, aquí iría la validación de DB y contraseñas hasheadas.
    // Usaremos valores fijos para demostrar la lógica de rol.
    UserRole role = RegularRole;
    QUuid userId = QUuid::createUuid();
    QString nombre = "Usuario Regular";

    if (email == "admin@libra.tech" && password == "admin123") {
        role = AdminRole;
        nombre = "Admin";
    } else if (email == "user@libra.tech" && password == "user123") {
        role = RegularRole;
        nombre = "Pedro Picapiedra";
    } else {
        qWarning() << "Login fallido para:" << email;
        return false;
    }

    // --- CREACIÓN DE USUARIO USANDO PATRÓN FACTORY ---
    // UserFactory crea el objeto Administrador o UsuarioRegular, devolviendo un puntero base.
    m_currentUser = UserFactory::createUser(userId, nombre, email, role, this);

    // Si el factory devuelve algo válido, emitimos las señales
    if (m_currentUser) {
        emit isLoggedInChanged();
        emit userRoleChanged();
        qDebug() << "Login exitoso. Rol:" << m_currentUser->roleString();
        return true;
    }

    return false;
}

void AutenticacionController::logout()
{
    m_currentUser.clear(); // El puntero inteligente elimina el objeto
    emit isLoggedInChanged();
    emit userRoleChanged();
    qDebug() << "Sesión cerrada.";
}
