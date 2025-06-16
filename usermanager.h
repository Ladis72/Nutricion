#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QObject>
#include <QVector>
#include <QSharedPointer>
#include "user.h" // Incluimos nuestra clase User

class UserManager : public QObject {
    Q_OBJECT
public:
    explicit UserManager(QObject *parent = nullptr);

    // Operaciones CRUD
    bool addUser(User& user); // Añade un nuevo usuario, el ID se actualizará en el objeto 'user'
    QList<QSharedPointer<User>> getAllUsers(); // Obtiene todos los usuarios
    QSharedPointer<User> getUserById(int userId); // Obtiene un usuario por su ID
    bool updateUser(const User& user); // Actualiza los datos de un usuario existente
    bool deleteUser(int userId); // Elimina un usuario por su ID

private:
         // No necesitamos una QSqlDatabase miembro aquí, usaremos la conexión por defecto.
};

#endif // USERMANAGER_H
