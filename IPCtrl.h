﻿#pragma once

#include <QFrame>
#include <QLineEdit>
#include <QIntValidator>
#include "stdint.h"
#include <QHBoxLayout>
#include <QFont>
#include <QLabel>
#include <QKeyEvent>
#include <QPushButton>

class IPCtrl : public QFrame
{
    Q_OBJECT

public:
    IPCtrl(QWidget* parent = 0);
    ~IPCtrl();

    virtual bool eventFilter(QObject* obj, QEvent* event);

public slots:
    void slotTextChanged(QLineEdit* pEdit);

signals:
    void signalTextChanged(QLineEdit* pEdit);

private:
    enum
    {
        QTUTL_IP_SIZE = 4,// число октетов IP адресе
        MAX_DIGITS = 3 // число символов в LineEdit
    };

    QLineEdit* (m_pLineEdit[QTUTL_IP_SIZE]);
    void MoveNextLineEdit(int i);
    void MovePrevLineEdit(int i);
};

