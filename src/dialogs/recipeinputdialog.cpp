/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   uga@ee.ed.ac.uk                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include <kfiledialog.h>
#include <qstring.h>
#include <kurl.h>
#include "recipeinputdialog.h"
#include "image.h" //Initializes default photo


RecipeInputDialog::RecipeInputDialog( QWidget* parent, RecipeDB *db): QWidget( parent)
{

// Adjust internal parameters
loadedRecipe=new Recipe();
loadedRecipe->recipeID=-1; // No loaded recipe initially
loadedRecipe->title=QString::null;
loadedRecipe->instructions=QString::null;
database=db;


// Design the Dialog

    // Recipe Photo

    QPixmap image1(defaultPhoto);

    photoLabel=new QLabel(this);
    photoLabel->setScaledContents(true);
    photoLabel->setPixmap(image1);
    photoLabel->setGeometry(QRect(20,10,200,100));

    changePhotoButton=new QPushButton(this);
    changePhotoButton->setGeometry(QRect(221,10,20,100));
    changePhotoButton->setText("...");


    //Ingredient Up & Down buttons

    ingredientGBox=new QGroupBox(this);
    ingredientGBox->setGeometry(QRect(20,130,255,350));
    ingredientGBox->setTitle("Ingredients");
    ingredientGBox->setFlat(true);
    il=new KIconLoader;

    addButton = new KPushButton(ingredientGBox);
    addButton->setGeometry( QRect( 210,65, 31, 31 ) );
    addButton->setFlat(true);
    QPixmap pm=il->loadIcon("new", KIcon::NoGroup,16); addButton->setPixmap(pm);

    upButton = new KPushButton(ingredientGBox);
    upButton->setGeometry( QRect( 210, 120, 31, 31 ) );
    upButton->setFlat(true);
    pm=il->loadIcon("up", KIcon::NoGroup,16); upButton->setPixmap(pm);

    downButton = new KPushButton(ingredientGBox);
    downButton->setGeometry( QRect( 210, 150, 31, 31 ) );
    downButton->setFlat(true);
    pm=il->loadIcon("down", KIcon::NoGroup,16); downButton->setPixmap(pm);

    removeButton = new KPushButton(ingredientGBox);
    removeButton->setGeometry( QRect( 210, 180, 31, 31 ) );
    removeButton->setFlat(true);
    pm=il->loadIcon("remove", KIcon::NoGroup,16); removeButton->setPixmap(pm);



    //Ingredient Input Widgets

    amountEdit = new KDoubleNumInput(ingredientGBox);
    amountEdit->setGeometry( QRect( 10, 25, 60, 30 ) );

    unitBox = new KComboBox( FALSE,ingredientGBox);
    unitBox->setGeometry( QRect( 190, 25, 60, 31 ) );

    ingredientBox = new KComboBox( FALSE,ingredientGBox);
    ingredientBox->setGeometry( QRect( 70, 25, 120, 31 ) );

    ingredientList = new KListView(ingredientGBox, "ingredientList" );
    ingredientList->addColumn("Ingredient");
    ingredientList->addColumn("Amount");
    ingredientList->addColumn("Units");
    ingredientList->setSorting(-1); // Do not sort
    ingredientList->setGeometry( QRect( 10, 65, 190, 280 ) );

    // Recipe Instructions Widgets
    instructionsEdit = new KTextEdit( this);
    instructionsEdit->setGeometry( QRect( 290, 50, 360, 420 ) );

    titleEdit = new KLineEdit( this);
    titleEdit->setGeometry( QRect( 290, 10, 360, 30 ) );

    // Dialog design
    resize( QSize(500, 700).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // Initialize internal data
    ingredientComboList=new ElementList;
    unitComboList=new ElementList;
    enableChangedSignal(); // Enables the signal "changed()"

    // Initialize lists
    reloadCombos();

    // Connect signals & Slots
    connect(this->ingredientBox, SIGNAL(activated(int)), this, SLOT(reloadUnitsCombo(int)));
    connect(this->changePhotoButton, SIGNAL(clicked()), this, SLOT(changePhoto()));
    connect(this->upButton, SIGNAL(clicked()), this, SLOT(moveIngredientUp()));
    connect(this->downButton, SIGNAL(clicked()), this, SLOT(moveIngredientDown()));
    connect(this->removeButton, SIGNAL(clicked()), this, SLOT(removeIngredient()));
    connect(this->addButton, SIGNAL(clicked()), this, SLOT(addIngredient()));
    connect(this, SIGNAL(changed()), this, SLOT(recipeChanged()));
    connect(this->titleEdit, SIGNAL(textChanged(const QString&)), this, SLOT(recipeChanged(const QString&)));
    connect(this->instructionsEdit, SIGNAL(textChanged()), this, SLOT(recipeChanged()));

}


RecipeInputDialog::~RecipeInputDialog()
{
}

void RecipeInputDialog::loadRecipe(int recipeID)
{

//Disable changed() signals
enableChangedSignal(false);


//Empty current recipe
loadedRecipe->empty();
ingredientComboList->clear();
unitComboList->clear();
reloadCombos();
amountEdit->setValue(0.0);
ingredientList->clear();


// Load specified Recipe ID

database->loadRecipe(loadedRecipe,recipeID);

//Load Values in Interface

titleEdit->setText(loadedRecipe->title);
instructionsEdit->setText(loadedRecipe->instructions);
	//show ingredient list
	Ingredient * ing;
	for ( ing = loadedRecipe->ingList.getFirst(); ing; ing = loadedRecipe->ingList.getNext() )
	{
		QListViewItem* lastElement=ingredientList->lastItem();
		 //Insert ingredient after last one
		 QListViewItem* element = new QListViewItem (ingredientList,lastElement,ing->name,QString::number(ing->amount),ing->units);
	}

	//show photo
	if (!loadedRecipe->photo.isNull())
		photoLabel->setPixmap(loadedRecipe->photo);
	else {
		QPixmap pm(defaultPhoto);
		photoLabel->setPixmap(pm);
		}



//Enable changed() signals
enableChangedSignal();

}

void RecipeInputDialog::loadIngredientListCombo(void)
{
database->loadIngredients(ingredientComboList);

//Populate this data into the ComboBox
ingredientBox->clear();
	for ( Element *ing =ingredientComboList->getFirst(); ing; ing =ingredientComboList->getNext() )
	{ingredientBox->insertItem(ing->name);
	}
}
void RecipeInputDialog::loadUnitListCombo(void)
{
int comboIndex=ingredientBox->currentItem();
int comboCount=ingredientBox->count();
if (comboCount>0){ // If not, the list may be empty (no ingredient list defined) and crashes while reading
int selectedIngredient=ingredientComboList->getElement(comboIndex)->id;

database->loadPossibleUnits(selectedIngredient,unitComboList);

//Populate this data into the ComboBox
	for ( Element *unit =unitComboList->getFirst(); unit; unit =unitComboList->getNext() )
	unitBox->insertItem(unit->name);
	}
}

void RecipeInputDialog::reloadUnitsCombo(int)
{

unitBox->clear();
unitComboList->clear();
loadUnitListCombo();
}
 void RecipeInputDialog::changePhoto(void)
 {
// standard filedialog
    KURL filename = KFileDialog::getOpenURL(QString::null, "*.png *.jpg *.jpeg *.xpm *.gif", this);
    QPixmap pixmap (filename.path());
    if (!(pixmap.isNull())) {
    	photoLabel->setPixmap(pixmap);
	emit changed();
	}

 }

void RecipeInputDialog::moveIngredientUp(void)
{
QListViewItem *it=ingredientList->selectedItem();
QListViewItem *iabove;

if (it) iabove=it->itemAbove(); // Only check itemAbove() if it exists...

if (it  && iabove)
	{

	iabove->moveItem(it); //Move the Item

	//Now move it in the Recipe (order is just opposite! above<->below index<->count-index)
	 int it_index=ingredientList->itemIndex(it);
	 int iabove_index=ingredientList->itemIndex(iabove);
	 loadedRecipe->ingList.move(iabove_index,it_index);
	 emit changed();

	}
}

void RecipeInputDialog::moveIngredientDown(void)
{

QListViewItem *it=ingredientList->selectedItem();
QListViewItem *ibelow;
if (it) ibelow=it->itemBelow(); // Only check itemBelow() if it exists...

if (ibelow && it)
{
	 it->moveItem(ibelow);
	 //Now move it in the Recipe
	 int it_index=ingredientList->itemIndex(it);
	 int ibelow_index=ingredientList->itemIndex(ibelow);
	 loadedRecipe->ingList.move(it_index,ibelow_index);
	 emit changed();
	 }

}

void RecipeInputDialog::removeIngredient(void)
{
QListViewItem *it=ingredientList->selectedItem();
if (it)
{
	// Find the one below or above, and save index first
	QListViewItem *iabove, *ibelow, *iselect=0;
	if (ibelow=it->itemBelow()) iselect=ibelow;
		else if (iabove=it->itemAbove()) iselect=iabove;
	int index=ingredientList->itemIndex(it);

	//Now remove the ingredient
	it->setSelected(false);
	delete it;
	if (iselect) ingredientList->setSelected(iselect,true); // be careful iselect->setSelected doesn't work this way.

	// Remove it from the recipe also
	loadedRecipe->ingList.remove(index); // Note index=0...n in KListView, same as in QPtrlist

	emit changed();
	}


}

void RecipeInputDialog::addIngredient(void)
{
Ingredient ing;


//Add it first to the Recipe list then to the ListView
if ((ingredientBox->count()>0) && (unitBox->count()>0)) // Check first they're not empty otherwise getElement crashes...
{
  ing.name=ingredientBox->currentText();
 ing.amount=amountEdit->value();
  ing.units=unitBox->currentText();
  ing.unitID=unitComboList->getElement(unitBox->currentItem())->id;
  ing.ingredientID=ingredientComboList->getElement(ingredientBox->currentItem())->id;
  loadedRecipe->ingList.add(ing);
  loadedRecipe->ingList.debug();
  //Append also to the ListView
  QListViewItem* lastElement=ingredientList->lastItem();
  QListViewItem* element = new QListViewItem (ingredientList,lastElement,ing.name,QString::number(ing.amount),ing.units);
}

emit changed();
}

void RecipeInputDialog::recipeChanged(void)
{
if (changedSignalEnabled)
	{
	// Enable Save Button
	emit enableSaveOption(true);

	}

}

void RecipeInputDialog::recipeChanged(const QString &t)
{
recipeChanged(); // jumps to the real slot function
}

void RecipeInputDialog::enableChangedSignal(bool en)
{
changedSignalEnabled=en;
}

void RecipeInputDialog::save (void)
{
std::cerr<<"I'm saving!\n";

emit enableSaveOption(false);
saveRecipe();
enableChangedSignal(false);
}

void RecipeInputDialog::saveRecipe(void)
{
// Nothing except for the ingredient list (loadedRecipe->ingList)
// was stored before for performance. (recipeID is already there)

loadedRecipe->photo=*(photoLabel->pixmap());
loadedRecipe->instructions=instructionsEdit->text();
loadedRecipe->title=titleEdit->text();

// Now save()
database->saveRecipe(loadedRecipe);


}

void RecipeInputDialog::newRecipe(void)
{
loadedRecipe->empty();
ingredientComboList->clear();
unitComboList->clear();
reloadCombos();
QPixmap image(defaultPhoto); photoLabel->setPixmap(image);
instructionsEdit->setText("Write the recipe instructions here");
titleEdit->setText("Write the recipe title here");
amountEdit->setValue(0.0);
ingredientList->clear();

}

void RecipeInputDialog::reloadCombos(void) //Reloads lists of ingredients and units
{
loadIngredientListCombo();
loadUnitListCombo();
}
