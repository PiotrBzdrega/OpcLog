
#include "OpcLog.h"




/* TODO: Loading window THREAD when connecting to server or waiting for data */




OpcLog::OpcLog(QWidget* parent)
	: QMainWindow(parent)
{

	//create main user interface
	ui.setupUi(this);

    //second window class
	secondUi = new NodeMatrix();
	
	//connect Clicked signal from NOde matrix class to change window
	connect(secondUi, &NodeMatrix::ClickedBtn, this, &OpcLog::retrunToMain);


	//regular expresion to IP address

	QRegExp rx("^(0|[1-9]|[1-9][0-9]|1[0-9][0-9]|2([0-4][0-9]|5[0-5]))$");
	QValidator* validator3 = new QRegExpValidator(rx, ui.Byte3);
	ui.Byte3->setValidator(validator3);

	QValidator* validator2 = new QRegExpValidator(rx, ui.Byte2);
	ui.Byte2->setValidator(validator2);

	QValidator* validator1 = new QRegExpValidator(rx, ui.Byte1);
	ui.Byte1->setValidator(validator1);

	QValidator* validator0 = new QRegExpValidator(rx, ui.Byte0);
	ui.Byte0->setValidator(validator0);

	//retrieve IP address from "cache"
	OpcLog::getIpCache();
}

OpcLog::~OpcLog()
{
	//std::cout << "Destructor OPC" << "\n";
}

void OpcLog::getIpCache()
{
	//get TEMP path
	char* p = getenv("TEMP"); 	

	//char* to string
	std::string path(p); 

	//get COMPUTERNAME
	p = getenv("COMPUTERNAME");	

	//add slash
	path.push_back('\\'); 

	//char* to string
	std::string computerName(p); 

	//add prefix & computername & file extension to path
	path += "OpcLog_" + computerName + ".bin"; 

	//std::cout << path << "\n";

	//open file in binary mode
	std::ifstream ipcache(path, std::ios::binary);	

	if (!ipcache)
		std::cout<<"not opened" << "\n";
	else if (ipcache.peek() == std::ifstream::traits_type::eof())
		std::cout << "empty" << "\n";
	else
	{
		// get length of file:
		ipcache.seekg(0, ipcache.end);
		int length = ipcache.tellg();
		ipcache.seekg(0, ipcache.beg);

		// allocate memory:
		char* buffer = new char[length];

		// read data as a block:
		ipcache.read(buffer, length);

		//allocate pointer on heap length 16 elements (char) -> int 4
		int* bufferInt=new int[length / sizeof(int)];

		//copy bytes into bufferInt
		memcpy(&bufferInt, &buffer, sizeof(length));
		//bufferInt = reinterpret_cast<int*>(&buffer); //why does not work

		//set IP from "cache"
		OpcLog::overwriteIp(bufferInt[0], bufferInt[1], bufferInt[2], bufferInt[3]);

		//delete pointer
		delete[] buffer;
		//delete[] bufferInt;
	}
}

void OpcLog::overwriteIp(int byte0, int byte1, int byte2, int byte3)
{
	ui.Byte0->setText(QString::number(byte0));
	ui.Byte1->setText(QString::number(byte1));
	ui.Byte2->setText(QString::number(byte2));
	ui.Byte3->setText(QString::number(byte3));
}

void OpcLog::setIpCache(int byte0, int byte1, int byte2, int byte3)
{
	//get TEMP path
	char* p = getenv("TEMP");

	//char* to string
	std::string path(p);

	//get COMPUTERNAME
	p = getenv("COMPUTERNAME");

	//add slash
	path.push_back('\\');

	//char* to string
	std::string computerName(p);

	//add prefix & computername & file extension to path
	path += "OpcLog_" + computerName + ".bin";

	//std::cout << path << "\n";

	//open file in binary mode
	std::ofstream ipcache(path, std::ios::binary);

	// allocate memory on stack
	int buffer[4]{byte0,byte1,byte2,byte3};

	//write buffer into file
	ipcache.write((char*)(&buffer), sizeof(buffer));

	//close file
	ipcache.close();
}


void OpcLog::retrunToMain(bool withConnection)
{

	if (secondUi->isVisible())
	{
		//open main ui
		secondUi->hide();
		this->show();
	}

	//returned to Main window with OPC connection
	if (withConnection)
		OpcLog::UaConnection(UA_FALSE, UA_FALSE);
}

