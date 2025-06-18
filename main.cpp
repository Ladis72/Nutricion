#include <QApplication> // Incluye la clase base para aplicaciones Qt
#include "mainwindow.h" // Incluye la cabecera de tu ventana principal
#include "databasemanager.h" // Incluye la cabecera de tu gestor de base de datos
#include <QDebug> // Para mensajes de depuración
#include <QStandardPaths> // Para obtener rutas estándar del sistema (útil para SQLite)
#include <QDir> // Para manejar directorios
#include <QMessageBox> // Para mostrar mensajes de error al usuario
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    // Crea una instancia de la aplicación Qt
    QApplication a(argc, argv);
    QLocale::setDefault(QLocale::c());

    // --- Cargar y aplicar Qt Style Sheet (QSS) desde los recursos ---
    // La ruta ahora usa ':/' seguido del prefijo definido en .qrc y el nombre del archivo.
    // Si tu prefijo en resources.qrc es '<qresource prefix="/">' y el archivo es '<file>styles.qss</file>',
    // entonces la ruta es ":/styles.qss".
    QFile styleFile(":/new/prefix1/styles.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream ts(&styleFile);
        QString styleSheet = ts.readAll();
        a.setStyleSheet(styleSheet); // Aplica el estilo a toda la aplicacion
        styleFile.close();
        qDebug() << "Estilo 'styles.qss' cargado correctamente desde recursos.";
    } else {
        qDebug() << "Advertencia: No se pudo abrir ':/styles.qss' desde recursos. La aplicacion se ejecutara con el estilo por defecto.";
    }
    // --- Fin de carga de QSS ---

    DatabaseManager dbManager;

    // --- ELIGE UNA DE LAS SIGUIENTES CONFIGURACIONES DE BASE DE DATOS ---
    // (Descomenta solo la opción que desees utilizar)

    // OPCIÓN 1: Configuración para SQLite (base de datos local en un archivo)
    // Es la opción más sencilla y no requiere un servidor de base de datos externo.

    QString appDirPath = QCoreApplication::applicationDirPath();
    QString dbFilePath = appDirPath+"/nutricion.db";
    qDebug() << "Ruta a la base de datos: " + dbFilePath;
    if (!dbManager.initializeSqliteDatabase(dbFilePath)) {
        // Si hay un error al inicializar, muestra un mensaje crítico y termina la aplicación
        QMessageBox::critical(nullptr, "Error de Base de Datos", "No se pudo inicializar la base de datos SQLite.");
        return 1; // Sale con código de error
    }

    // OPCIÓN 2: Configuración para MariaDB/MySQL (base de datos en un servidor)
    // Necesitas tener un servidor MariaDB/MySQL funcionando y los datos de conexión correctos.
    // Además, asegúrate de que el driver 'QMYSQL' de Qt esté disponible en tu sistema.
    /*
    QString mariaDbHost = "localhost"; // La dirección IP o nombre de host de tu servidor MariaDB
    int mariaDbPort = 3306;            // El puerto por defecto de MariaDB (3306)
    QString mariaDbName = "nutricion_db"; // El nombre de la base de datos que quieres usar en MariaDB
    QString mariaDbUser = "root";      // Tu usuario de MariaDB
    QString mariaDbPassword = "tu_contraseña"; // ¡IMPORTANTE! Cambia esto por tu contraseña real y no uses 'root' en producción sin una buena razón.

    // Si estás en Linux (Debian/Ubuntu), puede que necesites instalar: sudo apt-get install libqt5sql5-mysql
    // En Windows, los drivers suelen venir con el instalador de Qt, pero asegúrate de que la DLL de MariaDB/MySQL esté accesible.

    if (!dbManager.initializeMariaDbDatabase(mariaDbHost, mariaDbPort, mariaDbName, mariaDbUser, mariaDbPassword)) {
        QMessageBox::critical(nullptr, "Error de Base de Datos", "No se pudo inicializar la base de datos MariaDB.\n"
                                                                "Asegúrate de que el servidor esté funcionando, los datos de conexión sean correctos y el driver 'QMYSQL' esté disponible.");
        return 1;
    }
    */
    // --------------------------------------------------------------------

    // Crea y muestra la ventana principal de tu aplicación
    MainWindow w;
    w.show();

    // Inicia el bucle de eventos de la aplicación Qt
    return a.exec();
}
