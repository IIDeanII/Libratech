#include "TestLibro.h"
#include <QUuid>
#include <QDateTime>

void TestLibro::init()
{
    // Crear un libro de prueba antes de cada test
    QUuid id = QUuid::createUuid();
    m_libro = LibroPtr(new Libro(id, "Libro de Prueba", "Autor Test",
                                 2024, "Ficción", "test.jpg", false));
}

void TestLibro::cleanup()
{
    // Limpiar despues de cada test
    m_libro.clear();
}


// TESTS DE CONSTRUCCION

void TestLibro::testConstructor()
{
    QVERIFY(!m_libro.isNull());
    QCOMPARE(m_libro->titulo(), QString("Libro de Prueba"));
    QCOMPARE(m_libro->autor(), QString("Autor Test"));
    QCOMPARE(m_libro->anioPublicacion(), 2024);
    QCOMPARE(m_libro->genero(), QString("Ficción"));
    QCOMPARE(m_libro->imagenPath(), QString("test.jpg"));
    QCOMPARE(m_libro->prestado(), false);
}

void TestLibro::testConstructorConFechas()
{
    QUuid id = QUuid::createUuid();
    QDateTime fechaPrestamo = QDateTime::currentDateTime();
    QDateTime fechaDevolucion = fechaPrestamo.addDays(14);

    LibroPtr libroConFechas(new Libro(id, "Libro 2", "Autor 2", 2023,
                                      "Drama", "drama.jpg", true,
                                      fechaPrestamo, fechaDevolucion, QDateTime()));

    QVERIFY(libroConFechas->prestado());
    QVERIFY(libroConFechas->fechaPrestamo().isValid());
    QVERIFY(libroConFechas->fechaDevolucionEsperada().isValid());
    QCOMPARE(libroConFechas->fechaPrestamo(), fechaPrestamo);
}


// TESTS DE GETTERS

void TestLibro::testGetters()
{
    QVERIFY(m_libro->id().toString().length() > 0);
    QCOMPARE(m_libro->titulo(), QString("Libro de Prueba"));
    QCOMPARE(m_libro->autor(), QString("Autor Test"));
    QCOMPARE(m_libro->anioPublicacion(), 2024);
    QCOMPARE(m_libro->genero(), QString("Ficción"));
}


// TESTS DE SETTERS

void TestLibro::testSetPrestado()
{
    QSignalSpy spy(m_libro.data(), &Libro::prestadoChanged);

    QCOMPARE(m_libro->prestado(), false);

    m_libro->setPrestado(true);
    QCOMPARE(m_libro->prestado(), true);
    QCOMPARE(spy.count(), 1); // Verificar que se emitió la señal

    // No debe emitir señal si el valor no cambia
    m_libro->setPrestado(true);
    QCOMPARE(spy.count(), 1);
}

void TestLibro::testSetFechas()
{
    QDateTime ahora = QDateTime::currentDateTime();
    QSignalSpy spy(m_libro.data(), &Libro::fechaPrestamoChanged);

    m_libro->setFechaPrestamo(ahora);
    QCOMPARE(m_libro->fechaPrestamo(), ahora);
    QCOMPARE(spy.count(), 1);
}


// TESTS DE MÉTODOS DE UTILIDAD

void TestLibro::testFechasFormateadas()
{
    // Sin fecha valida
    QCOMPARE(m_libro->fechaPrestamoFormateada(), QString("N/A"));

    // Con fecha valida
    QDateTime fecha(QDate(2024, 12, 25), QTime(10, 30));
    m_libro->setFechaPrestamo(fecha);
    QCOMPARE(m_libro->fechaPrestamoFormateada(), QString("25/12/2024"));
}

void TestLibro::testDiasRestantes()
{
    // Sin fecha de devolucion
    QCOMPARE(m_libro->diasRestantes(), -1);

    // Con fecha de devolucion futura
    QDateTime futuro = QDateTime::currentDateTime().addDays(7);
    m_libro->setFechaDevolucionEsperada(futuro);

    int dias = m_libro->diasRestantes();
    QVERIFY(dias >= 6 && dias <= 7); // Puede variar por milisegundos
}

void TestLibro::testEstaRetrasado()
{
    // Libro no prestado
    QCOMPARE(m_libro->estaRetrasado(), false);

    // Libro prestado pero sin fecha
    m_libro->setPrestado(true);
    QCOMPARE(m_libro->estaRetrasado(), false);

    // Libro prestado con fecha de devolución pasada
    QDateTime pasado = QDateTime::currentDateTime().addDays(-5);
    m_libro->setFechaDevolucionEsperada(pasado);
    QCOMPARE(m_libro->estaRetrasado(), true);
}


// TESTS DE OPERADORES SOBRECARGADOS

void TestLibro::testOperadorIgualdad()
{
    QUuid id1 = QUuid::createUuid();
    QUuid id2 = QUuid::createUuid();

    LibroPtr libro1(new Libro(id1, "A", "B", 2000, "C", "d.jpg", false));
    LibroPtr libro2(new Libro(id1, "X", "Y", 2020, "Z", "z.jpg", true)); // Mismo ID
    LibroPtr libro3(new Libro(id2, "A", "B", 2000, "C", "d.jpg", false)); // Diferente ID

    // Dos libros con el mismo ID son iguales
    QVERIFY(*libro1 == *libro2);
    QVERIFY(!(*libro1 == *libro3));
}

void TestLibro::testOperadorDesigualdad()
{
    QUuid id1 = QUuid::createUuid();
    QUuid id2 = QUuid::createUuid();

    LibroPtr libro1(new Libro(id1, "A", "B", 2000, "C", "d.jpg", false));
    LibroPtr libro2(new Libro(id2, "A", "B", 2000, "C", "d.jpg", false));

    QVERIFY(*libro1 != *libro2);
}

void TestLibro::testOperadorMenorQue()
{
    QUuid id1 = QUuid::createUuid();
    QUuid id2 = QUuid::createUuid();

    LibroPtr libroA(new Libro(id1, "A título", "Autor", 2000, "G", "a.jpg", false));
    LibroPtr libroZ(new Libro(id2, "Z título", "Autor", 2000, "G", "z.jpg", false));

    // Orden alfabetico por titulo
    QVERIFY(*libroA < *libroZ);
    QVERIFY(!(*libroZ < *libroA));
}

// Registro del test para que Qt Test lo ejecute
QTEST_MAIN(TestLibro)
#include "TestLibro.moc"