void OpcLog::changeWindow()
{
	

	//if (secondUi->isVisible())
	//{
	//	//open main ui
	//	secondUi->hide();
	//	this->show();
	//}

		//hide main ui
		this->hide();

		//open "nodes" window
		secondUi->show();		
}

void OpcLog::ReadClicked()
{
	//detect if user filled all input boxes
	if (SetMembers() == 0)
	{
		OpcLog::UaConnection(UA_TRUE, UA_TRUE);

		this->close();
		//QApplication::exit();
	}
}

int OpcLog::SetMembers()
{
	//if we have something in input byte then copy out this number, otherwise set 256 as value out of IP range to recognize it in the next condition
	int byte0 = (ui.Byte0->text().toStdString() == "") ? 256 : ui.Byte0->text().toInt();
	int byte1 = (ui.Byte1->text().toStdString() == "") ? 256 : ui.Byte1->text().toInt();;
	int byte2 = (ui.Byte2->text().toStdString() == "") ? 256 : ui.Byte2->text().toInt();;
	int byte3 = (ui.Byte3->text().toStdString() == "") ? 256 : ui.Byte3->text().toInt();;

	//detect if user filled all input boxes
	if (byte0 > 0 && byte0 <= 255
		&&
		byte1 >= 0 && byte1 <= 255
		&&
		byte2 >= 0 && byte2 <= 255
		&&
		byte3 >= 0 && byte3 <= 255)
	{
		//save IP in class field
		IpAdress = (ui.Byte3->text() + "." + ui.Byte2->text() + "." + ui.Byte1->text() + "." + ui.Byte0->text()).toStdString();
		//save mode from radio field
		mode = (ui.StLog->isChecked()) ? "ST" : "TC";

		//save correct iP address
		OpcLog::setIpCache(byte0, byte1, byte2, byte3);

		return 0;
	}
	else
	{
		//invalid ip address throw message
		QMessageBox::critical(this, "Error", "IP address is invalid", QMessageBox::Ok);
		return 1;
	}
}

void OpcLog::ReadNodeClicked()
{

	//check if input boxes has been fullfield properly
	if (SetMembers()==0) 
	{

		//read nodes
		UA_StatusCode retval=OpcLog::UaConnection(UA_FALSE);

		//OPC connection has been correctly established
		if (retval == UA_STATUSCODE_GOOD)
		{
			//open "nodes" window
			OpcLog::changeWindow();
		}
		
	}
};

std::string OpcLog::getIp()
{
	return IpAdress;
}
bool OpcLog::isStandardMode()
{
	//standard mode is choosen
	return (mode == "ST");
}

std::fstream OpcLog::OpenExcelSheet(std::string path)
{
	// file pointer
	std::fstream cvs_log;

	// opens an existing csv file (and removes content) or creates a new file.
	cvs_log.open(path, std::ios::out | std::ios::trunc);

	return cvs_log;
}

int OpcLog::checkCommunicacion() {

	UA_SessionState sessionState;
	UA_StatusCode connectStatus;
	UA_Client_getState(client, NULL, &sessionState, &connectStatus);

		//std::cout << sessionState << std::endl;
		//std::cout << UA_StatusCode_name(connectStatus) << std::endl;

		return sessionState;
}

void OpcLog::deleteClient()
{
	if (checkCommunicacion()) //disconnect client if reachable >0
	{
		UA_Client_disconnect(client);
	}
	UA_Client_delete(client); /* Disconnects the client internally */
}

