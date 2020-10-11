#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"
#include <typeinfo>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){
    ui->setupUi(this);

    //Setup the DB
    CreateDB();
    //set the model for viewing
    this->open_tickets_model = new QSqlQueryModel;
    //set the table to the model
    ui->open_tickets_view->setModel(open_tickets_model);
    //set what gets displayed in the table
    this->open_tickets_model->setQuery("SELECT source,open,close from tickets WHERE close IS NULL");

    this->node_list_model = new QSqlQueryModel;
    ui->node_list_view->setModel(node_list_model);
    this->node_list_model->setQuery("SELECT * from nodes;");

}


MainWindow::~MainWindow()
{
    myDB.close();
    delete ui;
}

void MainWindow::on_Add_clicked(){

    QSqlQuery query;
    query.prepare("SELECT * FROM tickets WHERE source = :source_ID AND close IS NULL");
    query.bindValue(":source_ID", ui->TXT_ID->text().toInt());
    if(query.exec() && query.first()){//found

        int ticket = query.value(0).toInt();
        query.prepare("UPDATE tickets SET close = CURRENT_TIMESTAMP WHERE ID = :ticket_nr;");
        query.bindValue(":ticket_nr", ticket);
        if(query.exec())qDebug() << "Ticket Closed";
        else qDebug() << query.lastError();
    }
    else { //not found
        query.prepare("INSERT INTO tickets(source) VALUES (:source_ID);");
        query.bindValue(":source_ID", ui->TXT_ID->text().toInt());
        if(query.exec())qDebug() << "Ticket Opened";
        else qDebug() << query.lastError();
    }
    open_tickets_model->setQuery(open_tickets_model->query().executedQuery());

}

void MainWindow::CreateDB()
{
    myDB = QSqlDatabase::addDatabase("QSQLITE");
    myDB.setDatabaseName("myDB.db");
    if(myDB.open()){
        ui->statusBar->showMessage("DBOpened",1000);
        myDB.exec("PRAGMA foreign_keys = ON;");
    }
    else{
        ui->statusBar->showMessage("DB Failed To Open",10000);
        qWarning() << "DBOpened: " << myDB.lastError();
    }
}

void MainWindow::on_add_node_clicked()
{
    QSqlQuery query;
    query.prepare("SELECT * FROM nodes WHERE ID = :source_ID");
    query.bindValue(":source_ID", ui->TXT_ID->text().toInt());
    if(query.exec() && query.first()){//found
        qDebug() << "that node already exists";
    }
    else { //not found
        query.prepare("INSERT INTO nodes(ID) VALUES (:source_ID);");
        query.bindValue(":source_ID", ui->TXT_ID->text().toInt());
        if(query.exec())qDebug() << "Node Added";
        else qDebug() << query.lastError();
    }
    node_list_model->setQuery(node_list_model->query().executedQuery());

}


void MainWindow::on_del_node_clicked()
{
    QSqlQuery query;
    query.prepare("SELECT * FROM nodes WHERE ID = :source_ID");
    query.bindValue(":source_ID", ui->TXT_ID->text().toInt());
    if(query.exec() && query.first()){//found
        query.prepare("DELETE from nodes WHERE ID = (:source_ID);");
        query.bindValue(":source_ID", ui->TXT_ID->text().toInt());
        if(query.exec())qDebug() << "Node Removed";
        else qDebug() << query.lastError();
    }
    else { //not found

        qDebug() << "that does not exists";

    }
    node_list_model->setQuery(node_list_model->query().executedQuery());
    open_tickets_model->setQuery(open_tickets_model->query().executedQuery());


}
