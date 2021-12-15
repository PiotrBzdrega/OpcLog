
#pragma warning(disable:4996) //disable getenv warning
#include <iostream>
#include <string>

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

//qt 
#include "OpcLog.h"
#include <QtWidgets/QApplication>
#include "IPCtrl.h"

//#include <QtWidgets/QWidget >
//#include <QtWidgets/QPushButton>
//#include "dialog.h"
//#include "ui_dialog.h"

constexpr char opc_address_suffix[] = ":4840"; //opc port
constexpr char r_bracket = ']';
constexpr int max_log_entry = 8;//single/multi read test
constexpr int ip_length = 15; //4*3digits + 3*'.'


//range assign for nodes
constexpr int ns = 3;


int MultiRead(UA_Client* client, std::fstream& cvsfile, char* node_identifier)
{
    UA_Variant out;
    UA_Variant_init(&out);
    UA_DataValue res;
    UA_StatusCode retval;

    //create node id array
    UA_ReadRequest request;
    UA_ReadRequest_init(&request);
    UA_ReadValueId id[max_log_entry + 1];
    UA_String log_entry;


    //assign proper node id to all elements
    for (int i = 0; i < max_log_entry + 1; i++)
    {
        //invoke init
        UA_ReadValueId_init(&id[i]);

        std::cout << &id[i] << std::endl;

        //we need atribute value from node
        id[i].attributeId = UA_ATTRIBUTEID_VALUE;

        //paste subsequent entry number and ']' in the char array to create string type node id; '\0' is appended as last character
        sprintf(&node_identifier[28], "%d%c", i, r_bracket);

        std::cout << node_identifier << std::endl;

        //assign node ID
        id[i].nodeId = UA_NODEID_STRING(ns, node_identifier);

        std::cout << id[i].nodeId.identifier.string.data << std::endl;

    }

    request.nodesToRead = &id[0];
    request.nodesToReadSize = max_log_entry + 1;

    UA_ReadResponse response = UA_Client_Service_read(client, request);
    retval = response.responseHeader.serviceResult;

    if (retval == UA_STATUSCODE_GOOD)
    {
        for (size_t i = 0; i < max_log_entry + 1; i++)
        {

            if (response.results[i].status == UA_STATUSCODE_GOOD)
            {
                std::cout << i << std::endl;
                out = response.results[i].value;
                log_entry = *(UA_String*)out.data;
                std::cout << (log_entry.data) << std::endl;

                UA_Variant_clear(&out);
                UA_Variant_init(&out);
            }

        }
    }







    ////check if response is correct and if result the same number of indices as requested
    //if (retval != UA_STATUSCODE_GOOD || response.resultsSize != (max_log_entry + 1)) {
    //    UA_ReadResponse_clear(&response);
    //    return retval;
    //}

    //UA_StatusCode retStatusArray[max_log_entry + 1];
    //if (retval == UA_STATUSCODE_GOOD)
    //{


    //    if (response.resultsSize == max_log_entry + 1)
    //    {
    //        std::cout << response.resultsSize << std::endl;
    //        for (int i = 0; i < max_log_entry + 1; ++i)
    //        {


    //            retStatusArray[i] = response.results[i].status;
    //        }
    //    }
    //    else
    //    {


    //        UA_ReadResponse_clear(&response);
    //        return UA_STATUSCODE_BADUNEXPECTEDERROR;
    //    }
    //}

    //for (int i = 0; i < max_log_entry + 1; ++i)
    //{
    //    
    //    std::cout << retStatusArray[i]<<std::hex << std::endl;
    //    if (retStatusArray[i] == UA_STATUSCODE_GOOD)
    //    {
    //       

    //        res = response.results[i];
    //        if (!res.hasValue) // no value
    //        {
    //            
    //            
    //            UA_ReadResponse_clear(&response);
    //            return UA_STATUSCODE_BADUNEXPECTEDERROR;
    //        }

    //        //UA_Variant out;
    //        //UA_Variant_init(&out);
    //        memcpy(&out, &res.value, sizeof(UA_Variant));
    //        //UA_Variant_init(&res.value);
    //        //std::cout << i << std::endl;
    //        //std::cout << out.data << std::endl;
    //        

    //        if (out.type == &UA_TYPES[UA_TYPES_STRING])
    //        {

    //            
    //            UA_String* ptr = (UA_String*)out.data;
    //            printf("Text: %.*s\n", ptr->length, ptr->data);
    //        }
    //        else if (out.type == &UA_TYPES[UA_TYPES_UINT32])
    //        {


    //            UA_UInt32* ptr = (UA_UInt32*)out.data;
    //            printf("UInt32 Value: %d\n", *ptr);
    //        }

    //    }
    //}




    //UA_Variant value=response.results[11].value; /* Variants can hold scalar values and arrays of any type */
    //UA_Variant_init(&value);


    //std::cout << response.resultsSize << std::endl;
    //std::cout << value.data << std::endl;

    ////std::cout << response.diagnosticInfosSize << std::endl;
    ////std::cout << response.results[11].value.arrayLength << std::endl;
    ////std::cout << response.results[1].value.arrayLength << std::endl;
    ////std::cout << response.results[2].value.arrayLength << std::endl;
    ////std::cout << response.results[3].value.arrayLength << std::endl;

    ////std::cout << response.results[11].value.data<< std::endl;
    ////std::cout << response.results[1].value.data << std::endl;
    ////std::cout << response.results[2].value.data << std::endl;
    ////std::cout << response.results[3].value.data << std::endl;
    //
    //UA_String* ns = (UA_String*)response.results[0].value.data;
    //for (size_t i = 0; i < response.results[0].value.arrayLength; ++i) {
    //    printf("The NS is %*.s", (int)ns[i].length, ns[i].data);
    //}


    //for (int i = 0; i < max_log_entry + 1; i++)
    //{
    //    std::cout << i << std::endl;
    //    if (response.results[i].status == UA_STATUSCODE_GOOD)
    //    {
    //        UA_DataValue res = response.results[i];
    //        if (!res.hasValue) // no value
    //        {
    //            UA_ReadResponse_clear(&response);
    //            return UA_STATUSCODE_BADUNEXPECTEDERROR;
    //        }

    //        UA_Variant out;
    //        memcpy(&out, &res.value, sizeof(UA_Variant));
    //        UA_Variant_init(&res.value);

    //        if (out.type == &UA_TYPES[UA_TYPES_STRING])
    //        {
    //            UA_String* ptr = (UA_String*)out.data;

    //            std::cout << ptr->length << std::endl;

    //            printf("Text: %.*s\n", ptr->length, ptr->data);
    //        }

    //    }

    //}

    UA_ReadResponse_deleteMembers(&response);

    return 0;
}