int OpcLog::UaConnection(UA_Boolean logging, UA_Boolean readNode)
{
	char logging_db[] = "\"SSI_TL_Logging_DB\".\"entry\"";       //standard logs
	char logging_tc_db[] = "\"SSI_TL_Logging_TC_DB\".\"entry\""; //telegrams logs
	char nodes_log_level[] = "\"SSI_HMI_CPM_DB\".\"node\"";      //nodes

	//base opc ip
	char ip_identifier[31] = "opc.tcp://";

	//buffer to store ip from qt
	char b[ip_length];

	//copy out string to char
	strcpy(b, OpcLog::getIp().c_str());

	//paste opc ip address in the char array ; '\0' is appended as last character
	sprintf(&ip_identifier[10], "%s%s", b, opc_address_suffix);

	/* Make your custom datatype known to the stack */
	UA_DataType types[] = { Node_type };

	/* Attention! Here the custom datatypes are allocated on the stack. So they
	* cannot be accessed from parallel (worker) threads. */
	UA_DataTypeArray customDataTypes = { NULL, 1, types };

	client = UA_Client_new();
	UA_ClientConfig_setDefault(UA_Client_getConfig(client));
	UA_Client_getConfig(client)->timeout = 30000; //increase timeout from 5s -> 30s
	UA_Client_getConfig(client)->customDataTypes = &customDataTypes;// custom data types
	UA_StatusCode retval = UA_Client_connect(client, ip_identifier);
	if (retval != UA_STATUSCODE_GOOD) {

		//delete client
		OpcLog::deleteClient();

		//pop up message
		QMessageBox::critical(this, "Error", "open62541 error code: " + QString::fromStdString(UA_StatusCode_name(retval)), QMessageBox::Ok);
		return (int)retval;
	}

	char* p = getenv("USERPROFILE");
	if (logging != UA_TRUE)
	{ //read nodes
		retval = OpcLog::SingleReadArray(nodes_log_level, readNode);
	}		
	else if (p)
	{
		//verify if user has defined userprofile path
		std::string path(p); //assign read path

		path = path + "\\" + "log.csv";

		std::wstring widestr = std::wstring(path.begin(), path.end());
		const wchar_t* widecstr = widestr.c_str();


		//opened stream object
		std::fstream opc_log = OpcLog::OpenExcelSheet(path);


		if (OpcLog::isStandardMode())
		{   //user marked standard mode
			retval = OpcLog::SingleReadArray(opc_log,logging_db);
		}
		else
		{  //user marked telecomunication mode
			retval = OpcLog::SingleReadArray(opc_log, logging_tc_db);
		}

		//close file
		opc_log.close();

		if (!retval)
		{
			//opc connection was succesfull 
			//sort out table
			sort(widecstr);
		}

	}
	else
		QMessageBox::critical(this, "Error", "%USERPROFILE% not found", QMessageBox::Ok); //delete client and interrupt execution in case of missing user profile
		
	OpcLog::deleteClient();//delete client

	return 0;
}

int OpcLog::SingleReadArray(std::fstream& cvsfile, char* node_identifier)
{
	UA_String log_entry;
	UA_StatusCode retval;

	UA_Variant value; // Variants can hold scalar values and arrays of any type 
	UA_Variant_init(&value);

	//UA_sleep_ms(10);

	retval = UA_Client_readValueAttribute(client, UA_NODEID_STRING(ns, node_identifier), &value);

	//check if result array has some entries
	if (retval == UA_STATUSCODE_GOOD &&
		value.arrayLength > 0)
	{
		UA_String* values = (UA_String*)UA_Array_new(value.arrayLength, &UA_TYPES[UA_TYPES_BYTE]);

		//cast variant for string array
		values = (UA_String*)value.data;

		for (size_t i = 0; i < value.arrayLength; i++)
		{
			//cast to string with defined length, since values->data per se print sometimes trash overhead
			std::string s(reinterpret_cast<char const*>(values->data), values->length);

			//copy out current array entry to excel file
			cvsfile << s << std::endl;

			//move pointer one data type size forward
			values++;
		}
	}
	else		
		QMessageBox::critical(this, "Error", "open62541 error code: " + QString::fromStdString(UA_StatusCode_name(retval)), QMessageBox::Ok); //something went wrong


	/* Clean up */
	UA_Variant_clear(&value);

	return (int)retval;
}

