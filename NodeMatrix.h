#pragma once

#include <QWidget>
#include "ui_NodeMatrix.h"
//main ui
#include <OpcLog.h>

class NodeMatrix : public QWidget
{
	Q_OBJECT

public:
	NodeMatrix(QWidget *parent = Q_NULLPTR);
	~NodeMatrix();

public slots:
	void Overwrite();

private:
	Ui::NodeMatrix ui;
};
