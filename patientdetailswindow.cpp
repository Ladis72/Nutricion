// patientdetailswindow.cpp
#include "patientdetailswindow.h"
#include "ui_patientdetailswindow.h" // Incluye el archivo generado por Qt Designer
#include <QDebug>
#include <QMessageBox> // Para mostrar mensajes de error
#include <QTableWidgetItem> // Para elementos de QTableWidget

PatientDetailsWindow::PatientDetailsWindow(QSharedPointer<User> patient, QWidget *parent)
    : QWidget(parent),
    ui(new Ui::PatientDetailsWindow), // Inicializa el puntero inteligente de UI
    m_currentPatient(patient) // Almacena el paciente recibido
{
    ui->setupUi(this); // Configura la interfaz de usuario desde el archivo .ui

    // Asegurarse de que tenemos un paciente válido
    if (!m_currentPatient) {
        QMessageBox::critical(this, "Error", "No se ha proporcionado un paciente válido para mostrar.");
        close(); // Cierra la ventana si no hay paciente
        return;
    }
//connect(ui->addMetricButton, &QPushButton::clicked, this, &PatientDetailsWindow::on_addMetricButton_clicked);
    // Configurar y cargar los datos del paciente
    setupUi();          // Configuración inicial de la UI (columnas de tabla, etc.)
    loadPatientData();  // Carga los datos básicos del paciente
    loadHealthMetrics(); // Carga y muestra las métricas de salud
}

PatientDetailsWindow::~PatientDetailsWindow()
{
    // QScopedPointer se encarga de eliminar ui automáticamente
}

// Configuración inicial de los elementos de la UI
void PatientDetailsWindow::setupUi()
{
    // Por ahora, solo configuraremos la tabla de métricas de salud
    ui->healthMetricsTableWidget->setColumnCount(8); // Por ejemplo: Fecha, Peso, Altura, IMC, Grasa%, Músculo%, Notas, Creado
    ui->healthMetricsTableWidget->setHorizontalHeaderLabels({
        "Fecha", "Peso (kg)", "Altura (cm)", "IMC", "Grasa (%)", "Músculo (%)", "Notas", "Creado En"
    });
    // Ajustar columnas al contenido
    ui->healthMetricsTableWidget->horizontalHeader()->setStretchLastSection(true);
    // Hacer que la tabla no sea editable directamente por el usuario
    ui->healthMetricsTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // Seleccionar filas completas
    ui->healthMetricsTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->healthMetricsTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
}

// Carga los datos básicos del paciente en las etiquetas correspondientes
void PatientDetailsWindow::loadPatientData()
{
    this->setWindowTitle((QString("Paciente: %1 %2 %3")).arg(m_currentPatient->firstName())
                             .arg(m_currentPatient->lastName1())
                             .arg(m_currentPatient->lastName2()));
    // ui->patientIdLabel->setText(QString("ID: %1").arg(m_currentPatient->id()));
    // Puedes añadir más etiquetas aquí para mostrar otros datos del paciente
    ui->genderLabel->setText(QString("Género: %1").arg(m_currentPatient->gender()));
    ui->dobLabel->setText(QString("Nacimiento: %1").arg(m_currentPatient->birthDate().toString(Qt::ISODate)));
    ui->activityLevelLabel->setText(QString("Nivel Act.: %1").arg(m_currentPatient->activityLevel()));
    ui->goalLabel->setText(QString("Objetivo: %1").arg(m_currentPatient->goal()));
}

// Carga las métricas de salud del paciente y las muestra en la tabla
void PatientDetailsWindow::loadHealthMetrics()
{
    ui->healthMetricsTableWidget->setRowCount(0); // Limpiar filas existentes

    if (!m_currentPatient) {
        qWarning() << "No hay paciente para cargar métricas de salud.";
        return;
    }

    // Obtener las métricas del gestor de métricas
    QList<QSharedPointer<HealthMetric>> metrics = m_healthMetricManager.getHealthMetricsByUserId(m_currentPatient->id());

    ui->healthMetricsTableWidget->setRowCount(metrics.count()); // Establecer el número de filas de la tabla

    for (int i = 0; i < metrics.count(); ++i) {
        QSharedPointer<HealthMetric> metric = metrics.at(i);
        if (metric) { // Asegurarse de que el puntero no sea nulo
            ui->healthMetricsTableWidget->setItem(i, 0, new QTableWidgetItem(metric->date().toString(Qt::ISODate)));
            ui->healthMetricsTableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(metric->weight(), 'f', 2)));
            ui->healthMetricsTableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(metric->height(), 'f', 2)));
            ui->healthMetricsTableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(metric->bmi(), 'f', 2)));
            ui->healthMetricsTableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(metric->bodyFatPercentage(), 'f', 2)));
            ui->healthMetricsTableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(metric->muscleMassPercentage(), 'f', 2)));
            ui->healthMetricsTableWidget->setItem(i, 6, new QTableWidgetItem(metric->notes()));
            ui->healthMetricsTableWidget->setItem(i, 7, new QTableWidgetItem(metric->createdAt().toString(Qt::ISODate)));
        }
    }
    ui->healthMetricsTableWidget->resizeColumnsToContents(); // Ajustar el ancho de las columnas
}

void PatientDetailsWindow::on_addMetricButton_clicked()
{
    // 1. Crear una instancia del diálogo de entrada de métricas.
    AddMetricDialog dialog(this);

    // 2. Mostrar el diálogo y esperar a que el usuario interactúe.
    if (dialog.exec() == QDialog::Accepted) {
        // 3. Si el usuario hizo clic en "Aceptar", obtener los datos del diálogo.
        QDate date = dialog.getDate();
        double weight = dialog.getWeight();
        double height = dialog.getHeight();
        double bodyFat = dialog.getBodyFatPercentage();
        double muscleMass = dialog.getMuscleMassPercentage();
        QString notes = dialog.getNotes();

        // 4. ¡Perfecto! Crear un objeto HealthMetric usando el constructor para nuevas métricas.
        //    Aquí, BMI se inicializa a 0.0 y createdAt a una QDateTime nula por tu constructor.
        HealthMetric newMetric(
            m_currentPatient->id(), // ID del paciente actual
            date,
            weight,
            height,
            bodyFat,
            muscleMass,
            notes,
            QDateTime::currentDateTime()
            );

        // 5. ¡Perfecto! Calcular el BMI usando el método de la propia clase HealthMetric.
        newMetric.calculateBmi();

        // 6. ¡CORRECCIÓN AQUÍ! Pasar el objeto HealthMetric directamente al manager
        //    por referencia constante (const HealthMetric&).
        //    El manager se encargará de copiar sus datos o almacenarlos en la DB.
        bool success = m_healthMetricManager.addHealthMetric(newMetric); // <--- ¡CAMBIO AQUÍ!

        // 7. Mostrar un mensaje de éxito o error al usuario.
        if (success) {
            QMessageBox::information(this, "Éxito", "Métrica de salud añadida correctamente.");
            // 8. ¡Importante! Recargar la tabla para que la nueva métrica aparezca visible.
            loadHealthMetrics();
        } else {
            QMessageBox::critical(this, "Error", "No se pudo añadir la métrica de salud. Verifique la base de datos.");
        }
    }
}