int OpcLog::SingleReadArray(char* node_identifier, UA_Boolean readNode)
{
	UA_String log_entry;
	UA_StatusCode retval;

	UA_Variant value; // Variants can hold scalar values and arrays of any type 

	UA_Variant_init(&value);

	std::vector<int> level_array;

	retval = UA_Client_readValueAttribute(client, UA_NODEID_STRING(ns, node_identifier), &value);

	//check if result array has some entries
	if (retval == UA_STATUSCODE_GOOD &&
		value.arrayLength > 0)
	{
		//SSI_HMI_CPM_Node_ST* values = (SSI_HMI_CPM_Node_ST*)UA_Array_new(value.arrayLength, &UA_TYPES[UA_TYPES_STRING]);

		//optimalisation - reserve memory for know elements to avoid relocate vector in each pushback()
		level_array.reserve(value.arrayLength);

		UA_ExtensionObject* eo = ((UA_ExtensionObject*)value.data);
		for (int i = 0; i <value.arrayLength; i++)
		{
			//std::cout << "NODE: " << i << std::endl;
			SSI_HMI_CPM_Node_ST* MyCDS = (SSI_HMI_CPM_Node_ST*)eo[i].content.decoded.data;
			//std::cout << MyCDS->request_update << std::endl;
			//std::cout << MyCDS->update_done << std::endl;
			//std::cout << MyCDS->request_init << std::endl;
			//std::cout << MyCDS->init_done << std::endl;
			//std::cout << MyCDS->request_refresh << std::endl;
			//std::cout << MyCDS->refresh_done << std::endl;
			//std::cout << MyCDS->log_level << std::endl;
			//MyCDS->log_level = 1;
			//std::cout << MyCDS->log_level << std::endl;
			
			if (readNode)
				level_array.push_back(MyCDS->log_level); //fill levels array
			else
				MyCDS->log_level = secondUi->getNodeLevel(i); //overwrite current log level 
		}

		//don't overwrite node level values;
		//user invoked only read
		if (readNode)
			secondUi->ShowNodes(level_array); //pass node levels array to another class
		else
		{
			//write changed level nodes
			retval = UA_Client_writeValueAttribute(client, UA_NODEID_STRING(ns, node_identifier), &value);

			if (retval != UA_STATUSCODE_GOOD)
				QMessageBox::critical(this, "Error", "open62541 error code: " + QString::fromStdString(UA_StatusCode_name(retval)), QMessageBox::Ok); //something went wrong
		}
	
	}
	else
		QMessageBox::critical(this, "Error", "open62541 error code: " + QString::fromStdString(UA_StatusCode_name(retval)), QMessageBox::Ok); //something went wrong

	/* Clean up */
	UA_Variant_clear(&value);

	return (int)retval;
}

void OpcLog::sort(const wchar_t* path)
{
	CDhInitialize init;
	CDispPtr xlApp, xlRange;
	int i, j;
	VARIANT arr;

	dhToggleExceptions(TRUE);

	try
	{
		dhCheck(dhCreateObject(L"Excel.Application", NULL, &xlApp));

		dhPutValue(xlApp, L".Visible = %b", TRUE);

		dhCheck(dhCallMethod(xlApp, L".Workbooks.Open(%S)", path));

		//dhCheck(dhPutValue(xlApp, L".ActiveSheet.Cells(%d,%d) = %S", 8,7 , L"grabierzgrabierz2"));

		dhCheck(dhGetValue(L"%o", &xlRange, xlApp, L".ActiveSheet.Columns(%S)", L"C")); //Log ID row
		dhCallMethod(xlApp, L".ActiveSheet.Range(%S).Sort(%o,%d)", L"A:N", (IDispatch*)xlRange, 1);  // longest entry has 13 columns

		/* xlApp.ActiveSheet.Range("N:0").Clear */
		//dhCallMethod(xlApp, L".ActiveSheet.Range(%S).Clear", L"N:O");

		/* Columns Autofit */
		dhCallMethod(xlApp, L".ActiveSheet.Columns(%S).Autofit", L"A:M");



		//safe changes
		dhCallMethod(xlApp, L".ActiveWorkbook.Save");


		VariantClear(&arr);
	}
	catch (std::string errstr)
	{
		std::cerr << "Fatal error details:" << std::endl << errstr << std::endl;
	}



	dhToggleExceptions(FALSE);

	dhPutValue(xlApp, L".ActiveWorkbook.Saved = %b", TRUE);

}

QString OpcLog::toHex(long long dec)
{
	std::string hex;
	int rem;

	while (dec > 0)
	{
		rem = dec % 16;
		dec /= 16;


		if (rem > 9)
		{
			//A-F
			hex.push_back(char(rem - 10 + 'A'));
		}
		else
		{
			//0-9
			hex.push_back(char(rem + '0'));
		}

	}
	//reverse string
	std::reverse(hex.begin(), hex.end());

	return QString::fromStdString(hex);
}