int SingleRead(UA_Client* client, std::fstream& cvsfile, char* node_identifier, UA_Variant value)
{
    UA_String log_entry;
    UA_StatusCode retval;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
        //std::cout << (unsigned)strlen(base_node_chars) << std::endl;
    for (int i = 0; i <= max_log_entry; i++)
    {
        UA_sleep_ms(10);

        //paste subsequent entry number and ']' in the char array to create string type node id; '\0' is appended as last character
        sprintf(&node_identifier[28], "%d%c", i, r_bracket);

        retval = UA_Client_readValueAttribute(client, UA_NODEID_STRING(ns, node_identifier), &value);

        if (retval == UA_STATUSCODE_GOOD &&
            UA_Variant_hasScalarType(&value, &UA_TYPES[UA_TYPES_STRING])) {
            log_entry = *(UA_String*)value.data;
            //UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "date is:%.*s\n", log_entry.length, log_entry.data);


            if (log_entry.length > 30)
            {
                //save in excel
                cvsfile << (log_entry.data) << std::endl;
                //std::cout << (log_entry.data) << std::endl;
            }
            //std::cout <</*log_entry.length*/ i<< std::endl;

        }
        else
        {
            std::cout << "something went wrong :" << i << std::endl;

            std::cout << std::hex << retval << std::dec;

            break;
        }
    }
    return 0;
}

