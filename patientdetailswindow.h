// patientdetailswindow.h
#ifndef PATIENTDETAILSWINDOW_H
#define PATIENTDETAILSWINDOW_H

#include <QWidget> // La clase base para esta ventana
#include <QScopedPointer> // Para gestionar la UI de forma segura

// Incluimos las clases que vamos a necesitar
#include "user.h" // Para recibir el objeto User
#include "healthmetricmanager.h" // Para gestionar las métricas de salud
#include "healtmetric.h"

// Declaración forward para la interfaz de usuario generada por Qt Designer
namespace Ui {
class PatientDetailsWindow;
}

class PatientDetailsWindow : public QWidget
{
    Q_OBJECT

public:
    // Constructor que recibe un QSharedPointer<User> para el paciente y el parent
    explicit PatientDetailsWindow(QSharedPointer<User> patient, QWidget *parent = nullptr);
    ~PatientDetailsWindow();

private:
    // Puntero inteligente para la interfaz de usuario
    QScopedPointer<Ui::PatientDetailsWindow> ui;

    // Almacena el paciente actual que se está visualizando
    QSharedPointer<User> m_currentPatient;

    // Gestor de métricas de salud para este paciente
    HealthMetricManager m_healthMetricManager;

    // Métodos privados para configurar la interfaz y cargar datos
    void setupUi();
    void loadPatientData();
    void loadHealthMetrics();
};

#endif // PATIENTDETAILSWINDOW_H
