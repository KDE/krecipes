/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "ingredientsdialog.h"
#include "DBBackend/recipedb.h"
#include "selectunitdialog.h"
#include "createelementdialog.h"
#include "ingredientpropertylist.h"
#include "selectpropertydialog.h"
#include "editbox.h"
#include "unitsdialog.h"

#include <qheader.h>
#include <qmessagebox.h>
#include "dependanciesdialog.h"
#include <klocale.h>

IngredientsDialog::IngredientsDialog(QWidget* parent, RecipeDB *db):QWidget(parent)
{

    // Store pointer to database
    database=db;

    // Initialize internal variables
    propertiesList= new IngredientPropertyList;
    perUnitListBack= new ElementList;

    // Design dialog

    layout = new QGridLayout( this, 1, 1, 0, 0);
    QSpacerItem* spacer_left = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout->addItem( spacer_left, 1,0 );
    QSpacerItem* spacer_top = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
    layout->addItem(spacer_top,0,1);

    ingredientListView=new KListView (this);
    ingredientListView->setAllColumnsShowFocus(true);
    layout->addMultiCellWidget (ingredientListView,1,9,1,1);
    ingredientListView->addColumn(i18n("Id"));
    ingredientListView->addColumn(i18n("Ingredient"));
    ingredientListView->setRenameable(1, true);
    ingredientListView->setDefaultRenameAction(QListView::Reject);
    ingredientListView->setMinimumWidth(150);
    ingredientListView->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::MinimumExpanding));

    QSpacerItem* spacer_rightIngredients = new QSpacerItem( 5,5, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout->addItem(spacer_rightIngredients,1,2);


    addIngredientButton = new QPushButton( this);
    addIngredientButton->setText("+");
    layout->addWidget( addIngredientButton, 1, 3 );
    addIngredientButton->setMinimumSize(QSize(30,30));
    addIngredientButton->setMaximumSize(QSize(30,30));
    addIngredientButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    addIngredientButton->setFlat(true);

    removeIngredientButton = new QPushButton( this);
    removeIngredientButton->setText("-");
    layout->addWidget( removeIngredientButton, 3, 3 );
    removeIngredientButton->setMinimumSize(QSize(30,30));
    removeIngredientButton->setMaximumSize(QSize(30,30));
    removeIngredientButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    removeIngredientButton->setFlat(true);

    QSpacerItem* spacer_Ing_Buttons = new QSpacerItem( 5,5, QSizePolicy::Minimum, QSizePolicy::Fixed );
    layout->addItem(spacer_Ing_Buttons,2,3);


    QSpacerItem* spacer_Ing_Units = new QSpacerItem( 30,5, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout->addItem(spacer_Ing_Units,1,4);



    unitsListView=new KListView (this);
    unitsListView->addColumn(i18n("Id"));
    unitsListView->addColumn(i18n("Units"));
    unitsListView->setAllColumnsShowFocus(true);
    layout->addMultiCellWidget (unitsListView,1,4,5,5);
    unitsListView->setMinimumWidth(150);
    unitsListView->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));

    QSpacerItem* spacer_rightUnits = new QSpacerItem( 5,5, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout->addItem(spacer_rightUnits,1,6);

    addUnitButton = new QPushButton( this);
    addUnitButton->setText("+");
    layout->addWidget( addUnitButton, 1, 7 );
    addUnitButton->resize(QSize(30,30));
    addUnitButton->setMinimumSize(QSize(30,30));
    addUnitButton->setMaximumSize(QSize(30,30));
    addUnitButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    addUnitButton->setFlat(true);

    removeUnitButton = new QPushButton( this);
    removeUnitButton->setText("-");
    layout->addWidget( removeUnitButton, 3, 7 );
    removeUnitButton->resize(QSize(30,30));
    removeUnitButton->setMinimumSize(QSize(30,30));
    removeUnitButton->setMaximumSize(QSize(30,30));
    removeUnitButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    removeUnitButton->setFlat(true);
    QSpacerItem* spacer_Units_Properties = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
    layout->addItem(spacer_Units_Properties,5,5);


    propertiesListView=new KListView (this);
    layout->addMultiCellWidget (propertiesListView,6,9,5,5);
    propertiesListView->addColumn(i18n("Id"));
    propertiesListView->addColumn(i18n("Property"));
    propertiesListView->addColumn(i18n("Amount"));
    propertiesListView->addColumn(i18n("Units"));
    propertiesListView->setAllColumnsShowFocus(true);
    propertiesListView->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));
    propertiesListView->setSorting(-1); // Disable sorting. For the moment, the order is important to identify the per_units ID corresponding to this row. So the user shouldn't change this order.

    addPropertyButton= new QPushButton(this);
    addPropertyButton->setText("+");
    layout->addWidget( addPropertyButton, 6, 7 );
    addPropertyButton->resize(QSize(30,30));
    addPropertyButton->setMinimumSize(QSize(30,30));
    addPropertyButton->setMaximumSize(QSize(30,30));
    addPropertyButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    addPropertyButton->setFlat(true);

    removePropertyButton=new QPushButton(this);
    removePropertyButton->setText("-");
    layout->addWidget( removePropertyButton, 8, 7 );
    removePropertyButton->resize(QSize(30,30));
    removePropertyButton->setMinimumSize(QSize(30,30));
    removePropertyButton->setMaximumSize(QSize(30,30));
    removePropertyButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    removePropertyButton->setFlat(true);

    QSpacerItem* spacer_Prop_Buttons = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
    layout->addItem(spacer_Prop_Buttons,7,7);

    inputBox=new EditBox(this);
    inputBox->hide();

    // Initialize
    ingredientList =new ElementList;
    unitList=new ElementList;
    reloadIngredientList();

    // Signals & Slots
    connect(this->ingredientListView,SIGNAL(selectionChanged()),this, SLOT(updateLists()));
    connect(this->ingredientListView,SIGNAL(doubleClicked( QListViewItem*,const QPoint &, int )),this, SLOT(modIngredient( QListViewItem* )));
    connect(this->ingredientListView,SIGNAL(itemRenamed (QListViewItem*)),this, SLOT(saveIngredient( QListViewItem* )));
    connect(this->addIngredientButton,SIGNAL(clicked()),this,SLOT(addIngredient()));
    connect(this->addUnitButton,SIGNAL(clicked()),this,SLOT(addUnitToIngredient()));
    connect(this->removeUnitButton,SIGNAL(clicked()),this,SLOT(removeUnitFromIngredient()));
    connect(this->removeIngredientButton,SIGNAL(clicked()),this,SLOT(removeIngredient()));
    connect(this->addPropertyButton,SIGNAL(clicked()),this,SLOT(addPropertyToIngredient()));
    connect(this->removePropertyButton,SIGNAL(clicked()),this,SLOT(removePropertyFromIngredient()));
    connect(this->propertiesListView,SIGNAL(executed(QListViewItem*)),this,SLOT(insertPropertyEditBox(QListViewItem*)));
    connect(this->inputBox,SIGNAL(valueChanged(double)),this,SLOT(setPropertyAmount(double)));
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
CreateElementDialog* elementDialog=new CreateElementDialog(this,QString(i18n("New Ingredient")));

