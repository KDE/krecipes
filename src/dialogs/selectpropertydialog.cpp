/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   uga@ee.ed.ac.uk                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "selectpropertydialog.h"

SelectPropertyDialog::SelectPropertyDialog(QWidget* parent,IngredientPropertyList *propertyList,ElementList *unitList): QDialog(parent)
{

// Initialize internal variables
unitListBack =new ElementList;
// Initialize Widgets
container=new QVBoxLayout(this,5,5);
box=new QGroupBox(this);
container->addWidget(box);
box->setTitle("Choose a Property");
propertyChooseView=new KListView(box);
propertyChooseView->addColumn("id.");
propertyChooseView->addColumn("Property");
propertyChooseView->setGeometry( QRect( 5, 30, 180, 250 ) );
perUnitsLabel=new QLabel(box);
perUnitsLabel->setGeometry(QRect(5,285,100,30));
perUnitsLabel->setText("Per units:");
perUnitsBox=new KComboBox(box);
perUnitsBox->setGeometry(QRect(110,285,75,30));
okButton=new QPushButton(box);
okButton->setGeometry( QRect( 5, 320, 100, 20 ) );
okButton->setText("Ok");
okButton->setFlat(true);
cancelButton=new QPushButton(box);
cancelButton->setGeometry( QRect( 110, 320, 60, 20 ) );
cancelButton->setText("Cancel");
cancelButton->setFlat(true);
resize(QSize(200,380));

// Load data
loadProperties(propertyList);
loadUnits(unitList);

// Connect signals & Slots
connect (okButton,SIGNAL(clicked()),this,SLOT(accept()));
connect (cancelButton,SIGNAL(clicked()),this,SLOT(reject()));
}


SelectPropertyDialog::~SelectPropertyDialog()
{
}


int SelectPropertyDialog::propertyID(void)
{

QListViewItem *it;
if (it=propertyChooseView->selectedItem())
{
return(it->text(0).toInt());
}
else return(-1);
}

int SelectPropertyDialog::perUnitsID()
{

int comboCount=perUnitsBox->count();
if (comboCount>0){ // If not, the list may be empty (no list defined) and crashes while reading as seen before. So check just in case.
int comboID=perUnitsBox->currentItem();
return unitListBack->getElement(comboID)->id;
}
else return(-1);
}

void SelectPropertyDialog::loadProperties(IngredientPropertyList *propertyList)
{
for ( IngredientProperty *property =propertyList->getFirst(); property; property =propertyList->getNext() )
{
QListViewItem *it= new QListViewItem(propertyChooseView,QString::number(property->id),property->name);
}
}
void SelectPropertyDialog::loadUnits(ElementList *unitList)
{
for ( Element *unit =unitList->getFirst(); unit; unit =unitList->getNext() )
{
// Insert in the combobox
perUnitsBox->insertItem(unit->name);

// Store with index for using later
Element newUnit; newUnit.id=unit->id; newUnit.name=unit->name;
unitListBack->add(newUnit);
}
}