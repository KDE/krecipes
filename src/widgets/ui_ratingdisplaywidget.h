/********************************************************************************
** Form generated from reading ui file 'ratingdisplaywidget.ui'
**
** Created: Tue Feb 12 19:56:25 2008
**      by: Qt User Interface Compiler version 4.3.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_RATINGDISPLAYWIDGET_H
#define UI_RATINGDISPLAYWIDGET_H

#include <Qt3Support/Q3Header>
#include <Qt3Support/Q3MimeSourceFactory>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <k3listview.h>
#include "datablocks/rating.h"
#include "kpushbutton.h"

class Ui_RatingDisplayWidget : public QWidget 
{
public:
    QHBoxLayout *hboxLayout;
    QVBoxLayout *vboxLayout;
    QSpacerItem *spacerItem;
    QLabel *icon;
    QSpacerItem *spacerItem1;
    QVBoxLayout *vboxLayout1;
    QLabel *raterName;
    K3ListView *criteriaListView;
    QLabel *comment;
    QHBoxLayout *hboxLayout1;
    QSpacerItem *spacerItem2;
    KPushButton *buttonRemove;
    KPushButton *buttonEdit;
    
    RatingList::iterator rating_it;

    void setupUi(QWidget *RatingDisplayWidget)
    {
    if (RatingDisplayWidget->objectName().isEmpty())
        RatingDisplayWidget->setObjectName(QString::fromUtf8("RatingDisplayWidget"));
    RatingDisplayWidget->resize(533, 211);
    hboxLayout = new QHBoxLayout(RatingDisplayWidget);
    hboxLayout->setSpacing(KDialog::spacingHint());
    hboxLayout->setMargin(KDialog::marginHint());
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    vboxLayout = new QVBoxLayout();
    vboxLayout->setSpacing(KDialog::spacingHint());
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    spacerItem = new QSpacerItem(21, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);

    vboxLayout->addItem(spacerItem);

    icon = new QLabel(RatingDisplayWidget);
    icon->setObjectName(QString::fromUtf8("icon"));
    icon->setMinimumSize(QSize(76, 0));
    icon->setAlignment(Qt::AlignCenter);
    icon->setWordWrap(false);

    vboxLayout->addWidget(icon);

    spacerItem1 = new QSpacerItem(21, 80, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout->addItem(spacerItem1);


    hboxLayout->addLayout(vboxLayout);

    vboxLayout1 = new QVBoxLayout();
    vboxLayout1->setSpacing(KDialog::spacingHint());
    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
    raterName = new QLabel(RatingDisplayWidget);
    raterName->setObjectName(QString::fromUtf8("raterName"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(5));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(raterName->sizePolicy().hasHeightForWidth());
    raterName->setSizePolicy(sizePolicy);
    QFont font;
    font.setBold(true);
    raterName->setFont(font);
    raterName->setWordWrap(false);

    vboxLayout1->addWidget(raterName);

    criteriaListView = new K3ListView(RatingDisplayWidget);
    criteriaListView->addColumn(QApplication::translate("RatingDisplayWidget", "Criteria", 0, QApplication::UnicodeUTF8));
    criteriaListView->header()->setClickEnabled(true, criteriaListView->header()->count() - 1);
    criteriaListView->header()->setResizeEnabled(true, criteriaListView->header()->count() - 1);
    criteriaListView->addColumn(QApplication::translate("RatingDisplayWidget", "Stars", 0, QApplication::UnicodeUTF8));
    criteriaListView->header()->setClickEnabled(true, criteriaListView->header()->count() - 1);
    criteriaListView->header()->setResizeEnabled(true, criteriaListView->header()->count() - 1);
    criteriaListView->setObjectName(QString::fromUtf8("criteriaListView"));
    QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(5));
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(1);
    sizePolicy1.setHeightForWidth(criteriaListView->sizePolicy().hasHeightForWidth());
    criteriaListView->setSizePolicy(sizePolicy1);
    criteriaListView->setProperty("selectionMode", QVariant(K3ListView::NoSelection));

    vboxLayout1->addWidget(criteriaListView);

    comment = new QLabel(RatingDisplayWidget);
    comment->setObjectName(QString::fromUtf8("comment"));
    QSizePolicy sizePolicy2(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(7));
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(comment->sizePolicy().hasHeightForWidth());
    comment->setSizePolicy(sizePolicy2);
    comment->setAlignment(Qt::AlignTop);
    comment->setWordWrap(true);

    vboxLayout1->addWidget(comment);

    hboxLayout1 = new QHBoxLayout();
    hboxLayout1->setSpacing(KDialog::spacingHint());
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    spacerItem2 = new QSpacerItem(150, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout1->addItem(spacerItem2);

    buttonRemove = new KPushButton(RatingDisplayWidget);
    buttonRemove->setObjectName(QString::fromUtf8("buttonRemove"));

    hboxLayout1->addWidget(buttonRemove);

    buttonEdit = new KPushButton(RatingDisplayWidget);
    buttonEdit->setObjectName(QString::fromUtf8("buttonEdit"));

    hboxLayout1->addWidget(buttonEdit);


    vboxLayout1->addLayout(hboxLayout1);


    hboxLayout->addLayout(vboxLayout1);


    retranslateUi(RatingDisplayWidget);

    QMetaObject::connectSlotsByName(RatingDisplayWidget);
    } // setupUi

    void retranslateUi(QWidget *RatingDisplayWidget)
    {
    icon->setText(QString());
    raterName->setText(QApplication::translate("RatingDisplayWidget", "Rater", 0, QApplication::UnicodeUTF8));
    criteriaListView->header()->setLabel(0, QApplication::translate("RatingDisplayWidget", "Criteria", 0, QApplication::UnicodeUTF8));
    criteriaListView->header()->setLabel(1, QApplication::translate("RatingDisplayWidget", "Stars", 0, QApplication::UnicodeUTF8));
    comment->setText(QApplication::translate("RatingDisplayWidget", "Comments", 0, QApplication::UnicodeUTF8));
    buttonRemove->setText(QApplication::translate("RatingDisplayWidget", "Remove", 0, QApplication::UnicodeUTF8));
    buttonRemove->setShortcut(QString());
    buttonEdit->setText(QApplication::translate("RatingDisplayWidget", "Edit...", 0, QApplication::UnicodeUTF8));
    buttonEdit->setShortcut(QString());
    Q_UNUSED(RatingDisplayWidget);
    } // retranslateUi

};

namespace Ui {
    class RatingDisplayWidget: public Ui_RatingDisplayWidget {};
} // namespace Ui

#endif // UI_RATINGDISPLAYWIDGET_H