int SingleReadArray(UA_Client* client, std::fstream& cvsfile, UA_Variant value,char* node_identifier)
{
    UA_String log_entry;
    UA_StatusCode retval;

    UA_sleep_ms(10);

    //base node identiefier path
    //char node_identifier[34] = "\"SSI_TL_Logging_DB\".\"entry\"";



    retval = UA_Client_readValueAttribute(client, UA_NODEID_STRING(ns, node_identifier), &value);

    //check if result array has some entries
    if (retval == UA_STATUSCODE_GOOD &&
        value.arrayLength >0) 
    {
        UA_String* values = (UA_String*)UA_Array_new(value.arrayLength, &UA_TYPES[UA_TYPES_STRING]);

        //cast variant for string array
        values = (UA_String*)value.data;

        for (size_t i = 0; i < value.arrayLength; i++)
        {
 
            //copy out current array entry to excel file
            cvsfile << (values->data) << std::endl;
            //std::cout << values->length << std::endl; 
            //std::cout << values->data << std::endl;

            //move pointer one data type size forward
            values++;
        }
 }
    else
    {
        std::cout << "something went wrong :" << std::endl;

        std::cout << std::hex << retval << std::dec << std::endl;

        std::cout << value.arrayDimensionsSize << std::endl;
        std::cout << value.type << std::endl;
        std::cout << UA_TYPES_STRING << std::endl;
        std::cout << value.arrayLength << std::endl;   }
    return 0;
}


/* **************************************************************************
 * Excel Sample 1:
 *   Demonstrates outputting data to excel and using it to create a chart.
 *
 ============================================================================ */
int ExcelSample1(void)
{
    CDhInitialize init;
    CDispPtr xlApp, xlRange, xlChart;
    UINT i;
    const WCHAR* szHeadings[] = { L"Mammals", L"Birds", L"Reptiles", L"Fishes", L"Plants" };

    dhToggleExceptions(TRUE);

    try
    {
        dhCheck(dhCreateObject(L"Excel.Application", NULL, &xlApp));

        dhPutValue(xlApp, L".DisplayFullScreen = %b", TRUE);
        dhPutValue(xlApp, L".Visible = %b", TRUE);

        /* xlApp.Workbooks.Add */
        dhCheck(dhCallMethod(xlApp, L".Workbooks.Add"));

        /* Set the worksheet name */
        dhPutValue(xlApp, L".ActiveSheet.Name = %T", TEXT("Critically Endangered"));

        /* Add the column headings */
        for (i = 0; i < 5; i++)
        {
            dhPutValue(xlApp, L".ActiveSheet.Cells(%d, %d) = %S", 1, i + 1, szHeadings[i]);
        }

        /* Format the headings */
        WITH1(xlCells, xlApp, L".ActiveSheet.Range(%S)", L"A1:E1")
        {
            dhPutValue(xlCells, L".Interior.Color = %d", RGB(0xee, 0xdd, 0x82));
            dhPutValue(xlCells, L".Interior.Pattern = %d", 1);  /* xlSolid */
            dhPutValue(xlCells, L".Font.Size = %d", 13);
            dhPutValue(xlCells, L".Borders.Color = %d", RGB(0, 0, 0));
            dhPutValue(xlCells, L".Borders.LineStyle = %d", 1); /* xlContinuous */
            dhPutValue(xlCells, L".Borders.Weight = %d", 2);    /* xlThin */

        } END_WITH(xlCells);

        WITH(xlSheet, xlApp, L".ActiveSheet")
        {
            /* Set some values */
            dhPutValue(xlSheet, L".Range(%S).Value = %d", L"A2", 184);
            dhPutValue(xlSheet, L".Range(%S).Value = %d", L"B2", 182);
            dhPutValue(xlSheet, L".Range(%S).Value = %d", L"C2", 57);
            dhPutValue(xlSheet, L".Range(%S).Value = %d", L"D2", 162);
            dhPutValue(xlSheet, L".Range(%S).Value = %d", L"E2", 1276);

            /* Output data source */
            dhCallMethod(xlSheet, L".Range(%S).Merge", L"A4:E4");
            dhPutValue(xlSheet, L".Range(%S).Value = %S", L"A4", L"Source: IUCN Red List 2003 (http://www.redlist.org/info/tables/table2.html)");

            /* Apply a border around everything. Note '%m' means missing. */
            dhCallMethod(xlSheet, L".Range(%S).BorderAround(%d, %d, %m, %d)", L"A1:E2", 1, 2, RGB(0, 0, 0));

            /* Set column widths */
            dhPutValue(xlSheet, L".Columns(%S).ColumnWidth = %e", L"A:E", 12.5);

        } END_WITH_THROW(xlSheet);

        /* Set xlRange = xlApp.ActiveSheet.Range("A1:E2") */
        dhCheck(dhGetValue(L"%o", &xlRange, xlApp, L".ActiveSheet.Range(%S)", L"A1:E2"));

        /* Set xlChart = xlApp.ActiveWorkbook.Charts.Add */
        dhCheck(dhGetValue(L"%o", &xlChart, xlApp, L".ActiveWorkbook.Charts.Add"));

        /* Set up the chart */
        dhCheck(dhCallMethod(xlChart, L".ChartWizard(%o, %d, %d, %d, %d, %d, %b, %S)",
            (IDispatch*)xlRange, -4100, 7, 1, 1, 0, FALSE, L"Critically Endangered Plants and Animals"));

        dhPutValue(xlChart, L".HasAxis(%d) = %b", 3, FALSE); /* xlSeries */

        /* Put the chart on our worksheet */
        dhCallMethod(xlChart, L".Location(%d,%S)", 2, L"Critically Endangered");
    }
    catch (std::string errstr)
    {
        std::cerr << "Fatal error details:" << std::endl << errstr << std::endl;
    }

    dhToggleExceptions(FALSE);

    dhPutValue(xlApp, L".ActiveWorkbook.Saved = %b", TRUE);

    return 0;
}

