#include "addmetricdialog.h"
#include "ui_addmetricdialog.h"
#include <QMessageBox>
// Ya no necesitas QDoubleValidator

AddMetricDialog::AddMetricDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddMetricDialog)
{
    ui->setupUi(this);

    // Conectar botones
    connect(ui->okButton, &QPushButton::clicked, this, &AddMetricDialog::on_okButton_clicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &AddMetricDialog::on_cancelButton_clicked);

    // *** Configuración de QDoubleSpinBox ***
    // Peso (kg)
    ui->weightSpinBox->setSuffix(" kg"); // Sufijo para la unidad
    ui->weightSpinBox->setRange(1.0, 300.0); // Rango típico para peso
    ui->weightSpinBox->setDecimals(2); // Dos decimales de precisión
    ui->weightSpinBox->setSingleStep(0.1); // Incremento por defecto de 0.1

    // Altura (cm)
    ui->heightSpinBox->setSuffix(" cm");
    ui->heightSpinBox->setRange(50.0, 250.0); // Rango típico para altura
    ui->heightSpinBox->setDecimals(1); // Un decimal (o 0 si prefieres solo cm enteros)
    ui->heightSpinBox->setSingleStep(0.5);

    // Grasa (%)
    ui->bodyFatSpinBox->setSuffix(" %");
    ui->bodyFatSpinBox->setRange(0.0, 100.0); // Porcentaje de 0 a 100
    ui->bodyFatSpinBox->setDecimals(2);
    ui->bodyFatSpinBox->setSingleStep(0.1);

    // Músculo (%)
    ui->muscleMassSpinBox->setSuffix(" %");
    ui->muscleMassSpinBox->setRange(0.0, 100.0); // Porcentaje de 0 a 100
    ui->muscleMassSpinBox->setDecimals(2);
    ui->muscleMassSpinBox->setSingleStep(0.1);

    // Inicializar la fecha con la fecha actual
    ui->dateTimeEdit->setDate(QDate::currentDate());
    ui->dateTimeEdit->setTime(QTime::currentTime());


    // Opcional: Establecer valores por defecto (si tienes un caso de uso para ello)
    // ui->weightSpinBox->setValue(70.0);
    // ui->heightSpinBox->setValue(170.0);
}

AddMetricDialog::~AddMetricDialog()
{
    delete ui;
}

void AddMetricDialog::on_okButton_clicked()
{
    // *** Validación simplificada gracias a QDoubleSpinBox ***
    // Ahora solo necesitamos verificar si los valores están dentro de un rango razonable
    // El QDoubleSpinBox ya se encarga de la entrada numérica
    if (ui->weightSpinBox->value() <= 0 || ui->heightSpinBox->value() <= 0) {
        QMessageBox::warning(this, "Datos Inválidos", "El peso y la altura deben ser valores positivos.");
        return;
    }

    accept();
}

void AddMetricDialog::on_cancelButton_clicked()
{
    reject();
}

// Implementación de los métodos "getter" (¡Ahora usan .value() en lugar de .text().toDouble()!)
QDate AddMetricDialog::getDate() const {
    return ui->dateTimeEdit->date();
}

double AddMetricDialog::getWeight() const {
    return ui->weightSpinBox->value();
}

double AddMetricDialog::getHeight() const {
    return ui->heightSpinBox->value();
}

double AddMetricDialog::getBodyFatPercentage() const {
    return ui->bodyFatSpinBox->value();
}

double AddMetricDialog::getMuscleMassPercentage() const {
    return ui->muscleMassSpinBox->value();
}

QString AddMetricDialog::getNotes() const {
    return ui->notesTextEdit->toPlainText();
}
