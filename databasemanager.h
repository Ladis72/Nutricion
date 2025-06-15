#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QtSql/QSqlDatabase>
#include <QString>
#include <QObject>

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManager(QObject *parent = nullptr);
    // Método estático para inicializar y abrir la conexión a la base de datos
    static bool initializeDatabase(const QString& dbType, const QString& dbNameOrHost = "",
                                   const QString& username = "", const QString& password = "");

    // Método estático para cerrar la conexión
    static void closeDatabase();

    // Método estático para crear las tablas de la base de datos
    static void createTables();

signals:
};

#endif // DATABASEMANAGER_H