/* **************************************************************************
 * Excel Sample 2:
 *   Demonstrates a much faster way of inserting multiple values into excel
 * using an array.
 *
 ============================================================================ */
int ExcelSample2(void)
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

        dhCheck(dhCallMethod(xlApp, L".Workbooks.Add"));

        MessageBoxA(NULL, "First the slow method...", NULL, MB_SETFOREGROUND);

        WITH(xlSheet, xlApp, L"ActiveSheet")
        {
            /* Fill cells with values one by one */
            for (i = 1; i <= 15; i++)
            {
                for (j = 1; j <= 15; j++)
                {
                    dhCheck(dhPutValue(xlSheet, L".Cells(%d,%d) = %d", i, j, i * j));
                }
            }

        } END_WITH_THROW(xlSheet);

        MessageBoxA(NULL, "Now the fast way...", NULL, MB_SETFOREGROUND);

        /* xlApp.ActiveSheet.Range("A1:O15").Clear */
        dhCallMethod(xlApp, L".ActiveSheet.Range(%S).Clear", L"A1:O15");

        /* Create a safe array of VARIANT[15][15] */
        {
            SAFEARRAYBOUND sab[2];

            arr.vt = VT_ARRAY | VT_VARIANT;              /* An array of VARIANTs. */
            sab[0].lLbound = 1; sab[0].cElements = 15;   /* First dimension.  [1 to 15] */
            sab[1].lLbound = 1; sab[1].cElements = 15;   /* Second dimension. [1 to 15] */
            arr.parray = SafeArrayCreate(VT_VARIANT, 2, sab);
        }

        /* Now fill in the array */
        for (i = 1; i <= 15; i++)
        {
            for (j = 1; j <= 15; j++)
            {
                VARIANT tmp = { 0 };
                long indices[2];

                indices[0] = i;  /* Index of first dimension */
                indices[1] = j;  /* Index of second dimension */

                tmp.vt = VT_I4;
                tmp.lVal = i * j + 10;

                SafeArrayPutElement(arr.parray, indices, (void*)&tmp);
            }
        }

        /* Set all values in one shot! */
        /* xlApp.ActiveSheet.Range("A1:O15") = arr */
        dhCheck(dhPutValue(xlApp, L".ActiveSheet.Range(%S) = %v", L"A1:O15", &arr));


        //dhCallMethod(xlApp, L".ActiveSheet.Sort.Range(%S).Sort(%S,%d)", L"A1:O15", L"A1:A", 1); error


        //dhCallMethod(xlApp, L".ActiveSheet.Range(%S).Select", L"A6"); OK
        //dhCallMethod(xlApp, L".ActiveSheet.Columns(%S).Replace(%S,%S)", L"A:O", L"1", L"0"); OK
        //dhCallMethod(xlApp, L".DoubleClick"); nix geschehen

        //dhCallMethod(xlApp, L".ActiveSheet.Columns(%S).Sort(%S,%d)", L"A:O", L"A1",1); error
        //dhCallMethod(xlApp, L".ActiveSheet.Columns(%S).Sort(%S,%d,%d,%d,%b,%d)", L"A:O", L"A1",1,0,1,FALSE,2); error
        //dhCallMethod(xlApp, L".ActiveSheet.Columns(%S).Sort", L"A:O"); //sort method of Range class failed
        //dhCallMethod(xlApp, L".ActiveSheet.Range(%S).Sort", L"A:O"); //sort method of Range class failed

        dhCheck(dhGetValue(L"%o", &xlRange, xlApp, L".ActiveSheet.Columns(%S)", L"A"));
        dhCallMethod(xlApp, L".ActiveSheet.Range(%S).Sort(%o,%d)", L"A1:O15", (IDispatch*)xlRange, 2); //sort method of Range class failed

        VariantClear(&arr);
    }
    catch (std::string errstr)
    {
        std::cerr << "Fatal error details:" << std::endl << errstr << std::endl;
    }

    dhToggleExceptions(FALSE);

    dhPutValue(xlApp, L".ActiveWorkbook.Saved = %b", TRUE);

    return 0;
}


