#pragma once

#include <QWidget>
#include "ui_NodeMatrix.h"
#include <iostream>
#include <vector>

struct LOG_LEVEL {
	std::string name; //log level name
	int index;        //
	QColor color;    //background color
};

class NodeMatrix : public QWidget
{
	Q_OBJECT

public:
	NodeMatrix(QWidget *parent = Q_NULLPTR);
	~NodeMatrix();
	void ShowNodes(std::vector<int> nodeLevels);

public slots:
	void Overwrite();
	void ChangeLevel(QTableWidgetItem* item);
signals:
	void ClickedBtn();

private:
	Ui::NodeMatrix ui;
	std::vector<int> node_log_level;
	LOG_LEVEL TL[7]= 
	{	{ "C_SSI_TL_LOGGING_LEVEL_NULL",  0, Qt::black },
		{ "C_SSI_TL_LOGGING_LEVEL_FATAL", 1, Qt::darkRed },
		{ "C_SSI_TL_LOGGING_LEVEL_ERROR", 2, Qt::red } ,
		{ "C_SSI_TL_LOGGING_LEVEL_WARN",  3, Qt::lightGray } ,
		{ "C_SSI_TL_LOGGING_LEVEL_INFO",  4, Qt::magenta},
		{ "C_SSI_TL_LOGGING_LEVEL_DEBUG", 5, Qt::green },
		{ "C_SSI_TL_LOGGING_LEVEL_TRACE", 6, Qt::yellow }
	};
};
