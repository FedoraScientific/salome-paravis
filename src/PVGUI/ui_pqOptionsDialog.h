/********************************************************************************
** Form generated from reading ui file 'pqOptionsDialog.ui'
**
** Created: Wed Apr 21 12:59:58 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_PQOPTIONSDIALOG_H
#define UI_PQOPTIONSDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStackedWidget>
#include <QtGui/QTreeView>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_pqOptionsFrame
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    //QPushButton *ApplyButton;
    //QPushButton *ResetButton;
    //QPushButton *CloseButton;
    QStackedWidget *Stack;
    QWidget *BlankPage;
    QTreeView *PageNames;
    QFrame *Separator;

    void setupUi(QWidget *pqOptionsFrame)
    {
        if (pqOptionsFrame->objectName().isEmpty())
            pqOptionsFrame->setObjectName(QString::fromUtf8("pqOptionsFrame"));
        pqOptionsFrame->resize(514, 382);
        gridLayout = new QGridLayout(pqOptionsFrame);
#ifndef Q_OS_MAC
        gridLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        gridLayout->setMargin(9);
#endif
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        hboxLayout = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout->setSpacing(6);
#endif
        hboxLayout->setMargin(4);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        spacerItem = new QSpacerItem(0, 26, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem);

        //ApplyButton = new QPushButton(pqOptionsFrame);
        //ApplyButton->setObjectName(QString::fromUtf8("ApplyButton"));

        //hboxLayout->addWidget(ApplyButton);

        //ResetButton = new QPushButton(pqOptionsFrame);
        //ResetButton->setObjectName(QString::fromUtf8("ResetButton"));

        //hboxLayout->addWidget(ResetButton);

        //CloseButton = new QPushButton(pqOptionsFrame);
        //CloseButton->setObjectName(QString::fromUtf8("CloseButton"));
        //CloseButton->setDefault(true);

        //hboxLayout->addWidget(CloseButton);


        gridLayout->addLayout(hboxLayout, 2, 0, 1, 2);

        Stack = new QStackedWidget(pqOptionsFrame);
        Stack->setObjectName(QString::fromUtf8("Stack"));
        BlankPage = new QWidget();
        BlankPage->setObjectName(QString::fromUtf8("BlankPage"));
        Stack->addWidget(BlankPage);

        gridLayout->addWidget(Stack, 0, 1, 1, 1);

        PageNames = new QTreeView(pqOptionsFrame);
        PageNames->setObjectName(QString::fromUtf8("PageNames"));
        QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(7));
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(PageNames->sizePolicy().hasHeightForWidth());
        PageNames->setSizePolicy(sizePolicy);
        PageNames->setMinimumSize(QSize(160, 0));
        PageNames->setMaximumSize(QSize(160, 16777215));

        gridLayout->addWidget(PageNames, 0, 0, 1, 1);

        Separator = new QFrame(pqOptionsFrame);
        Separator->setObjectName(QString::fromUtf8("Separator"));
        Separator->setFrameShape(QFrame::HLine);
        Separator->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(Separator, 1, 0, 1, 2);

        //QWidget::setTabOrder(CloseButton, PageNames);
        //QWidget::setTabOrder(PageNames, ApplyButton);
        //QWidget::setTabOrder(ApplyButton, ResetButton);

        retranslateUi(pqOptionsFrame);

        Stack->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(pqOptionsFrame);
    } // setupUi

    void retranslateUi(QWidget *pqOptionsFrame)
    {
        pqOptionsFrame->setWindowTitle(QApplication::translate("pqOptionsFrame", "Options", 0, QApplication::UnicodeUTF8));
        //ApplyButton->setText(QApplication::translate("pqOptionsFrame", "&Apply", 0, QApplication::UnicodeUTF8));
        //ResetButton->setText(QApplication::translate("pqOptionsFrame", "&Reset", 0, QApplication::UnicodeUTF8));
        //CloseButton->setText(QApplication::translate("pqOptionsFrame", "&Ok", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        PageNames->setWhatsThis(QApplication::translate("pqOptionsFrame", "Select a category to show the options for that category.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        Q_UNUSED(pqOptionsFrame);
    } // retranslateUi

};

namespace Ui {
    class pqOptionsFrame: public Ui_pqOptionsFrame {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PQOPTIONSDIALOG_H
