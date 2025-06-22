#ifndef ADDMETRICDIALOG_H
#define ADDMETRICDIALOG_H

#include <QDialog>
#include <QDate>

// No necesitas QDoubleValidator si usas QDoubleSpinBox
// #include <QDoubleSpinBox> // Normalmente no es necesario incluir aquí si ya está en ui_addmetricdialog.h

namespace Ui {
class AddMetricDialog;
}

class AddMetricDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddMetricDialog(QWidget *parent = nullptr);
    explicit AddMetricDialog(QDate date, double weight, double height,
                             double bodyFat, double muscleMass, QString notes,
                             QWidget *parent = nullptr);
    ~AddMetricDialog();

    // Métodos para obtener los datos introducidos (sin cambios)
    QDate getDate() const;
    double getWeight() const;
    double getHeight() const;
    double getBodyFatPercentage() const;
    double getMuscleMassPercentage() const;
    QString getNotes() const;

private slots:
    void on_okButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::AddMetricDialog *ui;
};

#endif // ADDMETRICDIALOG_H
