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

    // Initialize internal variables
    propertiesList= new IngredientPropertyList;

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
    unitsListView->addColumn("i.");
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
    QSpacerItem* spacer_Units_Properties = new QSpacerItem( 30,5, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout->addMultiCell(spacer_Units_Properties,1,3,14,14);

    propertiesListView=new KListView (this);
    layout->addMultiCellWidget (propertiesListView,1,4,15,16);
    propertiesListView->addColumn("Id");
    propertiesListView->addColumn("Property");
    propertiesListView->addColumn("Amount");
    propertiesListView->addColumn("units");

    QSpacerItem* spacer_rightProperties= new QSpacerItem(5,5,QSizePolicy::Fixed,QSizePolicy::Minimum);
    layout->addMultiCell(spacer_rightProperties,1,3,17,17);

    addPropertyButton= new QPushButton(this);
    addPropertyButton->setText("+");
    layout->addWidget( addPropertyButton, 1, 18 );
    addPropertyButton->resize(QSize(30,30));
    addPropertyButton->setMinimumSize(QSize(30,30));
    addPropertyButton->setMaximumSize(QSize(30,30));
    addPropertyButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    addPropertyButton->setFlat(true);

    removePropertyButton=new QPushButton(this);
    removePropertyButton->setText("-");
    layout->addWidget( removePropertyButton, 3, 18 );
    removePropertyButton->resize(QSize(30,30));
    removePropertyButton->setMinimumSize(QSize(30,30));
    removePropertyButton->setMaximumSize(QSize(30,30));
    removePropertyButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    removePropertyButton->setFlat(true);

    // Initialize
    ingredientList =new ElementList;
    unitList=new ElementList;
    reloadIngredientList();

    // Signals & Slots
    connect(this->ingredientListView,SIGNAL(selectionChanged()),this, SLOT(updateLists()));
    connect(this->addIngredientButton,SIGNAL(clicked()),this,SLOT(addIngredient()));
    connect(this->addUnitButton,SIGNAL(clicked()),this,SLOT(addUnitToIngredient()));
    connect(this->removeUnitButton,SIGNAL(clicked()),this,SLOT(removeUnitFromIngredient()));
    connect(this->removeIngredientButton,SIGNAL(clicked()),this,SLOT(removeIngredient()));
    connect(this->addPropertyButton,SIGNAL(clicked()),this,SLOT(addPropertyToIngredient()));
    connect(this->removePropertyButton,SIGNAL(clicked()),this,SLOT(removePropertyFromIngredient()));
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
updateLists();

}