void sort(const wchar_t* path)
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
        dhCallMethod(xlApp, L".ActiveSheet.Range(%S).Clear", L"N:O");

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

int main(int argc, char* argv[])
{

    char logging_db[] = "\"SSI_TL_Logging_DB\".\"entry\""; //standard logs
    char logging_tc_db[] = "\"SSI_TL_Logging_TC_DB\".\"entry\""; //standard logs

    //base opc ip
    char ip_identifier[31] = "opc.tcp://";

    //buffer to store ip from qt
    char b[ip_length];

    QApplication a(argc, argv);

    //create local qt object
    OpcLog w;

    //a.setWindowIcon(QIcon(":/OpcLog/icon.jpg"));

    //display qt object
    w.show();

    //exit application, if has not been closed via object 
    a.exec();

    //we take over invalid ip address, usually when app has benn closed
    if (w.getIp() == "\0")
    {
        //std::cout << "IP invalid or app closed without enter IP" << std::endl;
        return 0;
    }
    //passed ip is correct
    else
    {
        //copy out string to char
        strcpy(b, w.getIp().c_str());
        //std::cout << w.getIp() << std::endl;

        //copy out choosen array node

        //paste opc ip address in the char array ; '\0' is appended as last character
        sprintf(&ip_identifier[10], "%s%s", b, opc_address_suffix);


        //std::cout << ip_identifier << std::endl;

        ////base node identiefier path
        //char node_identifier[34] = "\"SSI_TL_Logging_DB\".\"entry\"[";



        //////////////////////////////

        UA_Client* client = UA_Client_new();
        UA_ClientConfig_setDefault(UA_Client_getConfig(client));
        //UA_Client_getConfig(client)->timeout = 30000; //increase timeout from 5s -> 30s
        UA_StatusCode retval = UA_Client_connect(client, ip_identifier);
        if (retval != UA_STATUSCODE_GOOD) {
            UA_Client_delete(client);
            std::cout << std::hex << retval << std::dec;
            return (int)retval;
        }

        char* p = getenv("USERPROFILE");
        if (p)
            ;//std::cout << p << '\n';
        else
            std::cout << "USERPROFILE not found\n";


        std::string path(p); //assign read path

        path = path + "\\" + "log.csv";

        //std::cout << path << '\n'; //\n escape phrase

        std::wstring widestr = std::wstring(path.begin(), path.end());
        const wchar_t* widecstr = widestr.c_str();


        // file pointer
        std::fstream opc_log;

        // opens an existing csv file (and removes content) or creates a new file.
        opc_log.open(path, std::ios::out | std::ios::trunc);


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


        ////retval = MultiRead(client, opc_log, node_identifier);
        //
        ////retval = SingleRead(client, opc_log, node_identifier, value);

        if (w.isStandardMode())
        {   //user marked standard mode
            retval = SingleReadArray(client, opc_log, value, logging_db);
        }
        else
        {  //user marked telecomunication mode
            retval = SingleReadArray(client, opc_log, value, logging_tc_db);
        }

        

        /* Clean up */
        UA_Variant_clear(&value);

        UA_Client_disconnect(client);
        UA_Client_delete(client); /* Disconnects the client internally */


        //////////////////////////////////////
        //close file
        opc_log.close();


        //std::string IP("127.0.0.1");
        //std::string CMD("ping " + IP);
        //std::system(CMD.c_str());


        sort(widecstr);


    }

    
}

