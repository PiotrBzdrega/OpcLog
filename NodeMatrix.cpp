#include "NodeMatrix.h"

/* TODO: Range level change via left click (differentiate right/left) */
/* TODO: Reject() handling */

NodeMatrix::NodeMatrix(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

		ui.fatal->setStyleSheet("background-color: darkRed");
		ui.error->setStyleSheet("background-color: red");
		ui.warn->setStyleSheet("background-color: lightGray");
		ui.info->setStyleSheet("background-color: magenta");
		ui.debug->setStyleSheet("background-color: Chartreuse");
		ui.trace->setStyleSheet("background-color: yellow");
		
	//set id's to button, to facilitate latter detection of id
	ui.buttonGroup->setId(ui.level_up, TL[0].index);   //C_SSI_TL_LOGGING_LEVEL_NULL
	ui.buttonGroup->setId(ui.fatal,    TL[1].index);   //C_SSI_TL_LOGGING_LEVEL_FATAL
	ui.buttonGroup->setId(ui.error,    TL[2].index);   //C_SSI_TL_LOGGING_LEVEL_ERROR
	ui.buttonGroup->setId(ui.warn,     TL[3].index);   //C_SSI_TL_LOGGING_LEVEL_WARN
	ui.buttonGroup->setId(ui.info,     TL[4].index);   //C_SSI_TL_LOGGING_LEVEL_INFO
	ui.buttonGroup->setId(ui.debug,    TL[5].index);   //C_SSI_TL_LOGGING_LEVEL_DEBUG
	ui.buttonGroup->setId(ui.trace,    TL[6].index);   //C_SSI_TL_LOGGING_LEVEL_TRACE

}

NodeMatrix::~NodeMatrix()
{
	//std::cout << "Destructor NODE" << "\n";
}

void NodeMatrix::paintEvent(QPaintEvent* event) 
{

	//QPainter painter(this);
	//painter.setBrush(Qt::red);

	//painter.drawRect(40, 580, 151, 31);//fatal
	//painter.drawRect(40, 610, 151, 32);//error
	//painter.drawRect(40, 641, 151, 32);//warn
	//painter.drawRect(190,580,151,31);//info
	//painter.drawRect(190, 610, 151, 32);//debug
	//painter.drawRect(190, 641, 151, 32);//trace
}

void NodeMatrix::ShowNodes(std::vector<int> nodeLevels)
{
	//overwrite class array
	node_log_level = nodeLevels; 

	int columnAmount = 20;
	
	int rowAmount = node_log_level.size() / columnAmount;  //how many rows do we need to fit all nodes

	if (node_log_level.size() % columnAmount > 0) //seems that we need one more row
		rowAmount++;

	int columnWidth = 10;
	int rowHeight = 15;

	ui.tableWidget->setColumnCount(columnAmount);
	ui.tableWidget->setRowCount(rowAmount);


	//determine column size
	for (size_t i = 0; i < columnAmount; i++)
	{
		ui.tableWidget->setColumnWidth(i, columnWidth);
	}

	//determine row size
	for (size_t i = 0; i < rowAmount; i++)
	{
		ui.tableWidget->setRowHeight(i, rowHeight);
	}

	//view fixed size
	//ui.tableWidget->setMinimumWidth(1003);
	//ui.tableWidget->setMaximumWidth(1003);

	//horizontal fixed size
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	//ui.tableWidget->horizontalHeader()->hide();

	//vertical fixed size
	ui.tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	//ui.tableWidget->verticalHeader()->hide();

	//ui.tableWidget->setItem(0, 0, new QTableWidgetItem(QString::number(15)));

	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //disable posibility to edit cell

	// iterators to place item in proper place
	int row_idx = 0;
	int column_idx = 0;

	for (size_t i = 0; i < node_log_level.size(); i++)
	{
		QTableWidgetItem* tableItem = new QTableWidgetItem();

		tableItem->setText(QString::number(i)); //assign number for table item
		tableItem->setTextAlignment(Qt::AlignCenter);  //centre text
		tableItem->setBackgroundColor(TL[node_log_level[i]].color); // fill background

		ui.tableWidget->setItem(row_idx,column_idx, tableItem);// set item on the table

		//QFont font; //font for item
		//font.setBold(true); //set bold
	
		//ui.tableWidget->item(row_idx, column_idx)->setFont(font); //assign font to item

		//move pointer
		column_idx++;

		//we reached last column -> change row
		if (column_idx >= columnAmount)
		{
			row_idx++;

			if (row_idx > rowAmount)
				break;

			column_idx = 0;
		}
	}
}

void NodeMatrix::Overwrite()
{
	//emit signal to be able to detect it from other class
	emit ClickedBtn();

}

void NodeMatrix::Reject()
{
	//emit signal to be able to detect it from other class
	//emit ClickedBtn();

}

int NodeMatrix::ClickedButtonId() { return  ui.buttonGroup->checkedId(); }


void NodeMatrix::AssignAll()
{
	QAbstractButton* checkedButton = ui.buttonGroup->checkedButton();

	int buttonIdx = ClickedButtonId();
	
	
	if (buttonIdx >-1)	//some level button is checked
	{
		// iterators to place item in proper place
		int row_idx = 0;
		int column_idx = 0;

		for (size_t i = 0; i < node_log_level.size(); i++)
		{
			//get item based on coordinates
			QTableWidgetItem* tableItem = ui.tableWidget->item(row_idx, column_idx);


			if (buttonIdx) //custom log level
				node_log_level[i] = buttonIdx;

			else //only level up event
				node_log_level[i] = (node_log_level[i] > TL[5].index || node_log_level[i] < TL[1].index) ? TL[1].index : node_log_level[i] + 1;
		

			//assert(tableItem->text().toInt() != i);

			if (tableItem->text().toInt() != i) //we reference to wrong item -> :(
			{
				std::cout << "pointer:" << tableItem->text().toInt() << " iterator:" << i << "\n";
			}


			tableItem->setBackgroundColor(TL[node_log_level[i]].color); // fill background

			tableItem->setSelected(false);

			//ui.tableWidget->setItem(row_idx, column_idx, tableItem);

			//move pointer
			column_idx++;

			//we reached last column -> change row
			if (column_idx >= ui.tableWidget->columnCount())
			{
				row_idx++;
				
				//we reached last row leave loop
				if (row_idx > ui.tableWidget->rowCount())
					break;

				column_idx = 0;
			}
		}

	}
	else
	{
		//pop up message
		QMessageBox::critical(this, "Error", "wrong index", QMessageBox::Ok);
	}

}

void NodeMatrix::ChangeLevel(QTableWidgetItem* item)
{
	//text conversion QtString -> int
	int item_idx = item->text().toInt();

	//increase loglevel if node in the range of levels
	node_log_level[item_idx] = (node_log_level[item_idx] > TL[5].index || node_log_level[item_idx] < TL[1].index) ? TL[1].index : node_log_level[item_idx]+1;

	//update background color
	item->setBackgroundColor(TL[node_log_level[item_idx]].color);

	//"deselect" cell after it was double clicked to see new color
	item->setSelected(false);
}


