#ifndef HEALTHMETRIC_H
#define HEALTHMETRIC_H

#include <QString>
#include <QDate>
#include <QDateTime>

class HealthMetric {
public:
    // Constructor para una métrica existente (con ID y created_at)
    HealthMetric(int id, int userId, const QDate& date, double weight, double height,
                 double bmi, double bodyFatPercentage, double muscleMassPercentage,
                 const QString& notes, const QDateTime& createdAt)
        : m_id(id), m_userId(userId), m_date(date), m_weight(weight), m_height(height),
        m_bmi(bmi), m_bodyFatPercentage(bodyFatPercentage),
        m_muscleMassPercentage(muscleMassPercentage), m_notes(notes), m_createdAt(createdAt) {}

    // Constructor para una nueva métrica (sin ID ni created_at, la DB los generará)
    HealthMetric(int userId, const QDate& date, double weight, double height,
                 double bodyFatPercentage, double muscleMassPercentage, const QString& notes , const QDateTime time_at)
        : m_id(-1), m_userId(userId), m_date(date), m_weight(weight), m_height(height),
        m_bmi(0.0), // El BMI se calculará en el setter o antes de la DB
        m_bodyFatPercentage(bodyFatPercentage),
        m_muscleMassPercentage(muscleMassPercentage), m_notes(notes), m_createdAt(time_at) {}

    // Getters
    int id() const { return m_id; }
    int userId() const { return m_userId; }
    QDate date() const { return m_date; }
    double weight() const { return m_weight; }
    double height() const { return m_height; }
    double bmi() const { return m_bmi; }
    double bodyFatPercentage() const { return m_bodyFatPercentage; }
    double muscleMassPercentage() const { return m_muscleMassPercentage; }
    QString notes() const { return m_notes; }
    QDateTime createdAt() const { return m_createdAt; }

    // Setters (para permitir modificación si es necesario)
    void setId(int id) { m_id = id; }
    void setUserId(int userId) { m_userId = userId; }
    void setDate(const QDate& date) { m_date = date; }
    void setWeight(double weight) { m_weight = weight; }
    void setHeight(double height) { m_height = height; }
    void setBmi(double bmi) { m_bmi = bmi; }
    void setBodyFatPercentage(double bodyFatPercentage) { m_bodyFatPercentage = bodyFatPercentage; }
    void setMuscleMassPercentage(double muscleMassPercentage) { m_muscleMassPercentage = muscleMassPercentage; }
    void setNotes(const QString& notes) { m_notes = notes; }
    void setCreatedAt(const QDateTime& createdAt) { m_createdAt = createdAt; }

    // Función de ayuda para calcular el BMI
    void calculateBmi() {
        if (m_weight > 0 && m_height > 0) {
            // BMI = peso (kg) / (altura (m))^2
            // Si la altura está en cm, se divide por 100 para convertir a metros
            double heightInMeters = m_height / 100.0;
            m_bmi = m_weight / (heightInMeters * heightInMeters);
        } else {
            m_bmi = 0.0;
        }
    }

private:
    int m_id;
    int m_userId;
    QDate m_date;
    double m_weight;
    double m_height;
    double m_bmi;
    double m_bodyFatPercentage;
    double m_muscleMassPercentage;
    QString m_notes;
    QDateTime m_createdAt;
};

#endif // HEALTHMETRIC_H
