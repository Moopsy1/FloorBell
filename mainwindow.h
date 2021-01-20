#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QtDebug>
#include <QFileInfo>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QByteArray>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void readSerial();

    void on_Add_clicked();

    void on_add_node_clicked();

    void on_del_node_clicked();

    void sendACK(int node);

private:
    Ui::MainWindow *ui;
    QSqlQueryModel *open_tickets_model;
    QSqlQueryModel *node_list_model;
    QSqlDatabase myDB;
    QSerialPort *arduino;
    static const quint16 arduino_vendor_id = 6790;
    static const quint16 arduino_product_id = 29987;
    QString arduino_port_name;
    bool arduino_is_available;
    QByteArray serialData;
    QString serialBuffer;
    QString parsed_data;
    void CreateDB();
};

#endif // MAINWINDOW_H
