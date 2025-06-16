#include "databasemanager.h"
#include <QDebug>        // Para mensajes de depuración
#include <QSqlQuery>     // Para ejecutar consultas SQL
#include <QSqlError>     // Para obtener información de errores SQL
#include <QStandardPaths> // Para obtener rutas estándar del sistema (usado en ejemplos, no en la implementación final si usas ruta fija)
#include <QDir>          // Para manejar directorios
#include <QFileInfo>     // Para obtener información de archivos

// Constructor: Inicializa el objeto DatabaseManager
DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent)
{
    // No añadimos la base de datos aquí. Se hará en los métodos initialize*.
    // Esto permite que el gestor de la DB sea más flexible y elija el driver adecuado.
}

// Destructor: Asegura que la base de datos se cierre y se remueva la conexión
DatabaseManager::~DatabaseManager()
{
    closeDatabase(); // Cierra la conexión si está abierta

    // Es crucial remover explícitamente la conexión de la base de datos
    // para evitar problemas si se intenta reabrir o se crean múltiples gestores.
    if (m_db.isValid()) {
        QString connectionName = m_db.connectionName();
        if (!connectionName.isEmpty() && QSqlDatabase::contains(connectionName)) {
            QSqlDatabase::removeDatabase(connectionName);
            qInfo() << "Conexión a la base de datos removida:" << connectionName;
        }
    }
}


   // Inicializa la base de datos para SQLite
   bool DatabaseManager::initializeSqliteDatabase(const QString& dbFilePath)
{
    m_currentDbType = SQLite;   // Establece el tipo de DB actual
    m_dbName = dbFilePath;      // Para SQLite, m_dbName guarda la ruta del archivo

    // Si ya existe una conexión con el nombre por defecto, la eliminamos.
    // Esto es importante si el gestor se reutiliza o se inicializa varias veces.
    if (QSqlDatabase::contains(QSqlDatabase::defaultConnection)) {
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    }
    // Añade la base de datos usando el driver "QSQLITE"
    m_db = QSqlDatabase::addDatabase("QSQLITE");

    // Intenta abrir la base de datos usando la función auxiliar interna
    if (!openDatabaseInternal()) {
        qCritical() << "Error: No se pudo abrir la base de datos SQLite en la inicialización.";
        return false;
    }

    // Crea las tablas de la base de datos
    if (!createUsersTable()) {
        qCritical() << "Error: No se pudo crear la tabla 'users' (SQLite).";
        return false;
    }
    if (!createHealthMetricsTable()) {
        qCritical() << "Error: No se pudo crear la tabla 'health_metrics' (SQLite).";
        return false;
    }

    qInfo() << "Base de datos SQLite inicializada correctamente en:" << dbFilePath;
    return true;
}

