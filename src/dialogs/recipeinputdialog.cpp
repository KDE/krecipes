/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   ugarro@users.sourceforge.net                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include <kfiledialog.h>
#include <qstring.h>
#include <kurl.h>
#include <qlayout.h>
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
    QGridLayout* layout = new QGridLayout( this, 1, 1, 0, 0);

    // Border
    QSpacerItem* spacer_left = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout->addItem( spacer_left, 1,0 );
    QSpacerItem* spacer_top = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
    layout->addItem( spacer_top, 0,1 );
    // Recipe Photo

    QPixmap image1(defaultPhoto);

    photoLabel=new QLabel(this);
    photoLabel->setScaledContents(true);
    photoLabel->setPixmap(image1);
    photoLabel->setFixedSize(QSize(221,166));
    photoLabel->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    layout->addMultiCellWidget(photoLabel,1,5,1,1);

    changePhotoButton=new QPushButton(this);
    changePhotoButton->setFixedSize(QSize(20,166));
    changePhotoButton->setText("...");
    changePhotoButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    layout->addMultiCellWidget(changePhotoButton,1,5,2,2);



    // Spacers to ingredients and instructions
    QSpacerItem* spacer_bottomphoto = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
    layout->addItem( spacer_bottomphoto,6,1);
    //QSpacerItem* spacerToIngredients = new QSpacerItem( 10,200, QSizePolicy::Minimum, QSizePolicy::Fixed );
    //layout->addMultiCell( spacerToIngredients, 1,4,3,3 );
    QSpacerItem* spacer_rightGBox = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::MinimumExpanding );
    layout->addItem( spacer_rightGBox,7,4 );

    //------- Ingredients Box -----------------
    ingredientGBox =new QGroupBox(this);
    ingredientGBox->setMinimumSize(QSize(275,200));
    ingredientGBox->setMaximumSize(QSize(275,10000));
    ingredientGBox->setTitle("Ingredients");
    ingredientGBox->setFlat(true);
    ingredientGBox->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::MinimumExpanding));
    QGridLayout* boxLayout=new QGridLayout(ingredientGBox);
    layout->addMultiCellWidget(ingredientGBox,7,7,1,3);

    // Border
    QSpacerItem* spacerBoxLeft = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );
    boxLayout->addItem( spacerBoxLeft,1,0);
    QSpacerItem* spacerBoxTop = new QSpacerItem( 10,20, QSizePolicy::Minimum, QSizePolicy::Fixed );
    boxLayout->addItem(spacerBoxTop, 0,1);

    //Input Widgets

    amountEdit = new KDoubleNumInput(ingredientGBox);
    amountEdit->setFixedSize(QSize(60,30));
    amountEdit->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    boxLayout->addWidget(amountEdit,1,1);


    ingredientBox = new KComboBox( FALSE,ingredientGBox);
    ingredientBox->setFixedSize( QSize(120, 30 ) );
    ingredientBox->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    boxLayout->addWidget(ingredientBox,1,2);


    unitBox = new KComboBox( FALSE,ingredientGBox);
    unitBox->setMinimumSize(QSize(51,30));
    unitBox->setMaximumSize(QSize(10000,30));
    unitBox->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed));
    boxLayout->addMultiCellWidget(unitBox,1,1,3,5);

    // Spacers to list and buttons
    QSpacerItem* spacerToList = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
    boxLayout->addItem( spacerToList, 2,1 );
    QSpacerItem* spacerToButtons = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );
    boxLayout->addItem( spacerToButtons, 3,4);

    // Ingredient List
    ingredientList = new KListView(ingredientGBox, "ingredientList" );
    ingredientList->addColumn("Ingredient");
    ingredientList->addColumn("Amount");
    ingredientList->addColumn("Units");
    ingredientList->setSorting(-1); // Do not sort
    ingredientList->setMinimumSize(QSize(200,100));
    ingredientList->setMaximumSize(QSize(10000,10000));
    ingredientList->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));
    boxLayout->addMultiCellWidget(ingredientList,3,8,1,3);


    // Add, Up,down,... buttons
    il=new KIconLoader;

    addButton = new KPushButton(ingredientGBox);
    addButton->setFixedSize( QSize(31, 31 ) );
    addButton->setFlat(true);
    QPixmap pm=il->loadIcon("new", KIcon::NoGroup,16); addButton->setPixmap(pm);
    addButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    boxLayout->addWidget(addButton,3,5);

     // Spacer to the rest of buttons
    QSpacerItem* spacerToOtherButtons = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
    boxLayout->addItem( spacerToOtherButtons,4,5 );

    upButton = new KPushButton(ingredientGBox);
    upButton->setFixedSize( QSize(31, 31 ) );
    upButton->setFlat(true);
    pm=il->loadIcon("up", KIcon::NoGroup,16); upButton->setPixmap(pm);
    upButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    boxLayout->addWidget(upButton,5,5);

    downButton = new KPushButton(ingredientGBox);
    downButton->setFixedSize( QSize(31, 31 ) );
    downButton->setFlat(true);
    pm=il->loadIcon("down", KIcon::NoGroup,16); downButton->setPixmap(pm);
    downButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    boxLayout->addWidget(downButton,6,5);

    removeButton = new KPushButton(ingredientGBox);
    removeButton->setFixedSize( QSize(31, 31 ) );
    removeButton->setFlat(true);
    pm=il->loadIcon("remove", KIcon::NoGroup,16); removeButton->setPixmap(pm);
    removeButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    boxLayout->addWidget(removeButton,7,5);




    // ------- Recipe Instructions Widgets -----------

    titleEdit = new KLineEdit( this);
    titleEdit->setMinimumSize(QSize(360,30));
    titleEdit->setMaximumSize(QSize(10000,30));
    titleEdit->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed));
    layout->addMultiCellWidget(titleEdit,1,1,5,7);

    // Title ->author spacer
    QSpacerItem* title_spacer = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
    layout->addItem(title_spacer,2,5 );

    // Author & Categories
    authorEdit = new KLineEdit( this);
    authorEdit->setMinimumSize(QSize(100,30));
    authorEdit->setMaximumSize(QSize(10000,30));
    authorEdit->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed));
    layout->addWidget(authorEdit,3,5);

    categoryBox=new KComboBox(this);
    categoryBox->setMinimumSize(QSize(100,30));
    categoryBox->setMaximumSize(QSize(10000,30));
    categoryBox->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed));
    layout->addWidget(categoryBox,3,7);

    //Author ->instructions spacer
    QSpacerItem* author_spacer = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
    layout->addItem(author_spacer,4,5 );

    instructionsEdit = new KTextEdit( this);
    instructionsEdit->setMinimumSize(QSize(360,320));
    instructionsEdit->setMaximumSize(QSize(10000,10000));
    instructionsEdit->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));
    layout->addMultiCellWidget(instructionsEdit,5,7,5,7);




    // Dialog design
    resize( QSize(500, 700).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // Initialize internal data
    ingredientComboList=new ElementList;
    unitComboList=new ElementList;
    unsavedChanges=false; // Indicates if there's something not saved yet.
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
	unsavedChanges=true;

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
emit enableSaveOption(false);
saveRecipe();
unsavedChanges=false;
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

bool RecipeInputDialog::everythingSaved()
{
return (!(unsavedChanges));
}