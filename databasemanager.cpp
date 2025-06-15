#include "databasemanager.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject{parent}
{}

bool DatabaseManager::initializeDatabase(const QString &dbType, const QString &dbNameOrHost,
                                         const QString &username, const QString &password)
{
    QSqlDatabase db;
    // Si ya existe la conexión por defecto, la cerramos y removemos para evitar problemas
    if (QSqlDatabase::contains(QSqlDatabase::defaultConnection)) {
        db = QSqlDatabase::database(QSqlDatabase::defaultConnection);
        if (db.isOpen()) {
            db.close();
        }
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    }

    if (dbType == "SQLite") {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(dbNameOrHost); // dbNameOrHost será el path al archivo .db
    } else if (dbType == "MariaDB" || dbType == "MySQL") {
        // Asegúrate de que el driver QMYSQL esté disponible
        db = QSqlDatabase::addDatabase("QMYSQL");
        db.setHostName(dbNameOrHost); // Host (ej. "localhost")
        db.setPort(3306); // Puerto por defecto de MySQL/MariaDB
        db.setDatabaseName("nutricion"); // Asegúrate de que esta DB exista en tu servidor
        db.setUserName(username);
        db.setPassword(password);
    } else {
        qCritical() << "Tipo de base de datos no soportado:" << dbType;
        return false;
    }

    if (!db.open()) {
        qCritical() << "Error al abrir la base de datos:" << db.lastError().text();
        return false;
    }

    qInfo() << "Base de datos conectada con éxito:" << dbType;

    // Crear tablas si no existen
    createTables();

    return true;
}

void DatabaseManager::closeDatabase() {
    QSqlDatabase db = QSqlDatabase::database(QSqlDatabase::defaultConnection);
    if (db.isOpen()) {
        db.close();
        qInfo() << "Conexión a la base de datos cerrada.";
    }
    QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
}

void DatabaseManager::createTables() {
    QSqlQuery query;
    QStringList createStatements;

    // Sentencias para la creación de tablas (usando la sintaxis compatible)

    // Users Table
    createStatements << "CREATE TABLE IF NOT EXISTS Users ("
                        "user_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                        "first_name TEXT NOT NULL,"
                        "last_name1 TEXT NOT NULL,"
                        "last_name2 TEXT,"
                        "gender TEXT NOT NULL,"
                        "birth_date TEXT NOT NULL,"
                        "activity_level TEXT NOT NULL,"
                        "goal TEXT,"
                        "created_at TEXT DEFAULT CURRENT_TIMESTAMP"
                        ");";

    // HealthMetricsTypes Table
    createStatements << "CREATE TABLE IF NOT EXISTS HealthMetricsTypes ("
                        "metric_type_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                        "name TEXT NOT NULL UNIQUE,"
                        "unit TEXT NOT NULL,"
                        "is_user_defined INTEGER DEFAULT 0,"
                        "created_at TEXT DEFAULT CURRENT_TIMESTAMP"
                        ");";

    // HealthMetricsLogEntry Table
    createStatements << "CREATE TABLE IF NOT EXISTS HealthMetricsLogEntry ("
                        "log_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                        "user_id INTEGER NOT NULL,"
                        "metric_type_id INTEGER NOT NULL,"
                        "log_date TEXT NOT NULL,"
                        "value REAL NOT NULL,"
                        "notes TEXT,"
                        "recorded_at TEXT DEFAULT CURRENT_TIMESTAMP,"
                        "FOREIGN KEY (user_id) REFERENCES Users(user_id) ON DELETE CASCADE,"
                        "FOREIGN KEY (metric_type_id) REFERENCES HealthMetricsTypes(metric_type_id) ON DELETE RESTRICT"
                        ");";

    // --- Tablas de Alimentación (las agregaremos más adelante) ---
    // FoodItems Table (placeholder)
    createStatements << "CREATE TABLE IF NOT EXISTS FoodItems ("
                        "food_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                        "name TEXT NOT NULL UNIQUE,"
                        "calories_per_100g REAL NOT NULL,"
                        "protein_per_100g REAL NOT NULL,"
                        "carbs_per_100g REAL NOT NULL,"
                        "fat_per_100g REAL NOT NULL,"
                        "unit TEXT NOT NULL,"
                        "is_custom INTEGER DEFAULT 0"
                        ");";

    // FoodLog Table (placeholder)
    createStatements << "CREATE TABLE IF NOT EXISTS FoodLog ("
                        "log_entry_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                        "user_id INTEGER NOT NULL,"
                        "food_id INTEGER NOT NULL,"
                        "log_date TEXT NOT NULL,"
                        "meal_type TEXT NOT NULL,"
                        "quantity REAL NOT NULL,"
                        "consumed_at TEXT DEFAULT CURRENT_TIMESTAMP,"
                        "FOREIGN KEY (user_id) REFERENCES Users(user_id) ON DELETE CASCADE,"
                        "FOREIGN KEY (food_id) REFERENCES FoodItems(food_id) ON DELETE CASCADE"
                        ");";

    for (const QString& statement : createStatements) {
        if (!query.exec(statement)) {
            qCritical() << "Error al crear tabla o ejecutar statement:" << query.lastError().text() << "\nSQL:" << statement;
        } else {
            qInfo() << "Statement ejecutado con éxito o tabla ya existe.";
        }
    }
}
