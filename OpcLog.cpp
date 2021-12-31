
#include "OpcLog.h"

OpcLog::OpcLog(QWidget* parent)
	: QMainWindow(parent)
{
	//create main user interface
	ui.setupUi(this);

    //second window class
	secondUi = new NodeMatrix();
	
	//connect Clicked signal from NOde matrix class to change window
	connect(secondUi, &NodeMatrix::ClickedBtn, this, &OpcLog::changeWindow);


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

}
OpcLog::~OpcLog()
{

}

void OpcLog::changeWindow()
{

	if (secondUi->isVisible())
	{
		//open main ui
		secondUi->hide();
		this->show();
	}
	else
	{
		//hide main ui
		this->hide();
		secondUi->show();		
	}
}

void OpcLog::ReadClicked()
{
	//detect if user filled all input boxes
	if (SetMembers() == 0)
	{
		OpcLog::UaConnection(UA_TRUE);

		this->close();
		//QApplication::exit();
	}
}

int OpcLog::SetMembers()
{
	//if we have something in input byte then copy out this number, otherwise set 256 as value out of IP range to recognize it in the next condition
	int byte0 = ((ui.Byte0->text()).toStdString() == "") ? 256 : atoi((ui.Byte0->text()).toStdString().c_str());
	int byte1 = ((ui.Byte1->text()).toStdString() == "") ? 256 : atoi((ui.Byte1->text()).toStdString().c_str());
	int byte2 = ((ui.Byte2->text()).toStdString() == "") ? 256 : atoi((ui.Byte2->text()).toStdString().c_str());
	int byte3 = ((ui.Byte3->text()).toStdString() == "") ? 256 : atoi((ui.Byte3->text()).toStdString().c_str());

	//detect if user filled all input boxes
	if (byte0 >= 0 && byte0 <= 255
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
		mode = ui.StLog->isChecked() ? "ST" : "TC";
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
		OpcLog::UaConnection(UA_FALSE);

		//QApplication::exit();
	}

	OpcLog::changeWindow();
	
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



int OpcLog::UaConnection(UA_Boolean logging)
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

	/////////////////////////////////////////////////////////////

	/* Make your custom datatype known to the stack */
	UA_DataType types[] = { Node_type };

	/* Attention! Here the custom datatypes are allocated on the stack. So they
	* cannot be accessed from parallel (worker) threads. */
	UA_DataTypeArray customDataTypes = { NULL, 1, types };

////////////////////////////////////////////////////

	UA_Client* client = UA_Client_new();
	UA_ClientConfig_setDefault(UA_Client_getConfig(client));
	UA_Client_getConfig(client)->timeout = 30000; //increase timeout from 5s -> 30s
	UA_Client_getConfig(client)->customDataTypes = &customDataTypes;// custom data types
	UA_StatusCode retval = UA_Client_connect(client, ip_identifier);
	if (retval != UA_STATUSCODE_GOOD) {
		UA_Client_delete(client);
		//std::cout << std::hex << retval << std::dec;

		//pop up message
		QMessageBox::critical(this, "Error", "open62541 error code: " + QString::fromStdString(UA_StatusCode_name(retval)), QMessageBox::Ok);
		return (int)retval;
	}

	/* Read the value attribute of the node. UA_Client_readValueAttribute is a
 * wrapper for the raw read service available as UA_Client_Service_read. */
	UA_Variant value; /* Variants can hold scalar values and arrays of any type */
	UA_Variant_init(&value);

	/* NodeId of the variable holding the current time */
	const UA_NodeId nodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_SERVER_SERVERSTATUS_CURRENTTIME);
	retval = UA_Client_readValueAttribute(client, nodeId, &value);

	if (retval == UA_STATUSCODE_GOOD &&
		UA_Variant_hasScalarType(&value, &UA_TYPES[UA_TYPES_DATETIME])) {
		UA_DateTime raw_date = *(UA_DateTime*)value.data;
		UA_DateTimeStruct dts = UA_DateTime_toStruct(raw_date);
		UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "date is: %u-%u-%u %u:%u:%u.%03u\n",
			dts.day, dts.month, dts.year, dts.hour, dts.min, dts.sec, dts.milliSec);
	}
	else
	{
		//pop up message
		QMessageBox::critical(this, "Error", "open62541 error code: " + QString::fromStdString(UA_StatusCode_name(retval)), QMessageBox::Ok);
		return (int)retval;
	}




	char* p = getenv("USERPROFILE");
	if (logging != UA_TRUE)
	{ //read nodes
		retval = OpcLog::SingleReadArray(client,value, nodes_log_level);
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
			retval = OpcLog::SingleReadArray(client, opc_log, value, logging_db);
		}
		else
		{  //user marked telecomunication mode
			retval = OpcLog::SingleReadArray(client, opc_log, value, logging_tc_db);
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
	{
		//delete client and interrupt execution in case of missing user profile

		QMessageBox::critical(this, "Error", "%USERPROFILE% not found", QMessageBox::Ok);

		UA_Client_disconnect(client);
		UA_Client_delete(client); /* Disconnects the client internally */

		return 0;
	}

	/* Clean up */
	UA_Variant_clear(&value);

	UA_Client_disconnect(client);
	UA_Client_delete(client); /* Disconnects the client internally */

	return 0;
}

int OpcLog::SingleReadArray(UA_Client* client, std::fstream& cvsfile, UA_Variant value, char* node_identifier)
{
	UA_String log_entry;
	UA_StatusCode retval;

	UA_sleep_ms(10);

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
	{
		//something went wrong
		QMessageBox::critical(this, "Error", "open62541 error code: " + QString::fromStdString(UA_StatusCode_name(retval)), QMessageBox::Ok);
	}
	return (int)retval;
}

int OpcLog::SingleReadArray(UA_Client* client, UA_Variant value, char* node_identifier)
{
	UA_String log_entry;
	UA_StatusCode retval;

	std::vector<int> level_array;
	UA_sleep_ms(10);

	retval = UA_Client_readValueAttribute(client, UA_NODEID_STRING(ns, node_identifier), &value);

	//check if result array has some entries
	if (retval == UA_STATUSCODE_GOOD &&
		value.arrayLength > 0)
	{
		//SSI_HMI_CPM_Node_ST* values = (SSI_HMI_CPM_Node_ST*)UA_Array_new(value.arrayLength, &UA_TYPES[UA_TYPES_STRING]);


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
			// 
			//fill levels array
			level_array.push_back(MyCDS->log_level);
		}

		//pass node levels array to another class
		secondUi->ShowNodes(level_array);


		////cast variant for string array
		//values = (SSI_HMI_CPM_Node_ST*)value.data;



		//for (size_t i = 0; i < value.arrayLength; i++)
		//{
		//	//cast to string with defined length, since values->data per se print sometimes trash overhead
		//	//std::string s(reinterpret_cast<char const*>(values->log_level), values->length);

		//	std::cout << values->refresh_done << std::endl;

		//	//copy out current array entry to excel file
		//	std::cout << values->log_level << std::endl;

		//	//move pointer one data type size forward
		//	values++;
		//}

		

		retval = UA_Client_writeValueAttribute(client, UA_NODEID_STRING(ns, node_identifier), &value);


	}
	else
	{
		//something went wrong
		QMessageBox::critical(this, "Error", "open62541 error code: " + QString::fromStdString(UA_StatusCode_name(retval)), QMessageBox::Ok);
	}
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

