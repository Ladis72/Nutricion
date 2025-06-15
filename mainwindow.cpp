#include "mainwindow.h"
#include "./ui_mainwindow.h" // Cabecera generada por Qt Designer a partir de mainwindow.ui
#include <QMessageBox>      // Para mostrar mensajes al usuario (éxito/error)
#include <QDebug>           // Para mensajes de depuración en la consola
#include <QTableWidgetItem> // Para manejar los elementos dentro de QTableWidget
#include <QHeaderView>      // Para ajustar el tamaño de las columnas de la tabla

// Constructor de la ventana principal
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) // Inicializa la interfaz de usuario
{
    ui->setupUi(this); // Configura todos los widgets definidos en mainwindow.ui

    // Inicializa nuestro gestor de usuarios.
    // 'this' es el padre, lo que asegura que userManager se destruya cuando MainWindow se destruya.
    userManager = new UserManager(this);

    // Configura los datos de los ComboBox (Género, Nivel de Actividad, Objetivo)
    setupComboBoxes();

    // Carga inicialmente todos los usuarios de la base de datos en la QTableWidget
    loadUsersIntoTable();

    connect(ui->lineEdit_searchUser, &QLineEdit::textChanged,
            this, &MainWindow::loadUsersIntoTable);

}

// Destructor de la ventana principal
MainWindow::~MainWindow()
{
    delete ui; // Libera la memoria de la interfaz de usuario
    // userManager se borrará automáticamente porque MainWindow es su padre
}

// Función auxiliar para configurar las opciones de los ComboBox
void MainWindow::setupComboBoxes() {
    // Género
    ui->comboBox_gender->addItem("Masculino");
    ui->comboBox_gender->addItem("Femenino");
    ui->comboBox_gender->addItem("Otro");

    // Nivel de Actividad
    ui->comboBox_activityLevel->addItem("Sedentario");
    ui->comboBox_activityLevel->addItem("Ligero");
    ui->comboBox_activityLevel->addItem("Moderado");
    ui->comboBox_activityLevel->addItem("Activo");
    ui->comboBox_activityLevel->addItem("Muy Activo");

    // Objetivo
    ui->comboBox_goal->addItem("Perder peso");
    ui->comboBox_goal->addItem("Mantener peso");
    ui->comboBox_goal->addItem("Ganar musculo");
    ui->comboBox_goal->addItem("Mejorar salud");
    ui->comboBox_goal->addItem("Otro");

    // Establecer la fecha de nacimiento por defecto a un valor razonable (ej. 1 de enero de 2000)
    ui->dateEdit_birthDate->setDate(QDate(2000, 1, 1));
}

// Slot que se activa cuando se hace clic en el botón "Añadir Usuario"
void MainWindow::on_pushButton_addUser_clicked()
{
    // 1. Recolectar datos de los widgets de la interfaz
    QString firstName = ui->lineEdit_firstName->text().trimmed(); // .trimmed() quita espacios al inicio/final
    QString lastName1 = ui->lineEdit_lastName1->text().trimmed();
    QString lastName2 = ui->lineEdit_lastName2->text().trimmed(); // Puede estar vacío
    QString gender = ui->comboBox_gender->currentText();
    QDate birthDate = ui->dateEdit_birthDate->date();
    QString activityLevel = ui->comboBox_activityLevel->currentText();
    QString goal = ui->comboBox_goal->currentText();

    // 2. Validación básica de los datos de entrada
    if (firstName.isEmpty() || lastName1.isEmpty() || gender.isEmpty() || birthDate.isNull() || activityLevel.isEmpty()) {
        QMessageBox::warning(this, "Entrada Inválida",
                             "Por favor, complete los campos obligatorios: Nombre, Primer Apellido, Género, Fecha de Nacimiento y Nivel de Actividad.");
        return; // Detiene la ejecución si los datos son inválidos
    }

    // 3. Crear un objeto User con los datos recolectados
    // Usamos el constructor para NUEVOS usuarios (sin ID, sin created_at, la DB los generará)
    User newUser(firstName, lastName1, lastName2, gender, birthDate, activityLevel, goal);

    // 4. Intentar añadir el usuario a la base de datos usando UserManager
    if (userManager->addUser(newUser)) {
        // Éxito: el usuario fue añadido
        QMessageBox::information(this, "Éxito",
                                 "Usuario '" + newUser.firstName() + " " + newUser.lastName1() + "' añadido correctamente con ID: " + QString::number(newUser.id()));

        // 5. Limpiar los campos del formulario para el siguiente registro
        ui->lineEdit_firstName->clear();
        ui->lineEdit_lastName1->clear();
        ui->lineEdit_lastName2->clear();
        // Opcional: restablecer ComboBox/DateEdit a valores por defecto o al primero
        ui->comboBox_gender->setCurrentIndex(0);
        ui->dateEdit_birthDate->setDate(QDate(2000, 1, 1));
        ui->comboBox_activityLevel->setCurrentIndex(0);
        ui->comboBox_goal->setCurrentIndex(0);

        // 6. Actualizar la tabla de usuarios para mostrar el nuevo usuario
        loadUsersIntoTable();
    } else {
        // Error: no se pudo añadir el usuario
        QMessageBox::critical(this, "Error", "No se pudo añadir el usuario a la base de datos. Revise los logs.");
    }
}

