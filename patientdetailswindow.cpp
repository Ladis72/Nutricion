// patientdetailswindow.cpp
#include "patientdetailswindow.h"
#include "addmetricdialog.h"
#include "healthmetricmanager.h"
#include "ui_patientdetailswindow.h" // Incluye el archivo generado por Qt Designer
#include <QDebug>
#include <QMessageBox> // Para mostrar mensajes de error
#include <QTableWidgetItem> // Para elementos de QTableWidget
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>
#include <QDateTime>
#include <QDateTimeAxis>

PatientDetailsWindow::PatientDetailsWindow(QSharedPointer<User> patient, QWidget *parent)
    : QWidget(parent),
    ui(new Ui::PatientDetailsWindow), // Inicializa el puntero inteligente de UI
    m_currentPatient(patient),
    weightChart(nullptr),
    weightSeries(nullptr),
    weightAxisX(nullptr),
    weightAxisY(nullptr),
    bmiChart(nullptr),
    bmiSeries(nullptr),
    bmiAxisX(nullptr),
    bmiAxisY(nullptr)
// Almacena el paciente recibido
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
    setupCharts();
    updateCharts();
}

PatientDetailsWindow::~PatientDetailsWindow()
{
    // QScopedPointer se encarga de eliminar ui automáticamente
}

// Configuración inicial de los elementos de la UI
void PatientDetailsWindow::setupUi()
{
    // Por ahora, solo configuraremos la tabla de métricas de salud
    ui->healthMetricsTableWidget->setColumnCount(9); // Por ejemplo: Fecha, Peso, Altura, IMC, Grasa%, Músculo%, Notas, Creado
    ui->healthMetricsTableWidget->setHorizontalHeaderLabels({
        "Fecha", "Peso (kg)", "Altura (cm)", "IMC", "Grasa (%)", "Músculo (%)", "Notas", "Creado En", "ID"
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
    this->setWindowTitle((QString("Paciente: %1 %2 %3")).arg(m_currentPatient->firstName(),
                                                             m_currentPatient->lastName1(),
                                                             m_currentPatient->lastName2()));
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
            ui->healthMetricsTableWidget->setItem(i, 8, new QTableWidgetItem(QString::number(metric->id())));
        }
    }
    ui->healthMetricsTableWidget->hideColumn(8);
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

void PatientDetailsWindow::on_pushButtonEditar_clicked()
{
    // 1. Obtener la fila seleccionada
    int currentRow = ui->healthMetricsTableWidget->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Selección inválida", "Por favor seleccione una medición para editar.");
        return;
    }

    // 2. Obtener el ID de la métrica seleccionada (columna 8)
    int metricId = ui->healthMetricsTableWidget->item(currentRow, 8)->text().toInt();

    // 3. Obtener la métrica desde la base de datos
    HealthMetric originalMetric = m_healthMetricManager.getHealthMetric(metricId);

    if (originalMetric.id() == 0) {
        QMessageBox::critical(this, "Error", "No se pudo cargar la métrica seleccionada.");
        return;
    }
    // 4. Crear el diálogo con los valores actuales
    AddMetricDialog dialog(
        originalMetric.date(),
        originalMetric.weight(),
        originalMetric.height(),
        originalMetric.bodyFatPercentage(),
        originalMetric.muscleMassPercentage(),
        originalMetric.notes(),
        this
        );

    // 5. Mostrar el diálogo y esperar que el usuario lo acepte
    if (dialog.exec() == QDialog::Accepted) {
        // 6. Obtener los datos actualizados del diálogo
        originalMetric.setDate(dialog.getDate());
        originalMetric.setWeight(dialog.getWeight());
        originalMetric.setHeight(dialog.getHeight());
        originalMetric.setBodyFatPercentage(dialog.getBodyFatPercentage());
        originalMetric.setMuscleMassPercentage(dialog.getMuscleMassPercentage());
        originalMetric.setNotes(dialog.getNotes());

        // 7. Recalcular el BMI
        originalMetric.calculateBmi();

        // 8. Actualizar en la base de datos
        if (m_healthMetricManager.updateHealthMetric(originalMetric)) {
            QMessageBox::information(this, "Éxito", "Métrica actualizada correctamente.");
            loadHealthMetrics();  // Recargar la tabla
            ui->healthMetricsTableWidget->selectRow(currentRow);
            ui->healthMetricsTableWidget->scrollToItem(
                ui->healthMetricsTableWidget->item(currentRow,0),
                QAbstractItemView::PositionAtCenter);
        } else {
            QMessageBox::critical(this, "Error", "No se pudo actualizar la métrica.");
        }
    }
}

