#include "NodeMatrix.h"

NodeMatrix::NodeMatrix(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

}

NodeMatrix::~NodeMatrix()
{
}

void NodeMatrix::ShowNodes(std::vector<int> nodeLevels)
{
	//overwrite class array
	node_log_level = nodeLevels; 

	

	int columnAmount = 20;
	
	int rowAmount = node_log_level.size() / columnAmount;  //how many rows do we need to fit all nodes

	if (node_log_level.size() % columnAmount > 0) //seems that we need one more
		rowAmount++;

	int columnWidth = 10;
	int rowHeight = 15;



	//std::cout << "ich bin hier :" << node_log_level.size()<<" rows :"<< rowAmount << std::endl;
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
	ui.tableWidget->setMinimumWidth(1003);
	ui.tableWidget->setMaximumWidth(1003);

	//horizontal fixed size
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	ui.tableWidget->horizontalHeader()->hide();

	//vertical fixed size
	ui.tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	ui.tableWidget->verticalHeader()->hide();

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

		ui.tableWidget->setItem(row_idx,column_idx, tableItem);

		if (i == 20)
		{
			std::cout << row_idx << std::endl;
			std::cout << column_idx << std::endl;
		}


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

void NodeMatrix::ChangeLevel(QTableWidgetItem* item)
{
	int item_idx = item->text().toInt();

	std::cout << node_log_level[item_idx] << std::endl;

	//increase loglevel if in the range
	node_log_level[item_idx] = (node_log_level[item_idx] > TL[5].index || node_log_level[item_idx] < TL[1].index) ? TL[1].index : node_log_level[item_idx]+1;

	//update background color
	item->setBackgroundColor(TL[node_log_level[item_idx]].color);

	//"deselect" cell after it was double clicked to see new color
	item->setSelected(false);


}
