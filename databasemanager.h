#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include <QVariant> // Necesario para QSqlDatabase::addDatabase que a veces devuelve un QVariant

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    // Enum para definir los tipos de base de datos que soportamos
    enum DatabaseType {
        SQLite,
        MariaDB, // O MySQL, ya que usan el mismo driver QMYSQL
    };

    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    // Métodos de inicialización específicos para cada tipo de base de datos
    // Usa uno de estos según la base de datos que quieras emplear
    bool initializeSqliteDatabase(const QString& dbFilePath);
    bool initializeMariaDbDatabase(const QString& host, int port, const QString& dbName,
                                   const QString& user, const QString& password);

    // Métodos para cerrar y verificar el estado de la conexión
    void closeDatabase();
    bool isDatabaseOpen() const;

private:
    QSqlDatabase m_db; // El objeto principal de la base de datos de Qt
    DatabaseType m_currentDbType; // Guarda el tipo de base de datos que se está usando

    // Variables para almacenar los detalles de conexión, usados por openDatabaseInternal
    QString m_host;       // Para MariaDB/MySQL
    int m_port;           // Para MariaDB/MySQL
    QString m_dbName;     // Para MariaDB/MySQL (nombre de la DB) o SQLite (ruta del archivo)
    QString m_user;       // Para MariaDB/MySQL
    QString m_password;   // Para MariaDB/MySQL

    // Función auxiliar interna para abrir la base de datos
    // Utiliza las variables miembro (m_host, m_port, etc.) para la conexión
    bool openDatabaseInternal();

    // Métodos para crear las tablas de la base de datos
    // El SQL usado aquí es compatible con SQLite y MariaDB/MySQL
    bool createUsersTable();
    bool createHealthMetricsTable();
};

#endif // DATABASEMANAGER_H
