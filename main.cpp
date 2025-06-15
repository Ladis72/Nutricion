#include <QApplication>
#include <QDebug> // Para los mensajes de depuración
#include "mainwindow.h"
#include "databasemanager.h" // Incluye tu clase DatabaseManager

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // --- Configuración de la base de datos ---
    // Puedes elegir SQLite (para desarrollo local y despliegue simple)
    // o MariaDB (si necesitas un backend de servidor)

    // Opción 1: Usar SQLite
    // Se creará un archivo 'nutrition_health.db' en el directorio de compilación
    if (!DatabaseManager::initializeDatabase("SQLite", "nutrition_health.db")) {
        qCritical() << "¡Error fatal: No se pudo inicializar la base de datos SQLite! La aplicación no puede continuar.";
        return -1; // Termina la aplicación si la DB no se abre
    }
    qInfo() << "Base de datos SQLite inicializada correctamente.";

    // Opción 2: Usar MariaDB/MySQL (descomenta y configura si lo necesitas)
    /*
    // Asegúrate de que el servidor MariaDB/MySQL esté corriendo y la base de datos 'nutrition_db' exista
    // Reemplaza "localhost", "your_user", "your_password" con tus credenciales
    if (!DatabaseManager::initializeDatabase("MariaDB", "localhost", "your_user", "your_password")) {
        qCritical() << "¡Error fatal: No se pudo inicializar la base de datos MariaDB! La aplicación no puede continuar.";
        return -1; // Termina la aplicación si la DB no se abre
    }
    qInfo() << "Base de datos MariaDB inicializada correctamente.";
    */


    // --- Ejecución de la Interfaz de Usuario ---
    MainWindow w;
    w.show();

    int result = a.exec();

    // --- Cerrar la conexión a la base de datos al finalizar la aplicación ---
    DatabaseManager::closeDatabase();

    return result;
}