// Inicializa la base de datos para MariaDB/MySQL
bool DatabaseManager::initializeMariaDbDatabase(const QString& host, int port, const QString& dbName,
                                                const QString& user, const QString& password)
{
    m_currentDbType = MariaDB; // Establece el tipo de DB actual
    m_host = host;             // Guarda el host del servidor
    m_port = port;             // Guarda el puerto del servidor
    m_dbName = dbName;         // Guarda el nombre de la base de datos en el servidor
    m_user = user;             // Guarda el nombre de usuario
    m_password = password;     // Guarda la contraseña

    // Si ya existe una conexión con el nombre por defecto, la eliminamos.
    if (QSqlDatabase::contains(QSqlDatabase::defaultConnection)) {
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    }
    // Añade la base de datos usando el driver "QMYSQL" (usado para MySQL y MariaDB)
    m_db = QSqlDatabase::addDatabase("QMYSQL");

    // Intenta abrir la base de datos usando la función auxiliar interna
    if (!openDatabaseInternal()) {
        qCritical() << "Error: No se pudo abrir la base de datos MariaDB en la inicialización.";
        return false;
    }

    // Crea las tablas de la base de datos (requiere permisos en el servidor)
    if (!createUsersTable()) {
        qCritical() << "Error: No se pudo crear la tabla 'users' (MariaDB).";
        return false;
    }
    if (!createHealthMetricsTable()) {
        qCritical() << "Error: No se pudo crear la tabla 'health_metrics' (MariaDB).";
        return false;
    }

    qInfo() << "Base de datos MariaDB inicializada correctamente en" << host << ":" << port << "/" << dbName;
    return true;
}

   // Función auxiliar interna para abrir la conexión a la base de datos
   // Usa las variables miembro (m_currentDbType, m_dbName, etc.)
   bool DatabaseManager::openDatabaseInternal()
{
    if (m_db.isOpen()) {
        qInfo() << "La base de datos ya está abierta (verificación interna).";
        return true;
    }

    // Configura los parámetros de la base de datos según el tipo
    if (m_currentDbType == SQLite) {
        // Para SQLite, m_dbName es la ruta del archivo
        m_db.setDatabaseName(m_dbName);

        // Asegurarse de que el directorio exista para el archivo SQLite
        QFileInfo fi(m_dbName);
        QDir dir = fi.absoluteDir();
        if (!dir.exists()) {
            if (!dir.mkpath(".")) { // Crea la ruta si no existe
                qCritical() << "Error: No se pudo crear el directorio para la base de datos SQLite:" << dir.absolutePath();
                return false;
            }
        }
    } else if (m_currentDbType == MariaDB) {
        // Para MariaDB, configura host, puerto, nombre de DB, usuario y contraseña
        m_db.setHostName(m_host);
        m_db.setPort(m_port);
        m_db.setDatabaseName(m_dbName);
        m_db.setUserName(m_user);
        m_db.setPassword(m_password);
    } else {
        qCritical() << "Error interno: Tipo de base de datos no reconocido.";
        return false;
    }

    // Intenta abrir la conexión
    if (!m_db.open()) {
        qCritical() << "Error: No se pudo abrir la base de datos:" << m_db.lastError().text();
        return false;
    }

    qInfo() << "Conexión a la base de datos establecida correctamente.";
    return true;
}

// Cierra la conexión a la base de datos
void DatabaseManager::closeDatabase()
{
    if (m_db.isOpen()) {
        m_db.close();
        qInfo() << "Base de datos cerrada.";
    }
}

// Verifica si la base de datos está abierta
bool DatabaseManager::isDatabaseOpen() const
{
    return m_db.isOpen();
}


   // Crea la tabla 'users' si no existe
   bool DatabaseManager::createUsersTable()
{
    QSqlQuery query(m_db); // Usar m_db para asegurar que la consulta se haga en esta conexión
    QString createTableSql = "CREATE TABLE IF NOT EXISTS users (" // Nombre de tabla en snake_case
                             "user_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                             "first_name TEXT NOT NULL, "
                             "last_name1 TEXT NOT NULL, "
                             "last_name2 TEXT, "
                             "gender TEXT NOT NULL, "
                             "birth_date TEXT NOT NULL, "
                             "activity_level TEXT NOT NULL, "
                             "goal TEXT NOT NULL, "
                             "created_at TEXT DEFAULT CURRENT_TIMESTAMP"
                             ");";

    if (!query.exec(createTableSql)) {
        qCritical() << "Error al crear la tabla 'users':" << query.lastError().text();
        return false;
    }
    qInfo() << "Tabla 'users' asegurada/creada.";
    return true;
}

// Crea la tabla 'health_metrics' si no existe
bool DatabaseManager::createHealthMetricsTable()
{
    QSqlQuery query(m_db); // Usar m_db para asegurar que la consulta se haga en esta conexión
    QString createTableSql = "CREATE TABLE IF NOT EXISTS health_metrics (" // Nombre de tabla en snake_case
                             "metric_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                             "user_id INTEGER NOT NULL, "
                             "date TEXT NOT NULL, "
                             "weight REAL NOT NULL, "
                             "height REAL NOT NULL, "
                             "bmi REAL, "
                             "body_fat_percentage REAL, "
                             "muscle_mass_percentage REAL, "
                             "notes TEXT, "
                             "created_at TEXT DEFAULT CURRENT_TIMESTAMP, "
                             "FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE" // Referencia a la tabla 'users'
                             ");";

    if (!query.exec(createTableSql)) {
        qCritical() << "Error al crear la tabla 'health_metrics':" << query.lastError().text();
        return false;
    }
    qInfo() << "Tabla 'health_metrics' asegurada/creada.";
    return true;
}
