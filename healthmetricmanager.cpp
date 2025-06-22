#include "healthmetricmanager.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

HealthMetricManager::HealthMetricManager(QObject *parent) : QObject(parent)
{
    // El constructor es simple; el DatabaseManager ya se encarga de abrir la conexión.
}

// Implementación para añadir una nueva métrica de salud
bool HealthMetricManager::addHealthMetric(const HealthMetric& metric)
{
    QSqlQuery query;
    query.prepare("INSERT INTO health_metrics (user_id, date, weight, height, bmi, body_fat_percentage, muscle_mass_percentage, created_at, notes) "
                  "VALUES (:user_id, :date, :weight, :height, :bmi, :body_fat_percentage, :muscle_mass_percentage, :created_at, :notes)");

    // Vincula los valores de la métrica a los placeholders de la consulta
    query.bindValue(":user_id", metric.userId());
    query.bindValue(":date", metric.date().toString(Qt::ISODate)); // Formato ISO para la fecha (YYYY-MM-DD)
    query.bindValue(":weight", metric.weight());
    query.bindValue(":height", metric.height());
    query.bindValue(":bmi", metric.bmi());
    query.bindValue(":body_fat_percentage", metric.bodyFatPercentage());
    query.bindValue(":muscle_mass_percentage", metric.muscleMassPercentage());
    query.bindValue(":created_at", metric.createdAt());
    query.bindValue(":notes", metric.notes());

    if (!query.exec()) {
        qCritical() << "Error al añadir métrica de salud:" << query.lastError().text();
        qDebug() << query.lastError();
        return false;
    }

    qInfo() << "Métrica de salud añadida correctamente para el usuario ID:" << metric.userId();
    return true;
}

// Implementación para obtener métricas de salud por ID de usuario (CORREGIDA)
QList<QSharedPointer<HealthMetric>> HealthMetricManager::getHealthMetricsByUserId(int userId)
{
    QList<QSharedPointer<HealthMetric>> metrics;
    QSqlQuery query;
    query.prepare("SELECT metric_id, user_id, date, weight, height, bmi, body_fat_percentage, muscle_mass_percentage, notes, created_at " // Añadido created_at
                  "FROM health_metrics WHERE user_id = :user_id ORDER BY date ASC, created_at ASC"); // Ordenar por fecha y luego por hora de creación
    query.bindValue(":user_id", userId);

    if (!query.exec()) {
        qCritical() << "Error al obtener métricas de salud por usuario ID:" << query.lastError().text();
        return metrics; // Retorna lista vacía en caso de error
    }

    while (query.next()) {
        // Extraer todos los valores de la fila de la base de datos
        int id = query.value("metric_id").toInt();
        int retrievedUserId = query.value("user_id").toInt();
        QDate date = QDate::fromString(query.value("date").toString(), Qt::ISODate);
        double weight = query.value("weight").toDouble();
        double height = query.value("height").toDouble();
        double bmi = query.value("bmi").toDouble();
        double bodyFatPercentage = query.value("body_fat_percentage").toDouble();
        double muscleMassPercentage = query.value("muscle_mass_percentage").toDouble();
        QString notes = query.value("notes").toString();
        QDateTime createdAt = query.value("created_at").toDateTime();

        // Ahora, creamos el QSharedPointer y pasamos TODOS los argumentos
        // al constructor de HealthMetric que recibe todos los campos (el primero en HealthMetric.h)
        metrics.append(QSharedPointer<HealthMetric>::create(
            id, retrievedUserId, date, weight, height, bmi, bodyFatPercentage, muscleMassPercentage, notes, createdAt
            ));
    }

    qInfo() << "Obtenidas" << metrics.count() << "métricas de salud para el usuario ID:" << userId;
    return metrics;
}

// Implementación para actualizar una métrica de salud existente
bool HealthMetricManager::updateHealthMetric(const HealthMetric& metric)
{
    if (metric.id() <= 0) { // Usar metric.id() para el ID de la métrica
        qWarning() << "No se puede actualizar la métrica: ID de métrica no válido.";
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE health_metrics SET "
                  "user_id = :user_id, "
                  "date = :date, "
                  "weight = :weight, "
                  "height = :height, "
                  "bmi = :bmi, "
                  "body_fat_percentage = :body_fat_percentage, "
                  "muscle_mass_percentage = :muscle_mass_percentage, "
                  "notes = :notes "
                  "WHERE metric_id = :metric_id"); // Usar metric_id para el WHERE

    query.bindValue(":user_id", metric.userId());
    query.bindValue(":date", metric.date().toString(Qt::ISODate));
    query.bindValue(":weight", metric.weight());
    query.bindValue(":height", metric.height());
    query.bindValue(":bmi", metric.bmi());
    query.bindValue(":body_fat_percentage", metric.bodyFatPercentage());
    query.bindValue(":muscle_mass_percentage", metric.muscleMassPercentage());
    query.bindValue(":notes", metric.notes());
    query.bindValue(":metric_id", metric.id()); // Clave para la actualización

    if (!query.exec()) {
        qCritical() << "Error al actualizar métrica de salud con ID" << metric.id() << ":" << query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() == 0) {
        qWarning() << "Métrica de salud con ID" << metric.id() << "no encontrada para actualizar.";
        return false; // No se actualizó ninguna fila
    }

    qInfo() << "Métrica de salud con ID" << metric.id() << "actualizada correctamente.";
    return true;
}

// Implementación para eliminar una métrica de salud
bool HealthMetricManager::deleteHealthMetric(int metricId)
{
    if (metricId <= 0) {
        qWarning() << "No se puede eliminar la métrica: ID de métrica no válido.";
        return false;
    }

    QSqlQuery query;
    query.prepare("DELETE FROM health_metrics WHERE metric_id = :metric_id");
    query.bindValue(":metric_id", metricId);

    if (!query.exec()) {
        qCritical() << "Error al eliminar métrica de salud con ID" << metricId << ":" << query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() == 0) {
        qWarning() << "Métrica de salud con ID" << metricId << "no encontrada para eliminar.";
        return false; // No se eliminó ninguna fila
    }

    qInfo() << "Métrica de salud con ID" << metricId << "eliminada correctamente.";
    return true;
}

HealthMetric HealthMetricManager::getHealthMetric(int metricId)
{
    HealthMetric metrics;
    QSqlQuery query;
    query.prepare("SELECT metric_id, user_id, date, weight, height, bmi, body_fat_percentage, muscle_mass_percentage, notes, created_at " // Añadido created_at
                  "FROM health_metrics WHERE metric_id = :metric_id ");
    query.bindValue(":metric_id", metricId);
    if (!query.exec()) {
        qCritical() << "Error al devolver las metricas";
        return metrics;
    }
    query.first();
    metrics.setId(query.value("metric_id").toInt());
    metrics.setUserId(query.value("user_id").toInt());
    metrics.setDate(QDate::fromString(query.value("date").toString(),Qt::ISODate));
    metrics.setWeight(query.value("weight").toDouble());
    metrics.setHeight(query.value("height").toDouble());
    metrics.setBmi(query.value("bmi").toDouble());
    metrics.setBodyFatPercentage(query.value("body_fat_percentage").toDouble());
    metrics.setMuscleMassPercentage(query.value("muscle_mass_percentage").toDouble());
    metrics.setNotes(query.value("notes").toString());
    metrics.setCreatedAt(query.value("created_at").toDateTime());
    return metrics;
}
