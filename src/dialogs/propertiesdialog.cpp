/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   uga@ee.ed.ac.uk                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
//  *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "propertiesdialog.h"

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
    propertyListView->addColumn("Id");
    propertyListView->addColumn("Property");
    propertyListView->addColumn("Units");

    // Populate UI with data
    reloadPropertyList();

}


PropertiesDialog::~PropertiesDialog()
{
}

void PropertiesDialog::createNewProperty(void)
{
ElementList list;
database->loadUnits(&list);
CreatePropertyDialog* propertyDialog=new CreatePropertyDialog(&list);

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
