/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *   Cyril Bosselut (bosselut@b1project.com)                               *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "propertiesdialog.h"
#include <klocale.h>

#include "DBBackend/recipedb.h"
#include "createpropertydialog.h"
#include "widgets/propertylistview.h"

PropertiesDialog::PropertiesDialog(QWidget *parent,RecipeDB *db):QWidget(parent)
{

    // Store pointer to database
    database=db;

    // Design dialog

    layout = new QGridLayout( this, 1, 1, 0, 0);
    QSpacerItem* spacer_left = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout->addMultiCell( spacer_left, 1,4,0,0 );
    QSpacerItem* spacer_top = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
    layout->addMultiCell(spacer_top,0,0,1,4);


    propertyListView=new StdPropertyListView(this,database,true);
    propertyListView->reload();
    layout->addMultiCellWidget (propertyListView,1,4,1,6);
    QSpacerItem* spacer_toButtons = new QSpacerItem(10,10,QSizePolicy::Fixed, QSizePolicy::Minimum);
    layout->addItem(spacer_toButtons,1,7);
    addPropertyButton=new QPushButton(this);
    addPropertyButton->setText("+");
    addPropertyButton->setFixedSize(QSize(32,32));
    addPropertyButton->setFlat(true);
    layout->addWidget(addPropertyButton,1,8);
    QSpacerItem* spacer_betweenButtons = new QSpacerItem(10,10,QSizePolicy::Minimum, QSizePolicy::Fixed);
    layout->addItem(spacer_betweenButtons,2,7);
    removePropertyButton=new QPushButton(this);
    removePropertyButton->setText("-");
    removePropertyButton->setFixedSize(QSize(32,32));
    removePropertyButton->setFlat(true);
    layout->addWidget(removePropertyButton,3,8);

    // Connect signals & slots
    connect(addPropertyButton,SIGNAL(clicked()),this,SLOT(createNewProperty()));
    connect(removePropertyButton,SIGNAL(clicked()),this,SLOT(removeProperty()));
}


PropertiesDialog::~PropertiesDialog()
{
}

void PropertiesDialog::createNewProperty(void)
{
ElementList list;
database->loadUnits(&list);
CreatePropertyDialog* propertyDialog=new CreatePropertyDialog(this,&list);

if ( propertyDialog->exec() == QDialog::Accepted ) {
   QString name = propertyDialog->newPropertyName();
   QString units= propertyDialog->newUnitsName();
   if (!((name.isNull()) || (units.isNull()))) // Make sure none of the fields are empty
      database->addProperty(name, units);
}
delete propertyDialog;
}

void PropertiesDialog::removeProperty(void)
{
int propertyID = -1;
QListViewItem *it;
if ( (it=propertyListView->selectedItem()) )
{
propertyID=it->text(0).toInt();
}
database->removeProperty(propertyID);
}

void PropertiesDialog::reload(void)
{
propertyListView->reload();
}

#include "propertiesdialog.moc"
