/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   uga@ee.ed.ac.uk                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "ingredientsdialog.h"


IngredientsDialog::IngredientsDialog(QWidget* parent, RecipeDB *db):QWidget(parent)
{

    // Store pointer to database
    database=db;

    // Design dialog

    layout = new QGridLayout( this, 1, 1, 0, 0);
    QSpacerItem* spacer_left = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout->addMultiCell( spacer_left, 1,4,0,0 );
    QSpacerItem* spacer_top = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
    layout->addMultiCell(spacer_top,0,0,1,4);

    ingredientListView=new KListView (this);
    layout->addMultiCellWidget (ingredientListView,1,4,1,6);
    ingredientListView->addColumn("Id");
    ingredientListView->addColumn("Ingredient");

    QSpacerItem* spacer_rightIngredients = new QSpacerItem( 5,5, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout->addMultiCell(spacer_rightIngredients,1,3,7,7);


    addIngredientButton = new QPushButton( this);
    addIngredientButton->setText("+");
    layout->addWidget( addIngredientButton, 1, 8 );
    addIngredientButton->setMinimumSize(QSize(30,30));
    addIngredientButton->setMaximumSize(QSize(30,30));
    addIngredientButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    addIngredientButton->setFlat(true);

    removeIngredientButton = new QPushButton( this);
    removeIngredientButton->setText("-");
    layout->addWidget( removeIngredientButton, 3, 8 );
    removeIngredientButton->setMinimumSize(QSize(30,30));
    removeIngredientButton->setMaximumSize(QSize(30,30));
    removeIngredientButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    removeIngredientButton->setFlat(true);

    QSpacerItem* spacer_Ing_Buttons = new QSpacerItem( 5,5, QSizePolicy::Minimum, QSizePolicy::Fixed );
    layout->addItem(spacer_Ing_Buttons,2,8);


    QSpacerItem* spacer_Ing_Units = new QSpacerItem( 30,5, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout->addMultiCell(spacer_Ing_Units,1,3,9,9);



    unitsListView=new KListView (this);
    unitsListView->addColumn("Units");
    layout->addMultiCellWidget (unitsListView,1,4,10,11);


    QSpacerItem* spacer_rightUnits = new QSpacerItem( 5,5, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout->addMultiCell(spacer_rightUnits,1,3,12,12);

    addUnitButton = new QPushButton( this);
    addUnitButton->setText("+");
    layout->addWidget( addUnitButton, 1, 13 );
    addUnitButton->resize(QSize(30,30));
    addUnitButton->setMinimumSize(QSize(30,30));
    addUnitButton->setMaximumSize(QSize(30,30));
    addUnitButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    addUnitButton->setFlat(true);

    removeUnitButton = new QPushButton( this);
    removeUnitButton->setText("-");
    layout->addWidget( removeUnitButton, 3, 13 );
    removeUnitButton->resize(QSize(30,30));
    removeUnitButton->setMinimumSize(QSize(30,30));
    removeUnitButton->setMaximumSize(QSize(30,30));
    removeUnitButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    removeUnitButton->setFlat(true);
    QSpacerItem* spacer_Units_Characteristics = new QSpacerItem( 30,5, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout->addMultiCell(spacer_Units_Characteristics,1,3,14,14);

    characteristicsListView=new KListView (this);
    layout->addMultiCellWidget (characteristicsListView,1,4,15,16);
    characteristicsListView->addColumn("c.");
    characteristicsListView->addColumn("Amount");


    // Initialize
    ingredientList =new ElementList;
    unitList=new ElementList;
    reloadIngredientList();

    // Signals & Slots
    connect(this->ingredientListView,SIGNAL(selectionChanged()),this, SLOT(updateUnitList()));
    connect(this->addIngredientButton,SIGNAL(clicked()),this,SLOT(addIngredient()));
    connect(this->addUnitButton,SIGNAL(clicked()),this,SLOT(addUnitToIngredient()));
}


IngredientsDialog::~IngredientsDialog()
{

}

void IngredientsDialog::reloadIngredientList(void)
{
ingredientListView->clear();
ingredientList->clear();
database->loadIngredients(ingredientList);

//Populate this data into the KListView

	for ( Element *ing =ingredientList->getFirst(); ing; ing =ingredientList->getNext() )
	{
	QListViewItem *it= new QListViewItem(ingredientListView,QString::number(ing->id),ing->name);
	}

// Reload Unit List
updateUnitList();

}


void IngredientsDialog::updateUnitList(void)
{
//If none is selected, select first item

QListViewItem *it;
if (!(it=ingredientListView->selectedItem()))
{
it=ingredientListView->firstChild();
}

if (it) {
ingredientListView->setSelected(it,true);
reloadPossibleUnitList(it->text(0).toInt());
}

}

void IngredientsDialog::reloadPossibleUnitList(int ingredientID)
{
unitList->clear();
unitsListView->clear();
database->loadPossibleUnits(ingredientID,unitList);

//Populate this data into the KListView

	for ( Element *unit =unitList->getFirst(); unit; unit =unitList->getNext() )
	{
	QListViewItem *it= new QListViewItem(unitsListView,unit->name);
	}
}

void IngredientsDialog::addIngredient(void)
{
CreateElementDialog* elementDialog=new CreateElementDialog(QString("New Ingredient"));

if ( elementDialog->exec() == QDialog::Accepted ) {
   QString result = elementDialog->newElementName();
   database->createNewIngredient(result); // Create the new ingredient in database
   reloadIngredientList(); // Reload the list from database
}
delete elementDialog;
}

void IngredientsDialog::addUnitToIngredient(void)
{

QListViewItem *it;
int ingredientID=-1;
if (it=ingredientListView->selectedItem())
  {
  ingredientID=it->text(0).toInt();
  }

ElementList allUnits;
database->loadUnits(&allUnits);

SelectUnitDialog* unitsDialog=new SelectUnitDialog(0,&allUnits);

if ( unitsDialog->exec() == QDialog::Accepted ) {
   int result = unitsDialog->unitID();
   database->AddUnitToIngredient(ingredientID,result); // Add result chosen unit to ingredient in database
   updateUnitList(); // Reload the list from database
}

}

