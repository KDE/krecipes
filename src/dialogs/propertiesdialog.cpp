/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                          *
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

PropertiesDialog::PropertiesDialog(QWidget *parent,RecipeDB *db):QWidget(parent)
{

    // Store pointer to database
    database=db;

    // Initialize internal variables
    propertyList=new IngredientPropertyList;

    // Design dialog

    layout = new QGridLayout( this, 1, 1, 0, 0);
    QSpacerItem* spacer_left = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout->addMultiCell( spacer_left, 1,4,0,0 );
    QSpacerItem* spacer_top = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
    layout->addMultiCell(spacer_top,0,0,1,4);


    propertyListView=new KListView (this);
    layout->addMultiCellWidget (propertyListView,1,4,1,6);
    propertyListView->addColumn(i18n("Id"));
    propertyListView->addColumn(i18n("Property"));
    propertyListView->addColumn(i18n("Units"));
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


    // Populate UI with data
    reloadPropertyList();

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
   int perUnits= 1;
   if (!((name.isNull()) || (units.isNull()))) // Make sure none of the fields are empty
      database->addProperty(name, units);
}
delete propertyDialog;

reloadPropertyList();
}

void PropertiesDialog::reloadPropertyList(void)
{
propertyList->clear(); // Empty list
propertyListView->clear(); // Clear the view
database->loadProperties(propertyList);

//Populate this data into the KListView
	for ( IngredientProperty *prop =propertyList->getFirst(); prop; prop =propertyList->getNext() )
	{
	QListViewItem *it= new QListViewItem(propertyListView,QString::number(prop->id),prop->name,prop->units);


	}

}

void PropertiesDialog::removeProperty(void)
{
int propertyID;
QListViewItem *it;
if (it=propertyListView->selectedItem())
{
propertyID=it->text(0).toInt();
}
database->removeProperty(propertyID);

reloadPropertyList(); //Update the list
}

void PropertiesDialog::reload(void)
{
this->reloadPropertyList();
}
