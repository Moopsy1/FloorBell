#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"
#include <QSerialPortInfo>
#include <typeinfo>
#include <QtWidgets>

bool Ticket(int node);

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
    qDebug() << node_list_model->lastError();

    arduino = new QSerialPort(this);
    arduino_is_available = false;
    serialBuffer = "";
    parsed_data = "";
    arduino_port_name = "";
    //    qDebug() << "Nr of available ports:" << QSerialPortInfo::availablePorts().length();
    //    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){

    //        qDebug() << "info for port nr:" << serialPortInfo.systemLocation();
    //        if (serialPortInfo.hasVendorIdentifier()){
    //            qDebug() << "Vendor ID:" << serialPortInfo.vendorIdentifier();
    //        }
    //        if (serialPortInfo.hasProductIdentifier()){
    //            qDebug() << "Product ID:" << serialPortInfo.productIdentifier();
    //        }

    //        qDebug() << "SerialNumber:" << serialPortInfo.description();
    //    }
    bool found = false;
    while(!found){
        foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
            if(serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier()){
                if(serialPortInfo.vendorIdentifier() == arduino_vendor_id){
                    if(serialPortInfo.productIdentifier() == arduino_product_id){
                        arduino_port_name = serialPortInfo.portName();
                        arduino_is_available = true;
                        found = true;
                        qDebug() << "arduino  is now available on" << serialPortInfo.portName();
                        break;
                    }
                }
            }
        }
    }

//        qDebug() << "Nr of available ports:" << QSerialPortInfo::availablePorts().length();
//        const QSerialPortInfo serialPortInfo = QSerialPortInfo::availablePorts().at(2);
//        if(serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier()){
//            if(serialPortInfo.vendorIdentifier() == arduino_vendor_id){
//                if(serialPortInfo.productIdentifier() == arduino_product_id){
//                    arduino_port_name = serialPortInfo.portName();
//                    arduino_is_available = true;
//                    qDebug() << "arduino  is now available on" << serialPortInfo.portName();
//                }
//            }
//        }





    if (arduino_is_available){
        //open and configure port
        arduino->setPortName(arduino_port_name);
        arduino->open(QSerialPort::ReadWrite);
        arduino->setBaudRate(QSerialPort::Baud9600);
        arduino->setDataBits(QSerialPort::Data8);
        arduino->setParity(QSerialPort::NoParity);
        arduino->setStopBits(QSerialPort::OneStop);
        arduino->setFlowControl(QSerialPort::NoFlowControl);
        QObject::connect(arduino, SIGNAL(readyRead()), this, SLOT(readSerial()));

    }else{
        //give error message
        QMessageBox::warning(this,"Port Error" , "could not find arduino");
    }


}

MainWindow::~MainWindow()
{
    if(arduino->isOpen()){
        arduino->close();
    }
    myDB.close();
    delete ui;
}

void MainWindow::readSerial()
{
    serialData = arduino->readLine();
    QString input = QString::fromStdString(serialData.toStdString());
    if(input.startsWith("REQUEST")){
        qDebug() <<"request recieved" + input;

        //make a class called request to handle requests made

        QStringList request = input.split(",");

        qDebug() << "Node: "+ request.at(1) + " made request: " + request.at(2);
        if(Ticket(request.at(1).toInt())){
            open_tickets_model->setQuery(open_tickets_model->query().executedQuery());
            sendACK(request.at(1).toInt());
        }
    }
    //qDebug() << QString::fromStdString(serialData.toStdString());
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

bool Ticket(int node){
    QSqlQuery query;
    query.prepare("SELECT * FROM tickets WHERE source = :source_ID AND close IS NULL");
    query.bindValue(":source_ID", node);
    if(query.exec() && query.first()){//found

        int ticket = query.value(0).toInt();
        query.prepare("UPDATE tickets SET close = CURRENT_TIMESTAMP WHERE ID = :ticket_nr;");
        query.bindValue(":ticket_nr", ticket);
        if(query.exec()){
            qDebug() << "Ticket Closed";
            return true;
        }
        else {
            qDebug() << query.lastError();
            return false;
        }
    }
    else { //not found
        query.prepare("INSERT INTO tickets(source) VALUES (:source_ID);");
        query.bindValue(":source_ID", node);
        if(query.exec()){
            qDebug() << "Ticket Opened";
            return true;
        }
        else {
            qDebug() << query.lastError();
        }
        return false;
        //open_tickets_model->setQuery(open_tickets_model->query().executedQuery());

    }
}



void MainWindow::CreateDB()
{
    //this Create function needs to include the table creation. This is actually an open DB function

    myDB = QSqlDatabase::addDatabase("QSQLITE");
    myDB.setDatabaseName("myDB.db");
    if(myDB.open()){
        ui->statusBar->showMessage("DBOpened",1000);
        myDB.exec("PRAGMA foreign_keys = ON;");
        //qWarning() << "DBOpened: " << myDB.lastError();
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
    qDebug() << query.lastQuery() <<  query.boundValue(":source_ID") ;
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

void MainWindow::sendACK(int node)
{
    qDebug() << "sending Ack";
    arduino->write("ACK{1}");
    arduino->flush();
}
