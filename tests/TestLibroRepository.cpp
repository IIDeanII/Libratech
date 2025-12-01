#include "TestLibroRepository.h"
#include "Controlador/LibraTechException.h"
#include <QSqlQuery>
#include <QSqlError>

void TestLibroRepository::initTestCase()
{
    // Inicializar DB de prueba en memoria
    QSqlDatabase db;
    if (QSqlDatabase::contains(QSqlDatabase::defaultConnection)) {
        db = QSqlDatabase::database();
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(":memory:");
    }

    if (!db.open()) {
        QFAIL("No se pudo abrir la base de datos de prueba");
    }

    // Crear tabla de prueba
    QSqlQuery query(db);
    QString createTable =
        "CREATE TABLE Libros ("
        "id TEXT PRIMARY KEY,"
        "titulo TEXT NOT NULL,"
        "autor TEXT NOT NULL,"
        "anio INTEGER,"
        "genero TEXT,"
        "imagenPath TEXT,"
        "prestado INTEGER NOT NULL DEFAULT 0,"
        "fechaPrestamo TEXT,"
        "fechaDevolucionEsperada TEXT,"
        "fechaDevolucionReal TEXT"
        ")";

    if (!query.exec(createTable)) {
        QFAIL(qPrintable("Error al crear tabla: " + query.lastError().text()));
    }
}

void TestLibroRepository::cleanupTestCase()
{
    QSqlDatabase::database().close();
    QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
}

void TestLibroRepository::init()
{
    m_repo = new LibroRepository();
    m_librosCreados.clear();
}

void TestLibroRepository::cleanup()
{
    // Eliminar libros creados durante el test
    for (const QUuid &id : m_librosCreados) {
        try {
            m_repo->deleteById(id);
        } catch (...) {
            // Ignorar errores al limpiar
        }
    }

    delete m_repo;
}


// TESTS DE CRUD

void TestLibroRepository::testSaveLibro()
{
    QUuid id = QUuid::createUuid();
    LibroPtr libro(new Libro(id, "Test Libro", "Test Autor", 2024,
                             "Test Género", "test.jpg", false));

    bool resultado = m_repo->save(libro);

    QVERIFY(resultado);
    QVERIFY(m_repo->exists(id));

    m_librosCreados.append(id);
}

void TestLibroRepository::testFindById()
{
    // Crear libro
    QUuid id = QUuid::createUuid();
    LibroPtr libroOriginal(new Libro(id, "Buscar Test", "Autor Buscar",
                                     2024, "Drama", "buscar.jpg", false));
    m_repo->save(libroOriginal);
    m_librosCreados.append(id);

    // Buscar libro
    LibroPtr libroEncontrado = m_repo->findById(id);

    QVERIFY(!libroEncontrado.isNull());
    QCOMPARE(libroEncontrado->id(), id);
    QCOMPARE(libroEncontrado->titulo(), QString("Buscar Test"));
    QCOMPARE(libroEncontrado->autor(), QString("Autor Buscar"));
}

void TestLibroRepository::testFindAll()
{
    // Crear 3 libros
    for (int i = 0; i < 3; ++i) {
        QUuid id = QUuid::createUuid();
        LibroPtr libro(new Libro(id, QString("Libro %1").arg(i), "Autor",
                                 2024, "Género", "img.jpg", false));
        m_repo->save(libro);
        m_librosCreados.append(id);
    }

    QList<LibroPtr> todos = m_repo->findAll();

    QVERIFY(todos.size() >= 3);
}

void TestLibroRepository::testFindByGenero()
{
    // Crear libros de diferentes generos
    QUuid id1 = QUuid::createUuid();
    LibroPtr libro1(new Libro(id1, "Ficción 1", "Autor", 2024, "Ficción", "f.jpg", false));
    m_repo->save(libro1);
    m_librosCreados.append(id1);

    QUuid id2 = QUuid::createUuid();
    LibroPtr libro2(new Libro(id2, "Drama 1", "Autor", 2024, "Drama", "d.jpg", false));
    m_repo->save(libro2);
    m_librosCreados.append(id2);

    // Buscar por genero
    QList<LibroPtr> ficciones = m_repo->findByGenero("Ficción");

    QVERIFY(ficciones.size() >= 1);
    QCOMPARE(ficciones.first()->genero(), QString("Ficción"));
}

void TestLibroRepository::testDeleteById()
{
    // Crear libro
    QUuid id = QUuid::createUuid();
    LibroPtr libro(new Libro(id, "Eliminar Test", "Autor", 2024, "G", "e.jpg", false));
    m_repo->save(libro);

    QVERIFY(m_repo->exists(id));

    // Eliminar
    bool resultado = m_repo->deleteById(id);

    QVERIFY(resultado);
    QVERIFY(!m_repo->exists(id));
}

void TestLibroRepository::testExists()
{
    QUuid idExistente = QUuid::createUuid();
    QUuid idInexistente = QUuid::createUuid();

    LibroPtr libro(new Libro(idExistente, "Exists Test", "Autor",
                             2024, "G", "e.jpg", false));
    m_repo->save(libro);
    m_librosCreados.append(idExistente);

    QVERIFY(m_repo->exists(idExistente));
    QVERIFY(!m_repo->exists(idInexistente));
}

void TestLibroRepository::testCount()
{
    int countInicial = m_repo->count();

    // Agregar 2 libros
    for (int i = 0; i < 2; ++i) {
        QUuid id = QUuid::createUuid();
        LibroPtr libro(new Libro(id, QString("Count %1").arg(i), "A", 2024, "G", "c.jpg", false));
        m_repo->save(libro);
        m_librosCreados.append(id);
    }

    int countFinal = m_repo->count();

    QCOMPARE(countFinal, countInicial + 2);
}


// TESTS DE VALIDACIONES

void TestLibroRepository::testSaveLibroNulo()
{
    LibroPtr libroNulo;

    QVERIFY_EXCEPTION_THROWN(m_repo->save(libroNulo), ValidationException);
}

void TestLibroRepository::testSaveLibroTituloVacio()
{
    QUuid id = QUuid::createUuid();
    LibroPtr libro(new Libro(id, "", "Autor", 2024, "G", "i.jpg", false));

    QVERIFY_EXCEPTION_THROWN(m_repo->save(libro), ValidationException);
}

void TestLibroRepository::testFindByIdInexistente()
{
    QUuid idInexistente = QUuid::createUuid();

    QVERIFY_EXCEPTION_THROWN(m_repo->findById(idInexistente), NotFoundException);
}

QTEST_MAIN(TestLibroRepository)
#include "TestLibroRepository.moc"