void IngredientsDialog::reloadUnitList()
{

int ingredientID=-1;
// Find selected ingredient
QListViewItem *it; it=ingredientListView->selectedItem();

if (it){  // Check if an ingredient is selected first
ingredientID=it->text(0).toInt();
}


unitList->clear();
unitsListView->clear();

if (ingredientID>=0)
{
database->loadPossibleUnits(ingredientID,unitList);

//Populate this data into the KListView

	for ( Element *unit =unitList->getFirst(); unit; unit =unitList->getNext() )
	{
	QListViewItem *uit= new QListViewItem(unitsListView,QString::number(unit->id),unit->name);
	}

// Select the first unit
unitsListView->setSelected(unitsListView->firstChild(),true);

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

// Find selected ingredient item
QListViewItem *it;
int ingredientID=-1;
if (it=ingredientListView->selectedItem())
  {
  ingredientID=it->text(0).toInt();
  }
if (ingredientID>=0) // an ingredient was selected previously
{
  ElementList allUnits;
  database->loadUnits(&allUnits);

  SelectUnitDialog* unitsDialog=new SelectUnitDialog(0,&allUnits);

  if ( unitsDialog->exec() == QDialog::Accepted ) {
    int result = unitsDialog->unitID();
    database->AddUnitToIngredient(ingredientID,result); // Add result chosen unit to ingredient in database
    reloadUnitList(); // Reload the list from database
}
}
}

void IngredientsDialog::removeUnitFromIngredient(void)
{

// Find selected ingredient/unit item combination
QListViewItem *it;
int ingredientID=-1, unitID=-1;
if (it=ingredientListView->selectedItem()) ingredientID=it->text(0).toInt();
if (it=unitsListView->selectedItem()) unitID=it->text(0).toInt();

if ((ingredientID>=0)&&(unitID>=0)) // an ingredient/unit combination was selected previously
{
  ElementList results;
  database->findUseOf_Ing_Unit_InRecipes(&results,ingredientID,unitID); // Find if this ingredient-unit combination is being used
  if (results.isEmpty()) database->removeUnitFromIngredient(ingredientID,unitID);
  else database->removeUnitFromIngredient(ingredientID,unitID); //must warn!

reloadUnitList(); // Reload the list from database

}
}


void IngredientsDialog::removeIngredient(void)
{
// Find selected ingredient item
QListViewItem *it;
int ingredientID=-1;
if (it=ingredientListView->selectedItem()) ingredientID=it->text(0).toInt();

if (ingredientID>=0) // an ingredient/unit combination was selected previously
{
ElementList results;
database->findUseOfIngInRecipes(&results,ingredientID);
if (results.isEmpty()) database->removeIngredient(ingredientID);
else database->removeIngredient(ingredientID);


reloadIngredientList();// Reload the list from database

}

}

void IngredientsDialog:: reloadPropertyList(void)
{
propertiesList->clear();
propertiesListView->clear();



//If none is selected, select first item
QListViewItem *it;
it=ingredientListView->selectedItem();

//Populate this data into the KListView
if (it){// make sure that the ingredient list is not empty

	database->loadProperties(propertiesList,it->text(0).toInt()); // load the list for this ingredient
	for ( IngredientProperty *prop =propertiesList->getFirst(); prop; prop =propertiesList->getNext() )
	{
	std::cerr<<prop;
	  QListViewItem *it= new QListViewItem(propertiesListView,QString::number(prop->id),prop->name,QString::number(prop->amount),prop->units+QString("/")+prop->perUnit.name);
	}
	}
}

void IngredientsDialog:: updateLists(void)
{

//If no ingredient is selected, select first item

QListViewItem *it;
if (!(it=ingredientListView->selectedItem()))
{
it=ingredientListView->firstChild();
}

reloadUnitList();
reloadPropertyList();
}

void IngredientsDialog::addPropertyToIngredient(void)
{

// Find selected ingredient item
QListViewItem *it;
int ingredientID=-1;
if (it=ingredientListView->selectedItem())
  {
  ingredientID=it->text(0).toInt();
  }
if (ingredientID>=0) // an ingredient was selected previously
{
 IngredientPropertyList allProperties;
  database->loadProperties(&allProperties);

  SelectPropertyDialog* propertyDialog=new SelectPropertyDialog(0,&allProperties);

  if ( propertyDialog->exec() == QDialog::Accepted ) {
    int propertyID = propertyDialog->propertyID();
    database->AddPropertyToIngredient(ingredientID,propertyID,0); // Add result chosen property to ingredient in database, with amount 0 by default
    reloadPropertyList(); // Reload the list from database
}
}
}

void IngredientsDialog::removePropertyFromIngredient(void)
{

// Find selected ingredient/property item combination
QListViewItem *it;
int ingredientID=-1, propertyID=-1;
if (it=ingredientListView->selectedItem()) ingredientID=it->text(0).toInt();
if (it=propertiesListView->selectedItem()) propertyID=it->text(0).toInt();

if ((ingredientID>=0)&&(propertyID>=0)) // an ingredient/property combination was selected previously
{
  ElementList results;
  database->removePropertyFromIngredient(ingredientID,propertyID);

reloadPropertyList(); // Reload the list from database

}
}