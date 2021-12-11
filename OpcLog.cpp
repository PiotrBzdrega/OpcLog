
#include "OpcLog.h"

OpcLog::OpcLog(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

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

void OpcLog::ReadClicked()
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


        this->close();
        //QApplication::exit();
    }
    else
    {
        //invalid ip address throw message
        QMessageBox::critical(this, "Error", "IP address is invalid", QMessageBox::Ok);
    }   
}

std::string OpcLog::getIp()
{
    return IpAdress;
}
std::string OpcLog::getMode()
{
    return mode;
}


