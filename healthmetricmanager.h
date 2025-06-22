#ifndef HEALTHMETRICMANAGER_H
#define HEALTHMETRICMANAGER_H

#include <QObject>
#include <QList> // Para almacenar listas de objetos HealthMetric
#include <QSharedPointer> // Para manejar objetos HealthMetric de forma segura

// Asegúrate de incluir la definición de HealthMetric
#include "healtmetric.h"

class HealthMetricManager : public QObject
{
    Q_OBJECT

public:
    explicit HealthMetricManager(QObject *parent = nullptr);

    // Guarda una nueva métrica de salud en la base de datos.
    // Retorna true si tiene éxito, false si falla.
    bool addHealthMetric(const HealthMetric& metric);

    // Obtiene todas las métricas de salud para un usuario específico.
    // Retorna una lista de punteros compartidos a HealthMetric.
    // Usamos QSharedPointer para gestionar la memoria de forma segura.
    QList<QSharedPointer<HealthMetric>> getHealthMetricsByUserId(int userId);

    // Actualiza una métrica de salud existente en la base de datos.
    // La métrica debe tener un metric_id válido.
    // Retorna true si tiene éxito, false si falla.
    bool updateHealthMetric(const HealthMetric& metric);

    // Elimina una métrica de salud por su ID.
    // Retorna true si tiene éxito, false si falla.
    bool deleteHealthMetric(int metricId);

    // Obtiene una metrica por id
    HealthMetric getHealthMetric (int metricId);

private:
         // No necesitamos una conexión QSqlDatabase aquí directamente,
         // ya que trabajaremos con la conexión predeterminada que DatabaseManager ya abrió.
         // Sin embargo, sí usaremos QSqlQuery.
};

#endif // HEALTHMETRICMANAGER_H
