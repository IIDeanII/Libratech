#ifndef TESTLIBRO_H
#define TESTLIBRO_H

#include <QObject>
#include <QtTest/QtTest>
#include "Modelo/Libro.h"

class TestLibro : public QObject
{
    Q_OBJECT

private slots:
    // Ejecutado antes de cada test
    void init();

    // Ejecutado después de cada test
    void cleanup();

    // Tests de construcción
    void testConstructor();
    void testConstructorConFechas();

    // Tests de getters
    void testGetters();

    // Tests de setters
    void testSetPrestado();
    void testSetFechas();

    // Tests de métodos de utilidad
    void testFechasFormateadas();
    void testDiasRestantes();
    void testEstaRetrasado();

    // Tests de operadores sobrecargados
    void testOperadorIgualdad();
    void testOperadorDesigualdad();
    void testOperadorMenorQue();

private:
    LibroPtr m_libro;
};

#endif // TESTLIBRO_H