void PatientDetailsWindow::on_pushButtonBorrar_clicked()
{
    // 1. Obtener la fila seleccionada
    int currrentRow = ui->healthMetricsTableWidget->currentRow();

    // 2. Verificar si existe
    if (currrentRow < 0) {
        QMessageBox::warning(this, "Selección ionválida", "Por favor seleccione una medidición para eliminar");
        return;
    }
    // 3. Obtener el Id del la medición
    int metricToDelete = ui->healthMetricsTableWidget->item(currrentRow,8)->text().toInt();
    // 4. Pedir confirmación
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,"Confirmar eliminación","¿Está seguro?.\n Esta operación no de puede deshacer.",
                                                                     QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) {
        return;
    }
    // 5. Intentar eliminar la medición
    if (m_healthMetricManager.deleteHealthMetric(metricToDelete)) {
        QMessageBox::information(this,"Exito", "Metrica borrada con éxito");
        loadHealthMetrics();
    }else{
        QMessageBox::critical(this, "Error", "No se ha podido eliminar la métrica.");
    }
}

void PatientDetailsWindow::setupCharts()
{
    //Grafica del peso
    weightChart = new QChart();
    weightChart->setTitle("Evolución del peso");
    ui->widgetWeight->setChart(weightChart);
    ui->widgetWeight->setRenderHint(QPainter::Antialiasing);

    weightSeries = new QLineSeries(weightChart);
    weightSeries->setName("Peso (Kg=");
    weightChart->addSeries(weightSeries);

    //Eje X Fecha hora
    weightAxisX = new QDateTimeAxis(weightChart);
    weightAxisX->setFormat("dd/MM/yyyy");
    weightAxisX->setTitleText("Fecha");
    weightChart->addAxis(weightAxisX, Qt::AlignBottom);
    weightSeries->attachAxis(weightAxisX);

    //Eje Y peso
    weightAxisY = new QValueAxis(weightChart);
    weightAxisY->setLabelFormat("%.1f Kg");
    weightAxisY->setTitleText("Peso");
    weightChart->addAxis(weightAxisY, Qt::AlignLeft);
    weightSeries->attachAxis(weightAxisY);

    // --- Configuracion Grafica de IMC ---
    bmiChart = new QChart();
    bmiChart->setTitle("Evolucion del IMC");
    ui->widgetBMI->setChart(bmiChart); // Asigna el QChart al QChartView
    ui->widgetBMI->setRenderHint(QPainter::Antialiasing); // Suavizar lineas

    bmiSeries = new QLineSeries(bmiChart); // La serie tiene el chart como padre
    bmiSeries->setName("IMC");
    bmiChart->addSeries(bmiSeries);

    // Eje X: Fecha y Hora (QDateTimeAxis)
    bmiAxisX = new QDateTimeAxis(bmiChart);
    bmiAxisX->setFormat("dd/MM/yyyy");
    bmiAxisX->setTitleText("Fecha");
    bmiChart->addAxis(bmiAxisX, Qt::AlignBottom);
    bmiSeries->attachAxis(bmiAxisX);

    // Eje Y: IMC (QValueAxis)
    bmiAxisY = new QValueAxis(bmiChart);
    bmiAxisY->setLabelFormat("%.1f");
    bmiAxisY->setTitleText("IMC");
    bmiChart->addAxis(bmiAxisY, Qt::AlignLeft);
    bmiSeries->attachAxis(bmiAxisY);

    // Leyendas (opcional, pero mejora la claridad)
    weightChart->legend()->setVisible(true);
    weightChart->legend()->setAlignment(Qt::AlignBottom);
    bmiChart->legend()->setVisible(true);
    bmiChart->legend()->setAlignment(Qt::AlignBottom);


}

