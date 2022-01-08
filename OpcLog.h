#pragma once

//second ui
#include <NodeMatrix.h>
#include <QTimer>

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

typedef struct  {
	UA_UInt16 request_update; //PLC requests new parameter
	UA_UInt16 update_done;    //PLC has got the new parameter
	UA_UInt16 request_init;   //The written parameter needs an init
	UA_UInt16 init_done;      //Init done
	UA_UInt16 request_refresh;//PLC requests a refresh
	UA_UInt16 refresh_done;   //The refresh is done from the CPM
	UA_Int16  log_level;      //Log level of this node
}SSI_HMI_CPM_Node_ST;

static UA_DataTypeMember Node_members[7] = {
	{
		UA_TYPENAME("request_update") /* .memberName */
		& UA_TYPES[UA_TYPES_UINT16],            /* .memberType */
		0,                                      /* .padding */
		false,                                  /* .isArray */
		false                                   /* .isOptional */
	},
	{
		UA_TYPENAME("update_done")              /* .memberName */
		& UA_TYPES[UA_TYPES_UINT16],            /* .memberType */
		0,                                      /* .padding */
		false,                                  /* .isArray */
		false                                   /* .isOptional */
	},
	{
		UA_TYPENAME("request_init")             /* .memberName */
		& UA_TYPES[UA_TYPES_UINT16],            /* .memberType */
		0,                                      /* .padding */
		false,                                  /* .isArray */
		false                                   /* .isOptional */
	},
	{
		UA_TYPENAME("init_done")                 /* .memberName */
		& UA_TYPES[UA_TYPES_UINT16],             /* .memberType */
		0,                                     /* .padding */
		false,                                  /* .isArray */
		false                                  /* .isOptional */
	},
	{
		UA_TYPENAME("request_refresh")            /* .memberName */
		& UA_TYPES[UA_TYPES_UINT16],             /* .memberType */
		0,                                     /* .padding */
		false,                                  /* .isArray */
		false                                  /* .isOptional */
	},
	{
		UA_TYPENAME("refresh_done")            /* .memberName */
		& UA_TYPES[UA_TYPES_UINT16],             /* .memberType */
		0,                                     /* .padding */
		false,                                  /* .isArray */
		false                                  /* .isOptional */
	},
	{
		UA_TYPENAME("log_level")            /* .memberName */
		& UA_TYPES[UA_TYPES_INT16],             /* .memberType */
		0,                                     /* .padding */
		false,                                  /* .isArray */
		false                                  /* .isOptional */
	}
};

static const UA_DataType Node_type = {
	UA_TYPENAME("SSI_HMI_CPM_Node_ST")					/* The nodeid of the type */
	UA_NODEID_STRING(ns, (char*)"DT_\"SSI_HMI_CPM_Node_ST\""), //https://stackoverflow.com/questions/25549562/how-to-convert-const-char-to-char-in-c
	UA_NODEID_STRING(ns, (char*)"TE_\"SSI_HMI_CPM_Node_ST\""),
	sizeof(SSI_HMI_CPM_Node_ST),
	UA_DATATYPEKIND_STRUCTURE,
	true,
	false,
	7,
	Node_members
};

class OpcLog : public QMainWindow
{
	Q_OBJECT

public:
	OpcLog(QWidget* parent = Q_NULLPTR);
	~OpcLog();




public slots:
	void ReadClicked();
	void ReadNodeClicked();
private slots:
	void retrunToMain();


private:
	Ui::OpcLogClass ui;   //main window
	NodeMatrix *secondUi; //auxiliary ui window
	std::string IpAdress = "";
	std::string mode = "";
	int OpcLog::SingleReadArray(std::fstream& cvsfile, char* node_identifier); //store data in .cvs file
	int OpcLog::SingleReadArray(char* node_identifier, UA_Boolean readNode = UA_TRUE);		   //read into console
	int OpcLog::UaConnection(UA_Boolean logging, UA_Boolean readNode = UA_TRUE);
	std::string getIp();								   //return ip of OPC server in string type
	std::fstream OpcLog::OpenExcelSheet(std::string path); // create/open .cvs file based on path
	bool isStandardMode();
	void OpcLog::sort(const wchar_t* path);
	QString toHex(long long dec);
	int OpcLog::SetMembers();                             //state IP and log mode 
	UA_Client* client;			//client pointer to let other functions in
	void OpcLog::deleteClient();	
	int OpcLog::checkCommunicacion();
	void changeWindow();
	void getIpCache();
	void setIpCache(int byte0, int byte1, int byte2, int byte3);
	void overwriteIp(int byte0, int byte1, int byte2, int byte3);
	
};

