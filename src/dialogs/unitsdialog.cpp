/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   uga@ee.ed.ac.uk                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <qlayout.h>
#include "unitsdialog.h"
#include "createelementdialog.h"


UnitsDialog::UnitsDialog(QWidget *parent, RecipeDB *db):QWidget(parent)
{

    // Store pointer to database
    database=db;
    // Design dialog
    QGridLayout* layout = new QGridLayout( this, 1, 1, 0, 0);

    QSpacerItem* spacer_buttonright = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout->addMultiCell( spacer_buttonright, 0,2,3,3 );
    QSpacerItem* spacer_tobutton = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout->addMultiCell( spacer_tobutton, 0,2,1,1 );

    QSpacerItem* spacer_betweenbuttons = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
    layout->addItem( spacer_betweenbuttons, 1,2 );


    QSpacerItem* spacer_left = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout->addMultiCell( spacer_left, 0,2,3,3 );

    unitListView =new KListView(this);
    layout->addMultiCellWidget(unitListView,0,3,0,0);
    unitListView->addColumn("Id.");
    unitListView->addColumn("Unit");
    conversionTable=new ConversionTable(this);
    layout->addMultiCellWidget(conversionTable,0,3,4,4);

    newUnitButton=new QPushButton(this);
    newUnitButton->setText("Create a new Unit");
    newUnitButton->setFlat(true);
    layout->addWidget(newUnitButton,0,2);

    removeUnitButton=new QPushButton(this);
    removeUnitButton->setText("Remove a Unit");
    removeUnitButton->setFlat(true);
    layout->addWidget(removeUnitButton,2,2);


    // Connect signals & slots
    connect(newUnitButton,SIGNAL(clicked()),this,SLOT(createNewUnit()));


    //Populate data into the table
    reloadData();

}

UnitsDialog::~UnitsDialog()
{
}


void UnitsDialog::loadConversionTable(void)
{
ElementList unitList;
database->loadUnits(&unitList);
QStringList unitNames;
for (Element *unit=unitList.getFirst();unit;unit=unitList.getNext())
{
unitNames.append(unit->name);
}
conversionTable->setNumRows(unitNames.count());
conversionTable->setNumCols(unitNames.count());
conversionTable->setRowLabels(unitNames);
conversionTable->setColumnLabels(unitNames);
}

void UnitsDialog::loadUnitsList(void)
{
ElementList unitList;
database->loadUnits(&unitList);
unitListView->clear();
for (Element *unit=unitList.getFirst();unit;unit=unitList.getNext())
{
QListViewItem *it=new QListViewItem(unitListView,QString::number(unit->id),unit->name);
}
}

void UnitsDialog::reloadData(void)
{
loadUnitsList();
loadConversionTable();
}

void UnitsDialog::createNewUnit(void)
{
CreateElementDialog* elementDialog=new CreateElementDialog(QString("New Unit"));

if ( elementDialog->exec() == QDialog::Accepted ) {
   QString result = elementDialog->newElementName();
   database->createNewUnit(result); // Create the new unit in database
   reloadData(); // Reload the unitlist from the database
}
delete elementDialog;
}