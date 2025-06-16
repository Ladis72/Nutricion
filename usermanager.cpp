#include "usermanager.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant> // Necesario para QSqlQuery::value()

UserManager::UserManager(QObject *parent) : QObject(parent)
{
    // Constructor. La conexión a la base de datos es manejada por DatabaseManager.
}

// Añade un nuevo usuario a la base de datos.
// El ID del objeto 'user' se actualizará si la inserción es exitosa.
bool UserManager::addUser(User& user)
{
    QSqlQuery query;
    query.prepare("INSERT INTO users (first_name, last_name1, last_name2, gender, birth_date, activity_level, goal) "
                  "VALUES (:first_name, :last_name1, :last_name2, :gender, :birth_date, :activity_level, :goal)");

    query.bindValue(":first_name", user.firstName());
    query.bindValue(":last_name1", user.lastName1());
    query.bindValue(":last_name2", user.lastName2());
    query.bindValue(":gender", user.gender());
    query.bindValue(":birth_date", user.birthDate().toString(Qt::ISODate)); // Almacena la fecha en formato ISO
    query.bindValue(":activity_level", user.activityLevel());
    query.bindValue(":goal", user.goal());

    if (!query.exec()) {
        qCritical() << "Error al añadir usuario:" << query.lastError().text();
        return false;
    }

    // Si la inserción fue exitosa, recupera el ID autogenerado
    if (query.lastInsertId().isValid()) {
        user.setId(query.lastInsertId().toInt()); // Asigna el ID de vuelta al objeto User
        qInfo() << "Usuario añadido correctamente con ID:" << user.id();
        return true;
    }

    qCritical() << "Error: Usuario añadido, pero no se pudo recuperar el ID generado.";
    return false;
}

// Recupera todos los usuarios de la base de datos.
QList<QSharedPointer<User>> UserManager::getAllUsers()
{
    QList<QSharedPointer<User>> users;
    QSqlQuery query("SELECT user_id, first_name, last_name1, last_name2, gender, birth_date, activity_level, goal, created_at FROM users ORDER BY first_name ASC");

    if (!query.exec()) {
        qCritical() << "Error getting all users:" << query.lastError().text();
        return users; // Devuelve una lista vacía en caso de error
    }

    while (query.next()) {
        users.append(QSharedPointer<User>::create(
            query.value("user_id").toInt(),
            query.value("first_name").toString(),
            query.value("last_name1").toString(),
            query.value("last_name2").toString(),
            query.value("gender").toString(),
            QDate::fromString(query.value("birth_date").toString(), Qt::ISODate),
            query.value("activity_level").toString(),
            query.value("goal").toString(),
            query.value("created_at").toDateTime()
            ));
    }
    qInfo() << "Se recuperaron" << users.count() << "usuarios.";
    return users;
}

// Actualiza un usuario existente en la base de datos.
bool UserManager::updateUser(const User& user)
{
    if (user.id() <= 0) {
        qWarning() << "No se puede actualizar el usuario: ID de usuario no válido.";
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE users SET "
                  "first_name = :first_name, last_name1 = :last_name1, last_name2 = :last_name2, "
                  "gender = :gender, birth_date = :birth_date, activity_level = :activity_level, goal = :goal "
                  "WHERE user_id = :user_id");

    query.bindValue(":first_name", user.firstName());
    query.bindValue(":last_name1", user.lastName1());
    query.bindValue(":last_name2", user.lastName2());
    query.bindValue(":gender", user.gender());
    query.bindValue(":birth_date", user.birthDate().toString(Qt::ISODate));
    query.bindValue(":activity_level", user.activityLevel());
    query.bindValue(":goal", user.goal());
    query.bindValue(":user_id", user.id());

    if (!query.exec()) {
        qCritical() << "Error al actualizar usuario con ID" << user.id() << ":" << query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() == 0) {
        qWarning() << "Usuario con ID" << user.id() << "no encontrado para actualizar.";
        return false;
    }

    qInfo() << "Usuario con ID" << user.id() << "actualizado correctamente.";
    return true;
}

// Elimina un usuario de la base de datos por su ID.
bool UserManager::deleteUser(int id)
{
    if (id <= 0) {
        qWarning() << "No se puede eliminar el usuario: ID de usuario no válido.";
        return false;
    }

    QSqlQuery query;
    query.prepare("DELETE FROM users WHERE user_id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qCritical() << "Error al eliminar usuario con ID" << id << ":" << query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() == 0) {
        qWarning() << "Usuario con ID" << id << "no encontrado para eliminar.";
        return false;
    }

    qInfo() << "Usuario con ID" << id << "eliminado correctamente.";
    return true;
}

// Recupera un único usuario por su ID.
// Devuelve un QSharedPointer<User> o un QSharedPointer nulo si no se encuentra o hay un error.
QSharedPointer<User> UserManager::getUserById(int id)
{
    QSqlQuery query;
    query.prepare("SELECT user_id, first_name, last_name1, last_name2, gender, birth_date, activity_level, goal, created_at "
                  "FROM users WHERE user_id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qCritical() << "Error al obtener usuario por ID:" << query.lastError().text();
        return QSharedPointer<User>(); // Devuelve un puntero nulo en caso de error
    }

    if (query.next()) {
        // Crea un QSharedPointer a un nuevo objeto User usando su constructor completo
        QSharedPointer<User> user = QSharedPointer<User>::create(
            query.value("user_id").toInt(),
            query.value("first_name").toString(),
            query.value("last_name1").toString(),
            query.value("last_name2").toString(),
            query.value("gender").toString(),
            QDate::fromString(query.value("birth_date").toString(), Qt::ISODate),
            query.value("activity_level").toString(),
            query.value("goal").toString(),
            query.value("created_at").toDateTime()
            );
        qInfo() << "Usuario con ID" << id << "recuperado correctamente.";
        return user;
    } else {
        qWarning() << "Usuario con ID" << id << "no encontrado en la base de datos.";
        return QSharedPointer<User>(); // Devuelve un puntero nulo si el usuario no se encuentra
    }
}