void PatientDetailsWindow::updateCharts()
{
    weightSeries->clear(); // Limpiar datos anteriores
    bmiSeries->clear();

    QList<QPointF> weightDataPoints; // Para almacenar pares (fecha_milisegundos, peso)
    QList<QPointF> bmiDataPoints;    // Para almacenar pares (fecha_milisegundos, imc)
    QDateTime minDate, maxDate;
    qreal minWeight = std::numeric_limits<qreal>::max();
    qreal maxWeight = std::numeric_limits<qreal>::min();
    qreal minBmi = std::numeric_limits<qreal>::max();
    qreal maxBmi = std::numeric_limits<qreal>::min();

    // Columnas relevantes en healthMetricsTableWidget
    const int DATE_COL = 0; // Columna 'Fecha'
    const int WEIGHT_COL = 1; // Columna 'Peso (kg)'
    const int BMI_COL = 3; // Columna 'IMC'

    // Iterar sobre las filas del QTableWidget para recopilar datos
    for (int i = 0; i < ui->healthMetricsTableWidget->rowCount(); ++i) {
        QTableWidgetItem *dateItem = ui->healthMetricsTableWidget->item(i, DATE_COL);
        QTableWidgetItem *weightItem = ui->healthMetricsTableWidget->item(i, WEIGHT_COL);
        QTableWidgetItem *bmiItem = ui->healthMetricsTableWidget->item(i, BMI_COL);

        if (dateItem && weightItem && bmiItem) {
            // Intentar convertir la cadena de la fecha a QDateTime.
            // Asegurarse de que el formato de fecha coincida con el que se guarda en la tabla.
            QDateTime dateTime = QDateTime::fromString(dateItem->text(), Qt::ISODate);
            double weight = weightItem->text().toDouble();
            double imc = bmiItem->text().toDouble();

            // Asegurarse de que la fecha sea valida y los valores sean sensatos para la grafica
            if (dateTime.isValid() && weight > 0 && imc > 0) {
                // Guardar los datos como QPointF (X=milisegundos desde epoch, Y=valor)
                // QDateTimeAxis entiende los milisegundos desde epoch para su rango y etiquetas.
                weightDataPoints.append(QPointF(dateTime.toMSecsSinceEpoch(), weight));
                bmiDataPoints.append(QPointF(dateTime.toMSecsSinceEpoch(), imc));

                // Actualizar rangos de los ejes para auto-escalado
                if (minDate.isNull() || dateTime < minDate) minDate = dateTime;
                if (maxDate.isNull() || dateTime > maxDate) maxDate = dateTime;
                if (weight < minWeight) minWeight = weight;
                if (weight > maxWeight) maxWeight = weight;
                if (imc < minBmi) minBmi = imc;
                if (imc > maxBmi) maxBmi = imc;
            }
        }
    }

    // Ordenar los puntos por fecha (coordenada X) para asegurar que la linea sea continua y correcta
    std::sort(weightDataPoints.begin(), weightDataPoints.end(), [](const QPointF& a, const QPointF& b){
        return a.x() < b.x();
    });
    std::sort(bmiDataPoints.begin(), bmiDataPoints.end(), [](const QPointF& a, const QPointF& b){
        return a.x() < b.x();
    });

    // Anadir los puntos ordenados a las series de las graficas
    for (const auto& point : weightDataPoints) {
        weightSeries->append(point);
    }
    for (const auto& point : bmiDataPoints) {
        bmiSeries->append(point);
    }

    // Ajustar los rangos de los ejes solo si hay datos para mostrar
    if (!weightDataPoints.isEmpty()) {
        // Rangos para el Eje X (Fechas)
        // Agrega un pequeno margen para que los puntos no esten en el borde
        qint64 minMs = minDate.toMSecsSinceEpoch();
        qint64 maxMs = maxDate.toMSecsSinceEpoch();
        if (minMs == maxMs) { // Si solo hay un punto, extendemos el rango un dia a cada lado
            minMs -= 86400000; // 24 horas en milisegundos
            maxMs += 86400000;
        } else {
            // Anadir un 5% de margen temporal a cada lado para una mejor visualizacion
            qint64 marginMs = (maxMs - minMs) * 0.05;
            minMs -= marginMs;
            maxMs += marginMs;
        }

        // Establecer el rango de los ejes de fecha
        weightAxisX->setRange(QDateTime::fromMSecsSinceEpoch(minMs), QDateTime::fromMSecsSinceEpoch(maxMs));
        bmiAxisX->setRange(QDateTime::fromMSecsSinceEpoch(minMs), QDateTime::fromMSecsSinceEpoch(maxMs));

        // Rangos para los Ejes Y (Valores)
        // Anadir un margen del 5% arriba y abajo para que los puntos no toquen los limites
        weightAxisY->setRange(minWeight * 0.95, maxWeight * 1.05);
        bmiAxisY->setRange(minBmi * 0.95, maxBmi * 1.05);

    } else {
        // Si no hay datos, restablecer los ejes a un rango por defecto visible o mostrar un mensaje
        // Esto evita graficos vacios o ejes sin sentido.
        weightAxisX->setRange(QDateTime::currentDateTime().addMonths(-1), QDateTime::currentDateTime().addDays(1));
        weightAxisY->setRange(0, 100); // Ejemplo de rango para peso (0-100 kg)
        bmiAxisX->setRange(QDateTime::currentDateTime().addMonths(-1), QDateTime::currentDateTime().addDays(1));
        bmiAxisY->setRange(0, 40);   // Ejemplo de rango para IMC (0-40)
    }
}
