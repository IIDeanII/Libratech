#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>
#include <QtQuickControls2/QQuickStyle>

#include "Controlador/SingletonDB.h"
#include "Controlador/BibliotecaController.h"
#include "Controlador/AutenticacionController.h"
#include "Controlador/HistorialController.h"
#include "Controlador/RecomendacionController.h"
#include "Modelo/Libro.h"
#include "Modelo/BibliotecaListModel.h"
#include "Modelo/RegistroHistorial.h"
#include "Modelo/HistorialListModel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle("Fusion");

    qmlRegisterType<Libro>("LibraTechTO.Modelo", 1, 0, "Libro");
    qmlRegisterType<BibliotecaListModel>("LibraTechTO.Modelo", 1, 0, "BibliotecaListModel");
    qmlRegisterType<RegistroHistorial>("LibraTechTO.Modelo", 1, 0, "RegistroHistorial");
    qmlRegisterType<HistorialListModel>("LibraTechTO.Modelo", 1, 0, "HistorialListModel");

    if (!SingletonDB::getInstance().inicializar()) {
        qCritical() << "No se pudo inicializar la base de datos. Terminando.";
        return -1;
    }

    BibliotecaController bibliotecaController;
    AutenticacionController autenticacionController;
    HistorialController historialController;
    RecomendacionController recomendacionController;

    bibliotecaController.setHistorialController(&historialController);

    // Cast explicito a BibliotecaListModel
    BibliotecaListModel* modeloBase = qobject_cast<BibliotecaListModel*>(
        bibliotecaController.bibliotecaFilterModel()->sourceModel()
        );

    if (modeloBase) {
        recomendacionController.setBibliotecaModel(modeloBase);
    } else {
        qWarning() << "No se pudo obtener el modelo base de biblioteca";
    }

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("bibliotecaController", &bibliotecaController);
    engine.rootContext()->setContextProperty("autenticacionController", &autenticacionController);
    engine.rootContext()->setContextProperty("historialController", &historialController);
    engine.rootContext()->setContextProperty("recomendacionController", &recomendacionController);

    bibliotecaController.inicializarDatos();
    historialController.cargarHistorial();

    const QUrl url(QStringLiteral("qrc:/Vista/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); },
                     Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
