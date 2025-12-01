#ifndef TESTLIBROREPOSITORY_H
#define TESTLIBROREPOSITORY_H

#include <QObject>
#include <QtTest/QtTest>
#include "Controlador/LibroRepository.h"
#include "Controlador/SingletonDB.h"

class TestLibroRepository : public QObject
{
    Q_OBJECT

private slots:
    // Ejecutado una vez antes de todos los tests
    void initTestCase();

    // Ejecutado una vez después de todos los tests
    void cleanupTestCase();

    // Ejecutado antes de cada test
    void init();

    // Ejecutado después de cada test
    void cleanup();

    // Tests de CRUD
    void testSaveLibro();
    void testFindById();
    void testFindAll();
    void testFindByGenero();
    void testDeleteById();
    void testExists();
    void testCount();

    // Tests de validaciones
    void testSaveLibroNulo();
    void testSaveLibroTituloVacio();
    void testFindByIdInexistente();

private:
    LibroRepository *m_repo;
    QList<QUuid> m_librosCreados;
};

#endif // TESTLIBROREPOSITORY_H
