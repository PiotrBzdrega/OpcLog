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
#include <iostream>
#include <algorithm>

//for excel save opc ua entries in the .cvs file
#include <fstream>

//opc client purposes
#include <stdlib.h>
#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>
#include <open62541/plugin/log_stdout.h>

//to solve opc library references
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#pragma comment(lib, "Ws2_32.lib")

//format excel files
#include "disphelper.h"

constexpr char opc_address_suffix[] = ":4840"; //opc port
constexpr int ip_length = 15; //4*3digits + 3*'.'
constexpr int ns = 3; //range assign for nodes

class OpcLog : public QMainWindow
{
    Q_OBJECT

public:
    OpcLog(QWidget *parent = Q_NULLPTR);
    ~OpcLog();

    


public slots:
    void ReadClicked();



private:
    Ui::OpcLogClass ui;
    std::string IpAdress="";
    std::string mode = "";
    int OpcLog::SingleReadArray(UA_Client* client, std::fstream& cvsfile, UA_Variant value, char* node_identifier);
    int UaConnection();
    std::string getIp();
    bool isStandardMode();
    void OpcLog::sort(const wchar_t* path);
    QString toHex(long long dec);




};
