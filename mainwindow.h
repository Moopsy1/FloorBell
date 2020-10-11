#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QtDebug>
#include <QFileInfo>

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

    void on_Add_clicked();

    void on_add_node_clicked();

    void on_del_node_clicked();

private:
    Ui::MainWindow *ui;
    QSqlQueryModel *open_tickets_model;
    QSqlQueryModel *node_list_model;
    QSqlDatabase myDB;
    void CreateDB();
};

#endif // MAINWINDOW_H
