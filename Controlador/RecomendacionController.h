#ifndef RECOMENDACIONCONTROLLER_H
#define RECOMENDACIONCONTROLLER_H

#include <QObject>
#include <QString>
#include <QList>
#include "Modelo/BibliotecaListModel.h"
#include "Modelo/Libro.h"

class RecomendacionController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(BibliotecaListModel* recomendacionesModel READ recomendacionesModel CONSTANT)

public:
    explicit RecomendacionController(QObject *parent = nullptr);

    BibliotecaListModel* recomendacionesModel() const { return m_recomendacionesModel; }

    // Generar recomendaciones basadas en historial del usuario
    Q_INVOKABLE void generarRecomendaciones(const QString &usuarioNombre);

    // Establecer acceso al modelo principal de libros
    void setBibliotecaModel(BibliotecaListModel *model);

private:
    BibliotecaListModel *m_recomendacionesModel;
    BibliotecaListModel *m_bibliotecaModelRef; // Referencia al modelo principal

    // Métodos auxiliares
    QStringList obtenerGenerosPreferidos(const QString &usuarioNombre);
    QStringList obtenerAutoresPreferidos(const QString &usuarioNombre);
    QList<LibroPtr> filtrarLibrosDisponibles(const QList<LibroPtr> &libros);
};

#endif // RECOMENDACIONCONTROLLER_H