// Slot que se activa cuando se hace clic en el botón "Actualizar Lista"
void MainWindow::on_pushButton_refreshUsers_clicked()
{
    loadUsersIntoTable(); // Simplemente vuelve a llamar a la función para recargar los datos
}

void MainWindow::on_pushButton_deleteUser_clicked()
{
    // 1. Obtener la fila seleccionada en la tabla
    int currentRow = ui->tableWidget_users->currentRow();

    // Verificar si hay una fila seleccionada
    if (currentRow < 0) {
        QMessageBox::warning(this, "Selección Inválida", "Por favor, seleccione un usuario de la tabla para eliminar.");
        return;
    }

    // 2. Obtener el ID del usuario de la fila seleccionada
    // Asumimos que la columna 0 de la tabla contiene el ID del usuario
    int userIdToDelete = ui->tableWidget_users->item(currentRow, 0)->text().toInt();

    // 3. Pedir confirmación al usuario (¡IMPORTANTE!)
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmar Eliminación",
                                  "¿Está seguro de que desea eliminar al usuario con ID " + QString::number(userIdToDelete) + "?\n"
                                                                                                                              "¡Esta acción es irreversible!",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) {
        return; // El usuario canceló la eliminación
    }

    // 4. Intentar eliminar el usuario usando UserManager
    if (userManager->deleteUser(userIdToDelete)) {
        QMessageBox::information(this, "Éxito", "Usuario con ID " + QString::number(userIdToDelete) + " eliminado correctamente.");
        // 5. Actualizar la tabla después de la eliminación
        loadUsersIntoTable();
    } else {
        QMessageBox::critical(this, "Error", "No se pudo eliminar el usuario. Revise los logs.");
    }
}

// Función auxiliar para cargar usuarios desde la base de datos y mostrarlos en QTableWidget
void MainWindow::loadUsersIntoTable(const QString &searchTerm) {
    QVector<User> allUsers = userManager->getAllUsers();
    QVector<User> filteredUsers; // Obtiene todos los usuarios de la base de datos
    if (!searchTerm.isEmpty()) {
        QString lowerSearchTerm = searchTerm.toLower();
        for (const User& user : allUsers) {
            if (user.firstName().toLower().contains(lowerSearchTerm) ||
                user.lastName1().toLower().contains(lowerSearchTerm) ||
                user.lastName2().toLower().contains(lowerSearchTerm))
            {
                filteredUsers.append(user);
            }
        }
    } else {
        filteredUsers = allUsers;
    }
    // 1. Configurar la QTableWidget
    ui->tableWidget_users->setRowCount(filteredUsers.size()); // Establece el número de filas según la cantidad de usuarios
    ui->tableWidget_users->setColumnCount(8);        // 8 columnas: ID, Nombre, Apellido 1, Apellido 2, Género, Nacimiento, Actividad, Objetivo
    // Establece los encabezados de las columnas
    ui->tableWidget_users->setHorizontalHeaderLabels({"ID", "Nombre", "Apellido 1", "Apellido 2", "Género", "Nacimiento", "Actividad", "Objetivo"});
    // Oculta los encabezados verticales (números de fila)
    ui->tableWidget_users->verticalHeader()->setVisible(false);
    // Permite que el usuario redimensione las columnas y se ajusten al contenido
    ui->tableWidget_users->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    // Hace que la última sección se estire para llenar el espacio restante
    ui->tableWidget_users->horizontalHeader()->setStretchLastSection(true);


    // 2. Llenar la tabla con los datos de cada usuario
    for (int i = 0; i < filteredUsers.size(); ++i) {
        const User& user = filteredUsers.at(i); // Obtiene una referencia constante al usuario actual

        ui->tableWidget_users->setItem(i, 0, new QTableWidgetItem(QString::number(user.id())));
        ui->tableWidget_users->setItem(i, 1, new QTableWidgetItem(user.firstName()));
        ui->tableWidget_users->setItem(i, 2, new QTableWidgetItem(user.lastName1()));
        ui->tableWidget_users->setItem(i, 3, new QTableWidgetItem(user.lastName2())); // El segundo apellido puede estar vacío
        ui->tableWidget_users->setItem(i, 4, new QTableWidgetItem(user.gender()));
        ui->tableWidget_users->setItem(i, 5, new QTableWidgetItem(user.birthDate().toString("yyyy-MM-dd"))); // Formatear la fecha
        ui->tableWidget_users->setItem(i, 6, new QTableWidgetItem(user.activityLevel()));
        ui->tableWidget_users->setItem(i, 7, new QTableWidgetItem(user.goal()));
    }
}
