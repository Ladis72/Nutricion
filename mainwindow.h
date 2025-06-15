#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include "usermanager.h" // Incluimos UserManager
#include "user.h"        // Incluimos User

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT // Macro necesaria para el sistema de señales y slots de Qt

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Slot que se conectará al clic del botón "Añadir Usuario"
    void on_pushButton_addUser_clicked();
    // Slot que se conectará al clic del botón "Actualizar Lista"
    void on_pushButton_refreshUsers_clicked();
    // Slot para eliminar usuario
    void on_pushButton_deleteUser_clicked();

private:
    Ui::MainWindow *ui;
    UserManager *userManager; // Puntero a nuestra instancia de UserManager

    // Función auxiliar para cargar los usuarios de la base de datos en la tabla
    void loadUsersIntoTable(const QString& searchTerm = QString());
    // Función auxiliar para configurar los QComboBox con opciones predefinidas
    void setupComboBoxes();
};

#endif // MAINWINDOW_H