if ( elementDialog->exec() == QDialog::Accepted ) {
   QString result = elementDialog->newElementName();
   database->createNewIngredient(result); // Create the new ingredient in database
   reloadIngredientList(); // Reload the list from database
}
delete elementDialog;
}

void IngredientsDialog::modIngredient(QListViewItem* i)
{
  addIngredientButton->setEnabled(false);
  removeIngredientButton->setEnabled(false);
  addUnitButton->setEnabled(false);
  removeUnitButton->setEnabled(false);
  addPropertyButton->setEnabled(false);
  removePropertyButton->setEnabled(false);
  ingredientListView->rename(i, 1);
}

void IngredientsDialog::saveIngredient(QListViewItem* i)
{
  database->modIngredient((i->text(0)).toInt(), i->text(1));
  addIngredientButton->setEnabled(true);
  removeIngredientButton->setEnabled(true);
  addUnitButton->setEnabled(true);
  removeUnitButton->setEnabled(true);
  addPropertyButton->setEnabled(true);
  removePropertyButton->setEnabled(true);
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
    int unitID = unitsDialog->unitID();

    if (!(database->ingredientContainsUnit(ingredientID,unitID)))
    	database->addUnitToIngredient(ingredientID,unitID); // Add chosen unit to ingredient in database
    else
    	{
	QMessageBox::information(this,i18n("Unit Exists"),i18n("The ingredient contains already the unit that you have chosen."));
	}
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
  ElementList dependingRecipes,dependingPropertiesInfo;

  database->findIngredientUnitDependancies(ingredientID,unitID,&dependingRecipes,&dependingPropertiesInfo);
  if (dependingRecipes.isEmpty() && dependingPropertiesInfo.isEmpty()) database->removeUnitFromIngredient(ingredientID,unitID);
  else { // must warn!
  DependanciesDialog *warnDialog=new DependanciesDialog(0,&dependingRecipes,0,&dependingPropertiesInfo);

  if (warnDialog->exec()==QDialog::Accepted) database->removeUnitFromIngredient(ingredientID,unitID);
  delete warnDialog;
  }
reloadUnitList(); // Reload the list from database
reloadPropertyList(); // Properties could have been removed if a unit is removed, so we need to reload.
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
ElementList dependingRecipes;
database->findIngredientDependancies(ingredientID,&dependingRecipes);
if (dependingRecipes.isEmpty()) database->removeIngredient(ingredientID);
else { // Need Warning!
  DependanciesDialog *warnDialog=new DependanciesDialog(0,&dependingRecipes);
  if (warnDialog->exec()==QDialog::Accepted) database->removeIngredient(ingredientID);
  delete warnDialog;
	}

reloadIngredientList();// Reload the list from database

}

}

