#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>

void DBhandle::createTable()
{
 QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
 db.setDatabaseName("FBDateBase");
 if(!db.open())qDebug() << "Error: " <<db.lastError();

 QSqlQuery query("CREATE TABLE calls (id INTEGER PRIMARY KEY, value TEXT)");

 if(!query.isActive()) qDebug() << "Error: " << query.lastError().text();

 qDebug() << "Table created";

}

DBhandle::DBhandle()
{
    createTable();
}

void DBhandle::DBadd(QString text)
{
    QString q = QString("insert into calls(value) VALUES(%1)").arg(text);
    QSqlQuery query(q);
    if(!query.exec()) qDebug() << query.lastError().text();
}
