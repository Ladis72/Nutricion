#ifndef USER_H
#define USER_H

#include <QString>
#include <QDate>
#include <QDateTime> // Para created_at

class User {
public:
    User(QObject *parent);

    // Constructor para un usuario existente (con ID y fecha de creación)
    User(int id, const QString& firstName, const QString& lastName1, const QString& lastName2,
         const QString& gender, const QDate& birthDate, const QString& activityLevel,
         const QString& goal, const QDateTime& createdAt)
        : m_id(id), m_firstName(firstName), m_lastName1(lastName1), m_lastName2(lastName2),
        m_gender(gender), m_birthDate(birthDate), m_activityLevel(activityLevel),
        m_goal(goal), m_createdAt(createdAt) {}

    // Constructor para un nuevo usuario (sin ID, será autoincremental en DB, y sin created_at, será DEFAULT CURRENT_TIMESTAMP)
    User(const QString& firstName, const QString& lastName1, const QString& lastName2,
         const QString& gender, const QDate& birthDate, const QString& activityLevel,
         const QString& goal)
        : m_id(-1), m_firstName(firstName), m_lastName1(lastName1), m_lastName2(lastName2),
        m_gender(gender), m_birthDate(birthDate), m_activityLevel(activityLevel),
        m_goal(goal), m_createdAt(QDateTime()) {} // QDateTime por defecto o inválida

    // Getters
    int id() const { return m_id; }
    QString firstName() const { return m_firstName; }
    QString lastName1() const { return m_lastName1; }
    QString lastName2() const { return m_lastName2; }
    QString gender() const { return m_gender; }
    QDate birthDate() const { return m_birthDate; }
    QString activityLevel() const { return m_activityLevel; }
    QString goal() const { return m_goal; }
    QDateTime createdAt() const { return m_createdAt; }

    // Setters (para permitir modificación de datos)
    void setId(int id) { m_id = id; } // Usado al insertar y recuperar el ID generado
    void setFirstName(const QString& firstName) { m_firstName = firstName; }
    void setLastName1(const QString& lastName1) { m_lastName1 = lastName1; }
    void setLastName2(const QString& lastName2) { m_lastName2 = lastName2; }
    void setGender(const QString& gender) { m_gender = gender; }
    void setBirthDate(const QDate& birthDate) { m_birthDate = birthDate; }
    void setActivityLevel(const QString& activityLevel) { m_activityLevel = activityLevel; }
    void setGoal(const QString& goal) { m_goal = goal; }
    void setCreatedAt(const QDateTime& createdAt) { m_createdAt = createdAt; }

private:
    int m_id;
    QString m_firstName;
    QString m_lastName1;
    QString m_lastName2;
    QString m_gender;
    QDate m_birthDate;
    QString m_activityLevel;
    QString m_goal;
    QDateTime m_createdAt;
};

#endif // USER_H