void IngredientsDialog:: reloadPropertyList(void)
{
propertiesList->clear();
propertiesListView->clear();
perUnitListBack->clear();


//If none is selected, select first item
QListViewItem *it;
it=ingredientListView->selectedItem();

//Populate this data into the KListView
if (it){// make sure that the ingredient list is not empty

	database->loadProperties(propertiesList,it->text(0).toInt()); // load the list for this ingredient
	for ( IngredientProperty *prop =propertiesList->getFirst(); prop; prop =propertiesList->getNext() )
	{
	QListViewItem* lastElement=propertiesListView->lastItem();
	//Insert property after the last one (it's important to keep the order in the case of the properties to be able to identify the per_units ID later on).
	  QListViewItem *it= new QListViewItem(propertiesListView,lastElement,QString::number(prop->id),prop->name,QString::number(prop->amount),prop->units+QString("/")+prop->perUnit.name);
	  // Store the perUnits with the ID for using later
	  Element perUnitEl;
	  perUnitEl.id=prop->perUnit.id;
	  perUnitEl.name=prop->perUnit.name;
	  perUnitListBack->add(perUnitEl);

	}
	}
}

void IngredientsDialog:: updateLists(void)
{
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
 IngredientPropertyList allProperties; database->loadProperties(&allProperties);
 ElementList unitList; database->loadPossibleUnits(ingredientID,&unitList);
 SelectPropertyDialog* propertyDialog=new SelectPropertyDialog(0,&allProperties,&unitList);

 if ( propertyDialog->exec() == QDialog::Accepted ) {

    int propertyID = propertyDialog->propertyID();
    int perUnitsID = propertyDialog->perUnitsID();
    if (!(database->ingredientContainsProperty(ingredientID,propertyID,perUnitsID)))
    {
    if ((propertyID>=0) && (perUnitsID>=0)) // check if the property is not -1 ... (not selected)
    	database->addPropertyToIngredient(ingredientID,propertyID,0,perUnitsID); // Add result chosen property to ingredient in database, with amount 0 by default
    }
    else
    {
    QMessageBox::information(this,i18n("Property Exists"),i18n("The property you tried to add already exists in the ingredient with the same per units."));
    }
    reloadPropertyList(); // Reload the list from database
}
}
}

void IngredientsDialog::removePropertyFromIngredient(void)
{

// Find selected ingredient/property item combination
QListViewItem *it;
int ingredientID=-1, propertyID=-1; int perUnitsID=-1;
if (it=ingredientListView->selectedItem()) ingredientID=it->text(0).toInt();
if (it=propertiesListView->selectedItem()) propertyID=it->text(0).toInt();
if (propertyID>=0) perUnitsID=perUnitListBack->getElement(findPropertyNo(it))->id ;

if ((ingredientID>=0)&&(propertyID>=0) && (perUnitsID>=0)) // an ingredient/property combination was selected previously
{
  ElementList results;
  database->removePropertyFromIngredient(ingredientID,propertyID,perUnitsID);

reloadPropertyList(); // Reload the list from database

}
}

void IngredientsDialog::insertPropertyEditBox(QListViewItem* it)
{

QRect r;

r=propertiesListView->header()->sectionRect(2); //Set in position reference to qlistview, and with the column size();

r.moveBy(propertiesListView->pos().x(),propertiesListView->pos().y()); // Move to the position of qlistview
r.moveBy(0,r.height()+propertiesListView->itemRect(it).y()); //Move down to the item, note that its height is same as header's right now.

r.setHeight(it->height()); // Set the item's height



inputBox->setGeometry(r);
inputBox->show();
}

void IngredientsDialog::setPropertyAmount(double amount)
{

inputBox->hide();


QListViewItem *ing_it=ingredientListView->selectedItem(); // Find selected ingredient
QListViewItem *prop_it=propertiesListView->selectedItem();

if (ing_it && prop_it)// Appart from property, Check if an ingredient is selected first, just in case
{
prop_it->setText(2,QString::number(amount));
int propertyID=prop_it->text(0).toInt();
int ingredientID=ing_it->text(0).toInt();
int per_units=perUnitListBack->getElement(findPropertyNo(prop_it))->id ;
database->changePropertyAmountToIngredient(ingredientID,propertyID,amount,per_units);
}

reloadPropertyList();

}

int IngredientsDialog::findPropertyNo(QListViewItem *it)
{
bool found=false;
int i = 0;
QListViewItem* item = propertiesListView->firstChild();
while (i < propertiesListView->childCount() && !found) {
  if (item == propertiesListView->currentItem())
    found = true;
  else {
    item = item->nextSibling();
    ++i;
  }
}
if (found)
  {
  return (i);
  }
else
  {
  return (-1);
  }
}

void IngredientsDialog::reload(void)
{
this->reloadIngredientList();
}
