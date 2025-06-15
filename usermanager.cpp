#include "usermanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant> // Para usar bindValue

UserManager::UserManager(QObject *parent) : QObject(parent) {
    // La conexión a la base de datos se asume que ya está abierta
    // y es la conexión por defecto, manejada por DatabaseManager.
}

bool UserManager::addUser(User& user) {
    QSqlQuery query;
    // Adaptamos la consulta SQL a las nuevas columnas de nombre
    query.prepare("INSERT INTO Users (first_name, last_name1, last_name2, gender, birth_date, activity_level, goal) "
                  "VALUES (:first_name, :last_name1, :last_name2, :gender, :birth_date, :activity_level, :goal)");

    query.bindValue(":first_name", user.firstName());
    query.bindValue(":last_name1", user.lastName1());
    query.bindValue(":last_name2", user.lastName2().isEmpty() ? QVariant(QVariant::String) : user.lastName2()); // Manejar NULL para segundo apellido
    query.bindValue(":gender", user.gender());
    query.bindValue(":birth_date", user.birthDate().toString("yyyy-MM-dd"));
    query.bindValue(":activity_level", user.activityLevel());
    query.bindValue(":goal", user.goal());

    if (!query.exec()) {
        qCritical() << "Error al añadir usuario:" << query.lastError().text();
        return false;
    }

    int lastId = query.lastInsertId().toInt();
    user.setId(lastId); // Actualizamos el ID del objeto User

    // También actualizamos la fecha de creación si es importante tenerla en el objeto tras la inserción
    // QDateTime::fromString() puede necesitar el formato si no es ISO estándar perfecto del DB
    // Para SQLite TEXT DEFAULT CURRENT_TIMESTAMP, suele ser "YYYY-MM-DD HH:MM:SS"
    // Para DATETIME de MySQL/MariaDB, es formato "YYYY-MM-DD HH:MM:SS"
    QSqlQuery selectQuery;
    selectQuery.prepare("SELECT created_at FROM Users WHERE user_id = :user_id");
    selectQuery.bindValue(":user_id", lastId);
    if (selectQuery.exec() && selectQuery.next()) {
        user.setCreatedAt(selectQuery.value("created_at").toDateTime());
    }

    qInfo() << "Usuario añadido con ID:" << lastId;
    return true;
}

QVector<User> UserManager::getAllUsers() {
    QVector<User> users;
    // Adaptar la consulta a las nuevas columnas
    QSqlQuery query("SELECT user_id, first_name, last_name1, last_name2, gender, birth_date, activity_level, goal, created_at FROM Users");

    if (!query.exec()) {
        qCritical() << "Error al obtener todos los usuarios:" << query.lastError().text();
        return users;
    }

    while (query.next()) {
        int id = query.value("user_id").toInt();
        QString firstName = query.value("first_name").toString();
        QString lastName1 = query.value("last_name1").toString();
        QString lastName2 = query.value("last_name2").toString(); // Puede ser vacía si es NULL en DB
        QString gender = query.value("gender").toString();
        QDate birthDate = QDate::fromString(query.value("birth_date").toString(), "yyyy-MM-dd");
        QString activityLevel = query.value("activity_level").toString();
        QString goal = query.value("goal").toString();
        QDateTime createdAt = query.value("created_at").toDateTime(); // toDateTime() es bastante robusto

        users.append(User(id, firstName, lastName1, lastName2, gender, birthDate, activityLevel, goal, createdAt));
    }
    return users;
}

std::optional<User> UserManager::getUserById(int userId) {
    QSqlQuery query;
    // Adaptar la consulta
    query.prepare("SELECT user_id, first_name, last_name1, last_name2, gender, birth_date, activity_level, goal, created_at FROM Users WHERE user_id = :user_id");
    query.bindValue(":user_id", userId);

    if (!query.exec()) {
        qCritical() << "Error al obtener usuario por ID:" << query.lastError().text();
        return std::nullopt; // Retorna un usuario inválido
    }

    if (query.next()) {
        int id = query.value("user_id").toInt();
        QString firstName = query.value("first_name").toString();
        QString lastName1 = query.value("last_name1").toString();
        QString lastName2 = query.value("last_name2").toString();
        QString gender = query.value("gender").toString();
        QDate birthDate = QDate::fromString(query.value("birth_date").toString(), "yyyy-MM-dd");
        QString activityLevel = query.value("activity_level").toString();
        QString goal = query.value("goal").toString();
        QDateTime createdAt = query.value("created_at").toDateTime();

        return User(id, firstName, lastName1, lastName2, gender, birthDate, activityLevel, goal, createdAt);
    }
    return std::nullopt; // No se encontró el usuario
}

bool UserManager::updateUser(const User& user) {
    if (user.id() == -1) {
        qWarning() << "No se puede actualizar un usuario sin ID válido.";
        return false;
    }

    QSqlQuery query;
    // Adaptar la consulta
    query.prepare("UPDATE Users SET first_name = :first_name, last_name1 = :last_name1, last_name2 = :last_name2, "
                  "gender = :gender, birth_date = :birth_date, activity_level = :activity_level, goal = :goal "
                  "WHERE user_id = :user_id");

    query.bindValue(":first_name", user.firstName());
    query.bindValue(":last_name1", user.lastName1());
    query.bindValue(":last_name2", user.lastName2().isEmpty() ? QVariant(QVariant::String) : user.lastName2());
    query.bindValue(":gender", user.gender());
    query.bindValue(":birth_date", user.birthDate().toString("yyyy-MM-dd"));
    query.bindValue(":activity_level", user.activityLevel());
    query.bindValue(":goal", user.goal());
    query.bindValue(":user_id", user.id());

    if (!query.exec()) {
        qCritical() << "Error al actualizar usuario:" << query.lastError().text();
        return false;
    }

    qInfo() << "Usuario actualizado con ID:" << user.id();
    return query.numRowsAffected() > 0;
}

bool UserManager::deleteUser(int userId) {
    QSqlQuery query;
    query.prepare("DELETE FROM Users WHERE user_id = :user_id");
    query.bindValue(":user_id", userId);

    if (!query.exec()) {
        qCritical() << "Error al eliminar usuario:" << query.lastError().text();
        return false;
    }

    qInfo() << "Usuario eliminado con ID:" << userId;
    return query.numRowsAffected() > 0;
}
