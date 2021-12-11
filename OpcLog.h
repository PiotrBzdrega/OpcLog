#pragma once

#include <QFrame>
#include <QLineEdit>
#include <QIntValidator>
#include "stdint.h"
#include <string>
#include <QHBoxLayout>
#include <QFont>
#include <QLabel>
#include <QKeyEvent>
#include <QPushButton>
#include <QtWidgets/QMainWindow>
#include "ui_OpcLog.h"
#include <qmessagebox.h>

class OpcLog : public QMainWindow
{
    Q_OBJECT

public:
    OpcLog(QWidget *parent = Q_NULLPTR);
    ~OpcLog();
    std::string getIp();
    std::string getMode();


public slots:
    void ReadClicked();



private:
    Ui::OpcLogClass ui;
    std::string IpAdress="";
    std::string mode = "";



};
