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

#include "recipeinputdialog.h"

#include <qstring.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qimage.h>
#include <qmessagebox.h>
#include <qtooltip.h>

#include <kapplication.h>
#include <kspell.h>
#include <kurl.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>

#include "selectauthorsdialog.h"
#include "resizerecipedialog.h"
#include "recipe.h"
#include "DBBackend/recipedb.h"
#include "selectcategoriesdialog.h"
#include "fractioninput.h"
#include "image.h" //Initializes default photo


RecipeInputDialog::RecipeInputDialog( QWidget* parent, RecipeDB *db): QVBox( parent)
{

// Adjust internal parameters
loadedRecipe=new Recipe();
loadedRecipe->recipeID=-1; // No loaded recipe initially
loadedRecipe->title=QString::null;
loadedRecipe->instructions=QString::null;
database=db;

// Tabs
    tabWidget = new QTabWidget( this, "tabWidget" );
    tabWidget->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));
    tabWidget->setAutoMask( TRUE );



    // Spacers to ingredients and instructions
    //QSpacerItem* spacer_bottomphoto = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
    //layout->addItem( spacer_bottomphoto,6,1);
    //QSpacerItem* spacerToIngredients = new QSpacerItem( 10,200, QSizePolicy::Minimum, QSizePolicy::Fixed );
    //layout->addMultiCell( spacerToIngredients, 1,4,3,3 );
    //QSpacerItem* spacer_rightGBox = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::MinimumExpanding );
    //layout->addItem( spacer_rightGBox,7,4 );

    //------- Recipe Tab -----------------
    // Recipe Photo

    recipeTab =new QGroupBox(this);
    recipeTab->setFrameStyle(QFrame::NoFrame);
    recipeTab->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));


        // Design the Dialog
    QGridLayout* recipeLayout = new QGridLayout( recipeTab, 1, 1, 0, 0);

    	// Border
    QSpacerItem* spacer_left = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );
    recipeLayout->addItem( spacer_left, 1,0 );
    QSpacerItem* spacer_right = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );
    recipeLayout->addItem( spacer_right, 1,8 );
    QSpacerItem* spacer_top = new QSpacerItem( 10,10, QSizePolicy::Minimum ,QSizePolicy::Fixed );
    recipeLayout->addItem( spacer_top, 0,1 );
    QSpacerItem* spacer_bottom = new QSpacerItem( 10,10, QSizePolicy::Minimum ,QSizePolicy::MinimumExpanding );
    recipeLayout->addItem( spacer_bottom, 8,1 );


    QPixmap image1(defaultPhoto);

    photoLabel=new QLabel(recipeTab);
    photoLabel->setPixmap(image1);
    photoLabel->setFixedSize(QSize(221,166));
    photoLabel->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    photoLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    recipeLayout->addMultiCellWidget(photoLabel,3,7,1,1);

    changePhotoButton=new QPushButton(recipeTab);
    changePhotoButton->setFixedSize(QSize(20,166));
    changePhotoButton->setText("...");
    changePhotoButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    recipeLayout->addMultiCellWidget(changePhotoButton,3,7,2,2);


    //Title->photo spacer
    QSpacerItem* title_photo = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
    recipeLayout->addItem(title_photo,2,3 );


    // Title
    QVBox *titleBox = new QVBox(recipeTab);
    titleBox->setSpacing(5);
    titleLabel = new QLabel(i18n("Recipe Name"),titleBox);
    titleEdit = new KLineEdit(titleBox);
    titleEdit->setMinimumSize(QSize(360,30));
    titleEdit->setMaximumSize(QSize(10000,30));
    titleEdit->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed));
    recipeLayout->addMultiCellWidget(titleBox,1,1,1,7);


    // Photo ->author spacer
    QSpacerItem* title_spacer = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );
    recipeLayout->addItem(title_spacer,2,1 );

    // Author(s) & Categories
    QVBox *authorBox = new QVBox(recipeTab); // contains label and authorInput (input widgets)
    authorBox->setSpacing(5);
    recipeLayout->addWidget(authorBox,3,4);
    authorLabel = new QLabel(i18n("Authors"),authorBox);
    QHBox *authorInput= new QHBox(authorBox); // Contains input + button


    authorShow = new KLineEdit(authorInput);
    authorShow->setReadOnly(true);
    authorShow->setMinimumSize(QSize(100,20));
    authorShow->setMaximumSize(QSize(10000,20));
    authorShow->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed));


    addAuthorButton= new QPushButton(authorInput);
    addAuthorButton->setText("+");
    addAuthorButton->setFixedSize(QSize(20,20));
    addAuthorButton->setFlat(true);


    QSpacerItem* author_category = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );
    recipeLayout->addItem(author_category,3,5 );

    QVBox *categoryBox = new QVBox(recipeTab); // Contains the label and categoryInput (input widgets)
    categoryBox->setSpacing(5);
    categoryLabel = new QLabel(i18n("Categories"),categoryBox);
    QHBox *categoryInput= new QHBox(categoryBox); // Contains the input widgets

    categoryShow=new KLineEdit(categoryInput);
    categoryShow->setReadOnly(true);
    categoryShow->setMinimumSize(QSize(100,20));
    categoryShow->setMaximumSize(QSize(10000,20));
    categoryShow->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed));
    recipeLayout->addWidget(categoryBox,4,4);

    addCategoryButton= new QPushButton(categoryInput);
    addCategoryButton->setText("+");
    addCategoryButton->setFixedSize(QSize(20,20));
    addCategoryButton->setFlat(true);

    //Category ->Servings spacer
    QSpacerItem* category_servings = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
    recipeLayout->addItem(category_servings,5,4 );

    QVBox *servingsBox = new QVBox(recipeTab);
    servingsBox->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed));
    servingsBox->setSpacing(5);

    servingsLabel = new QLabel(i18n("Servings"),servingsBox);
    servingsNumInput = new KIntNumInput(servingsBox);
    servingsNumInput->setMinValue(1);

    recipeLayout->addWidget(servingsBox,6,4);

    //------- END OF Recipe Tab ---------------

    // Functions Box

    il=new KIconLoader;

    functionsBox=new QGroupBox(this);
    functionsBox->setFrameStyle(QFrame::NoFrame);
    functionsBox->setColumns(3);
    //recipeLayout->addMultiCellWidget(functionsBox,10,10,4,8);
    saveButton=new QToolButton(functionsBox); saveButton->setIconSet(il->loadIconSet("filesave", KIcon::Small)); saveButton->setEnabled(false);
    showButton=new QToolButton(functionsBox); showButton->setIconSet(il->loadIconSet("viewmag", KIcon::Small));
    shopButton=new QToolButton(functionsBox); shopButton->setIconSet(il->loadIconSet("trolley", KIcon::Small));
    closeButton=new QToolButton(functionsBox); closeButton->setIconSet(il->loadIconSet("fileclose", KIcon::Small));
    resizeButton=new QToolButton(functionsBox); resizeButton->setIconSet(il->loadIconSet("2uparrow", KIcon::Small)); //TODO: give me an icon :)

    QToolTip::add(saveButton, i18n("Save recipe"));
    QToolTip::add(showButton, i18n("Show recipe"));
    QToolTip::add(shopButton,i18n("Add to shopping list"));
    QToolTip::add(closeButton,i18n("Close"));
    QToolTip::add(resizeButton,i18n("Resize Recipe"));

    //------- Ingredients Tab -----------------

    ingredientGBox =new QGroupBox(this);
    ingredientGBox->setFrameStyle(QFrame::NoFrame);
    ingredientGBox->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));
    QGridLayout* ingredientsLayout=new QGridLayout(ingredientGBox);

    // Border
    QSpacerItem* spacerBoxLeft = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );
    ingredientsLayout->addItem( spacerBoxLeft,1,0);
    QSpacerItem* spacerBoxTop = new QSpacerItem( 10,20, QSizePolicy::Minimum, QSizePolicy::Fixed );
    ingredientsLayout->addItem(spacerBoxTop, 0,1);

    //Input Widgets
    QVBox *ingredientVBox = new QVBox( ingredientGBox );
    ingredientVBox->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed));
    ingredientLabel = new QLabel( i18n("Ingredient:"), ingredientVBox );
    ingredientBox = new KComboBox( TRUE, ingredientVBox );
    ingredientBox->setAutoCompletion( TRUE );
    //çingredientBox->completionObject()->setCompletionMode( KGlobalSettings::CompletionPopupAuto );
    ingredientBox->lineEdit()->disconnect(ingredientBox); //so hitting enter doesn't enter the item into the box
    ingredientBox->setMinimumSize(QSize(100,30));
    ingredientBox->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed));
    ingredientsLayout->addWidget(ingredientVBox,1,1);

    QVBox *amountVBox = new QVBox( ingredientGBox );
    amountVBox->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed));
    amountLabel = new QLabel( i18n("Amount:"), amountVBox );
    amountEdit = new FractionInput( amountVBox);
    amountEdit->setFixedSize(QSize(60,30));
    amountEdit->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    ingredientsLayout->addWidget(amountVBox,1,2);

    QVBox *unitVBox = new QVBox( ingredientGBox );
    unitVBox->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed));
    unitLabel = new QLabel( i18n("Unit:"), unitVBox );
    unitBox = new KComboBox( TRUE, unitVBox );
    unitBox->setAutoCompletion( TRUE );
    //unitBox->completionObject()->setCompletionMode( KGlobalSettings::CompletionPopupAuto );
    unitBox->lineEdit()->disconnect(unitBox); //so hitting enter doesn't enter the item into the box
    unitBox->setMinimumSize(QSize(100,30));
    unitBox->setMaximumSize(QSize(10000,30));
    unitBox->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed));
    ingredientsLayout->addMultiCellWidget(unitVBox,1,1,3,5);

    // Spacers to list and buttons
    QSpacerItem* spacerToList = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
    ingredientsLayout->addItem( spacerToList, 2,1 );
    QSpacerItem* spacerToButtons = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );
    ingredientsLayout->addItem( spacerToButtons, 3,4);

    // Add, Up,down,... buttons

    addButton = new KPushButton(ingredientGBox);
    addButton->setFixedSize( QSize(31, 31 ) );
    addButton->setFlat(true);
    QPixmap pm=il->loadIcon("new", KIcon::NoGroup,16); addButton->setPixmap(pm);
    addButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    ingredientsLayout->addWidget(addButton,3,5);

     // Spacer to the rest of buttons
    QSpacerItem* spacerToOtherButtons = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
    ingredientsLayout->addItem( spacerToOtherButtons,4,5 );

    upButton = new KPushButton(ingredientGBox);
    upButton->setFixedSize( QSize(31, 31 ) );
    upButton->setFlat(true);
    pm=il->loadIcon("up", KIcon::NoGroup,16); upButton->setPixmap(pm);
    upButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    ingredientsLayout->addWidget(upButton,5,5);

    downButton = new KPushButton(ingredientGBox);
    downButton->setFixedSize( QSize(31, 31 ) );
    downButton->setFlat(true);
    pm=il->loadIcon("down", KIcon::NoGroup,16); downButton->setPixmap(pm);
    downButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    ingredientsLayout->addWidget(downButton,6,5);

    removeButton = new KPushButton(ingredientGBox);
    removeButton->setFixedSize( QSize(31, 31 ) );
    removeButton->setFlat(true);
    pm=il->loadIcon("remove", KIcon::NoGroup,16); removeButton->setPixmap(pm);
    removeButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    ingredientsLayout->addWidget(removeButton,7,5);

    QToolTip::add(addButton, i18n("Add ingredient"));
    QToolTip::add(upButton, i18n("Move ingredient up"));
    QToolTip::add(downButton, i18n("Move ingredient down"));
    QToolTip::add(removeButton, i18n("Remove ingredient"));

    // Ingredient List
    ingredientList = new KListView(ingredientGBox, "ingredientList" );
    ingredientList->addColumn(i18n("Ingredient"));
    ingredientList->addColumn(i18n("Amount"));
    ingredientList->setColumnAlignment( 1, Qt::AlignHCenter );
    ingredientList->addColumn(i18n("Units"));
    ingredientList->setSorting(-1); // Do not sort
    ingredientList->setMinimumSize(QSize(200,100));
    ingredientList->setMaximumSize(QSize(10000,10000));
    ingredientList->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));
    ingredientList->setRenameable(1, true);
    ingredientList->setDefaultRenameAction(QListView::Reject);
    ingredientsLayout->addMultiCellWidget(ingredientList,3,8,1,3);

    // ------- Recipe Instructions Tab -----------

    instructionsTab = new QGroupBox(this);
    instructionsTab->setFrameStyle(QFrame::NoFrame);
    instructionsTab->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));

    QVBoxLayout *instructionsLayout = new QVBoxLayout( instructionsTab );

    instructionsEdit = new KTextEdit( instructionsTab );
    instructionsEdit->setMinimumSize(QSize(360,320));
    instructionsEdit->setMaximumSize(QSize(10000,10000));
    instructionsEdit->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));
    instructionsLayout->addWidget(instructionsEdit);

    spellCheckButton=new QToolButton( instructionsTab );
    spellCheckButton->setIconSet(il->loadIconSet("spellcheck", KIcon::Small));
    QToolTip::add(spellCheckButton, i18n("Check spelling"));
    instructionsLayout->addWidget(spellCheckButton);

    // ------- END OF Recipe Instructions Tab -----------

    tabWidget->insertTab( recipeTab, "" );
    tabWidget->insertTab( ingredientGBox, "" );
    tabWidget->insertTab( instructionsTab, "" );
    tabWidget->changeTab( recipeTab, tr2i18n( "Recipe" ) );
    tabWidget->changeTab( ingredientGBox, tr2i18n( "Ingredients" ) );
    tabWidget->changeTab( instructionsTab, tr2i18n( "Instructions" ) );


    // Dialog design
    tabWidget->resize( size().expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // Initialize internal data
    ingredientComboList=new ElementList;
    unitComboList=new ElementList;
    unsavedChanges=false; // Indicates if there's something not saved yet.
    enableChangedSignal(); // Enables the signal "changed()"

    // Initialize lists
    reloadCombos();

    // Connect signals & Slots
    connect(ingredientBox, SIGNAL(activated(int)), this, SLOT(reloadUnitsCombo(int)));
    connect(changePhotoButton, SIGNAL(clicked()), this, SLOT(changePhoto()));
    connect(upButton, SIGNAL(clicked()), this, SLOT(moveIngredientUp()));
    connect(downButton, SIGNAL(clicked()), this, SLOT(moveIngredientDown()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeIngredient()));
    connect(addButton, SIGNAL(clicked()), this, SLOT(addIngredient()));
    connect(this, SIGNAL(changed()), this, SLOT(recipeChanged()));
    connect(servingsNumInput, SIGNAL(valueChanged(int)), this, SLOT(recipeChanged()));
    connect(titleEdit, SIGNAL(textChanged(const QString&)), this, SLOT(recipeChanged(const QString&)));
    connect(instructionsEdit, SIGNAL(textChanged()), this, SLOT(recipeChanged()));
    connect(addCategoryButton,SIGNAL(clicked()),this,SLOT(addCategory()));
    connect(ingredientBox->lineEdit(), SIGNAL(lostFocus()), this, SLOT(slotIngredientBoxLostFocus()) );
    connect(addAuthorButton,SIGNAL(clicked()),this,SLOT(addAuthor()));
    connect(titleEdit,SIGNAL(textChanged(const QString&)),this, SIGNAL(titleChanged(const QString&)));
    connect(ingredientList,SIGNAL(doubleClicked( QListViewItem*,const QPoint &, int )),this, SLOT(modIngredientAmount( QListViewItem* )));
    connect(ingredientList,SIGNAL(itemRenamed (QListViewItem*)),this, SLOT(saveIngredientAmount( QListViewItem* )));
    	// Function buttons
    connect (saveButton,SIGNAL(clicked()),this,SLOT(save()));
    connect (closeButton,SIGNAL(clicked()),this,SLOT(closeOptions()));
    connect (showButton,SIGNAL(clicked()),this,SLOT(showRecipe()));
    connect (shopButton, SIGNAL (clicked()),this,SLOT(addToShoppingList()));
    connect (resizeButton,SIGNAL(clicked()),this,SLOT(resizeRecipe()));
    connect (spellCheckButton,SIGNAL(clicked()),this,SLOT(spellCheck()));
    connect (this, SIGNAL(enableSaveOption(bool)),this,SLOT(enableSaveButton(bool)));
}


RecipeInputDialog::~RecipeInputDialog()
{
	delete loadedRecipe;
	delete ingredientComboList;
	delete unitComboList;
}

int RecipeInputDialog::loadedRecipeID() const
{
	return loadedRecipe->recipeID;
}

void RecipeInputDialog::loadRecipe(int recipeID)
{
//Disable changed() signals
enableChangedSignal(false);

//Empty current recipe
loadedRecipe->empty();

//Set back to the first page
tabWidget->setCurrentPage(0);

// Load specified Recipe ID
database->loadRecipe(loadedRecipe,recipeID);


reload();

//Enable changed() signals
enableChangedSignal();

}

void RecipeInputDialog::reload(void)
{
ingredientComboList->clear();
unitComboList->clear();
reloadCombos();
servingsNumInput->setValue(1);
amountEdit->setValue(0.0);
ingredientList->clear();

//Load Values in Interface
titleEdit->setText(loadedRecipe->title);
instructionsEdit->setText(loadedRecipe->instructions);
servingsNumInput->setValue(loadedRecipe->persons);

	//show ingredient list
	for ( IngredientList::const_iterator ing_it = loadedRecipe->ingList.begin(); ing_it != loadedRecipe->ingList.end(); ++ing_it )
	{
		QListViewItem* lastElement=ingredientList->lastItem();

		KConfig *config=kapp->config();
		config->setGroup("Numbers");
		QString amount_str;

		if ( config->readBoolEntry("Fraction"))
			amount_str = MixedNumber((*ing_it).amount).toString();
		else
			amount_str = QString::number((*ing_it).amount);

		 //Insert ingredient after last one
		 (void)new QListViewItem (ingredientList,lastElement,(*ing_it).name,amount_str,(*ing_it).units);
	}

	//show photo
	if (!loadedRecipe->photo.isNull()){

//     		//get the photo
    		sourcePhoto = loadedRecipe->photo;

		if( (sourcePhoto.width() > photoLabel->width() || sourcePhoto.height() > photoLabel->height()) || (sourcePhoto.width() < photoLabel->width() && sourcePhoto.height() < photoLabel->height()) )
    			{
			QImage pm = sourcePhoto.convertToImage();
			QPixmap pm_scaled;
			pm_scaled.convertFromImage(pm.smoothScale(photoLabel->width(),photoLabel->height(), QImage::ScaleMin));
			photoLabel->setPixmap(pm_scaled);

			sourcePhoto=pm_scaled; // to save scaled later on
    			}
		else
		{
		photoLabel->setPixmap( sourcePhoto );
		}
	}
	else {
		sourcePhoto = QPixmap(defaultPhoto);
		photoLabel->setPixmap(sourcePhoto);
		}


// Show categories
showCategories();

// Show authors
showAuthors();
}

void RecipeInputDialog::loadIngredientListCombo(void)
{
	database->loadIngredients(ingredientComboList);

	//Populate this data into the ComboBox
	ingredientBox->clear();
	ingredientBox->completionObject()->clear();
	for ( ElementList::const_iterator ing_it = ingredientComboList->begin(); ing_it != ingredientComboList->end(); ++ing_it )
	{
		ingredientBox->insertItem((*ing_it).name);
		ingredientBox->completionObject()->addItem((*ing_it).name);
	}
}

void RecipeInputDialog::loadUnitListCombo(void)
{
	unitBox->clear(); // Empty the combo first
	unitComboList->clear(); // Empty the list also

	int comboIndex=ingredientBox->currentItem();
	int comboCount=ingredientBox->count();

	if (comboCount>0)
	{ // If not, the list may be empty (no ingredient list defined) and crashes while reading
		int selectedIngredient=ingredientComboList->getElement(comboIndex).id;
		database->loadPossibleUnits(selectedIngredient,unitComboList);

		//Populate this data into the ComboBox
		for ( ElementList::const_iterator unit_it = unitComboList->begin(); unit_it != unitComboList->end(); ++unit_it )
		{
			unitBox->insertItem((*unit_it).name);
			unitBox->completionObject()->addItem((*unit_it).name);
		}
	}
}

void RecipeInputDialog::reloadUnitsCombo(int)
{
unitBox->clear();
unitBox->completionObject()->clear();
unitComboList->clear();
loadUnitListCombo();
}

 void RecipeInputDialog::changePhoto(void)
 {
// standard filedialog
    KURL filename = KFileDialog::getOpenURL(QString::null, i18n("*.png *.jpg *.jpeg *.xpm *.gif|Images (*.png *.jpg *.jpeg *.xpm *.gif)"), this);
    QPixmap pixmap (filename.path());
    if (!(pixmap.isNull())) {
      // If photo is bigger than the label, or smaller in width, than photoLabel, scale it
      sourcePhoto = pixmap;
      if( (sourcePhoto.width() > photoLabel->width() || sourcePhoto.height() > photoLabel->height()) || (sourcePhoto.width() < photoLabel->width() && sourcePhoto.height() < photoLabel->height()) ){
        QImage pm = sourcePhoto.convertToImage();
	QPixmap pm_scaled;
	pm_scaled.convertFromImage(pm.smoothScale(photoLabel->width(),photoLabel->height(), QImage::ScaleMin));
	photoLabel->setPixmap(pm_scaled);

	sourcePhoto=pm_scaled; // to save scaled later on
	photoLabel->setPixmap(pm_scaled);
       }
       else{
        photoLabel->setPixmap( sourcePhoto );
       }
	    emit changed();
	  }
 }

void RecipeInputDialog::moveIngredientUp(void)
{
QListViewItem *it=ingredientList->selectedItem();
QListViewItem *iabove = 0;

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
QListViewItem *ibelow = 0;
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
	if ( (ibelow=it->itemBelow()) ) iselect=ibelow;
		else if ( (iabove=it->itemAbove()) ) iselect=iabove;
	int index=ingredientList->itemIndex(it);

	//Now remove the ingredient
	it->setSelected(false);
	delete it;
	if (iselect) ingredientList->setSelected(iselect,true); // be careful iselect->setSelected doesn't work this way.

	// Remove it from the recipe also
	loadedRecipe->ingList.remove( loadedRecipe->ingList.at(index) ); // Note index=0...n in KListView, same as in QPtrlist

	emit changed();
	}


}

void RecipeInputDialog::createNewIngredientIfNecessary()
{
	if ( !ingredientBox->currentText().stripWhiteSpace().isEmpty() &&
	     !ingredientBox->contains(ingredientBox->currentText()) )
	{
		if (unitBox->currentText().stripWhiteSpace() == "")
		{
			QMessageBox::information( this,
			  i18n("Unit missing"),
			  QString(i18n("\"%1\" is being added to the list of ingredients.\n"
			  " Before this can be done, please enter a unit to associate with"
			  " this ingredient.")).arg(ingredientBox->currentText()),
			  QMessageBox::Ok
			  );
			return;
		}

		QString newIngredient(ingredientBox->currentText());
		database->createNewIngredient(newIngredient);

		ingredientComboList->clear();
		loadIngredientListCombo();

		QString saveUnit(unitBox->currentText());
		ingredientBox->setCurrentItem(newIngredient);
		unitBox->setCurrentText(saveUnit);
	}
}

void RecipeInputDialog::createNewUnitIfNecessary()
{
	if ( !unitBox->currentText().stripWhiteSpace().isEmpty() &&
	     !unitBox->contains(unitBox->currentText()) )
	{
		QString newUnit(unitBox->currentText());

		int id;
		if ( ( id = database->findExistingUnitByName(newUnit) ) == -1 )
		{
			database->createNewUnit(newUnit);
			id = database->lastInsertID();
		}

		database->addUnitToIngredient(
		  ingredientComboList->getElement(ingredientBox->currentItem()).id,
		  id );

		reloadUnitsCombo(0);
		unitBox->setCurrentItem(newUnit);
	}
}

bool RecipeInputDialog::checkAmountEdit()
{
	if ( amountEdit->isInputValid() )
		return true;
	else
	{
		KMessageBox::error( this, i18n("Invalid input"),
		  i18n("Amount field contains invalid input.") );
		amountEdit->selectAll();
		return false;
	}
}

void RecipeInputDialog::addIngredient(void)
{
	if ( !checkAmountEdit() )
		return;

	createNewIngredientIfNecessary();
	createNewUnitIfNecessary();

//Add it first to the Recipe list then to the ListView
if ((ingredientBox->count()>0) && (unitBox->count()>0)) // Check first they're not empty otherwise getElement crashes...
{
  Ingredient ing;

  ing.name=ingredientBox->currentText();
  ing.amount=amountEdit->value().toDouble();
  ing.units=unitBox->currentText();
  ing.unitID=unitComboList->getElement(unitBox->currentItem()).id;
  ing.ingredientID=ingredientComboList->getElement(ingredientBox->currentItem()).id;
  loadedRecipe->ingList.append(ing);
  //Append also to the ListView
  QListViewItem* lastElement=ingredientList->lastItem();

  KConfig *config=kapp->config();
  config->setGroup("Numbers");
  QString amount_str;

  if ( config->readBoolEntry("Fraction"))
    amount_str = MixedNumber(ing.amount).toString();
  else
    amount_str = QString::number(ing.amount);

  (void)new QListViewItem (ingredientList,lastElement,ing.name,amount_str,ing.units);

  amountEdit->setFocus(); //put cursor back to amount so user can begin next ingredient
  amountEdit->selectAll();
}

emit changed();
}

void RecipeInputDialog::modIngredientAmount( QListViewItem *it){
  saveButton->setEnabled(false);
  closeButton->setEnabled(false);
  showButton->setEnabled(false);
  shopButton->setEnabled(false);
  resizeButton->setEnabled(false);
  addButton->setEnabled(false);
  removeButton->setEnabled(false);
  downButton->setEnabled(false);
  upButton->setEnabled(false);
  previousAmount = it->text(1);
  ingredientList->rename(it, 1);
}

void RecipeInputDialog::saveIngredientAmount( QListViewItem *it)
{
  bool ok;
	int index=ingredientList->itemIndex(it);
  Ingredient ing = *(loadedRecipe->ingList).at(index);
  KConfig *config=kapp->config();
  config->setGroup("Numbers");

  MixedNumber::Format number_format = (config->readBoolEntry("Fraction")) ? MixedNumber::MixedNumberFormat : MixedNumber::DecimalFormat;

    MixedNumber mn = MixedNumber::fromString( it->text(1), &ok );
    if(ok){
      ing.amount = mn.toDouble();
      it->setText(1, mn.toString( number_format ));
    }
    else{
      it->setText(1, previousAmount);
    }

  if(it->text(1) != previousAmount){
    emit changed();
  }
  closeButton->setEnabled(true);
  showButton->setEnabled(true);
  shopButton->setEnabled(true);
  resizeButton->setEnabled(true);
  addButton->setEnabled(true);
  removeButton->setEnabled(true);
  downButton->setEnabled(true);
  upButton->setEnabled(true);
}

void RecipeInputDialog::recipeChanged(void)
{
if (changedSignalEnabled)
	{
	// Enable Save Button
	emit enableSaveOption(true);
	emit createButton(this,titleEdit->text());
	unsavedChanges=true;

	}

}

void RecipeInputDialog::recipeChanged(const QString &/*t*/)
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

loadedRecipe->photo=sourcePhoto;
loadedRecipe->instructions=instructionsEdit->text();
loadedRecipe->title=titleEdit->text();
loadedRecipe->persons=servingsNumInput->value();
// Now save()
kdDebug()<<"Saving..."<<endl;
database->saveRecipe(loadedRecipe);


}

void RecipeInputDialog::newRecipe(void)
{
loadedRecipe->empty();
ingredientComboList->clear();
unitComboList->clear();
reloadCombos();
QPixmap image(defaultPhoto);
photoLabel->setPixmap(image);
sourcePhoto=image;
instructionsEdit->setText(i18n("Write the recipe instructions here"));
titleEdit->setText(i18n("Write the recipe title here"));
amountEdit->setValue(0.0);
ingredientList->clear();
authorShow->clear();
categoryShow->clear();
servingsNumInput->setValue(1);
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

void RecipeInputDialog::addCategory(void)
{
ElementList categoryList; database->loadCategories(&categoryList);
QPtrList <bool>selected;
findCategoriesInRecipe(categoryList,selected);

SelectCategoriesDialog *editCategoriesDialog=new SelectCategoriesDialog(this,categoryList,&selected);


if ( editCategoriesDialog->exec() == QDialog::Accepted ) { // user presses Ok
   this->loadedRecipe->categoryList.clear();
   editCategoriesDialog->getSelectedCategories(&(loadedRecipe->categoryList)); // get the category list chosen
   emit(this->recipeChanged()); //Indicate that the recipe changed

}

delete editCategoriesDialog;

// show category list
showCategories();


}

// Find which of the elements in the category lists is selected in the recipe (i.e. which categories this recipe belongs to)
void RecipeInputDialog::findCategoriesInRecipe(const ElementList &categoryList, QPtrList <bool>  &selected)
{

for ( ElementList::const_iterator cat_it = categoryList.begin(); cat_it != categoryList.end(); ++cat_it )
	{
	bool *value=new bool;
	if ((loadedRecipe->categoryList.contains(*cat_it)) > 0)  // Recipe contains this category?
		*value=true;
	else
		*value=false;
	selected.append(value);
	}
}

void RecipeInputDialog::showCategories(void)
{
QString categories;
for ( ElementList::const_iterator cat_it = loadedRecipe->categoryList.begin(); cat_it != loadedRecipe->categoryList.end(); ++cat_it )
	{
	if ( !categories.isEmpty() ) categories+=",";
	categories+=(*cat_it).name;
	}
categoryShow->setText(categories);
}

void RecipeInputDialog::slotIngredientBoxLostFocus(void)
{
	if ( ingredientBox->contains(ingredientBox->currentText()) )
	{
		ingredientBox->setCurrentItem(ingredientBox->currentText());
		reloadUnitsCombo(0);
	}
	else
	{
		unitBox->clear();
		unitBox->completionObject()->clear();
		unitComboList->clear();
	}
}

void RecipeInputDialog::addAuthor(void)
{
SelectAuthorsDialog *editAuthorsDialog=new SelectAuthorsDialog(this,loadedRecipe->authorList,database);


if ( editAuthorsDialog->exec() == QDialog::Accepted ) { // user presses Ok
this->loadedRecipe->authorList.clear();
editAuthorsDialog->getSelectedAuthors(&(loadedRecipe->authorList)); // get the category list chosen
emit(this->recipeChanged()); //Indicate that the recipe changed
}

delete editAuthorsDialog;

// show authors list
showAuthors();
}

void RecipeInputDialog::showAuthors(void)
{
QString authors;
for ( ElementList::const_iterator author_it = loadedRecipe->authorList.begin(); author_it != loadedRecipe->authorList.end(); ++author_it )
	{
	if ( !authors.isEmpty() ) authors+=",";
	authors+=(*author_it).name;
	}
authorShow->setText(authors);
}

void RecipeInputDialog::enableSaveButton(bool enabled)
{
saveButton->setEnabled(enabled);
}

void RecipeInputDialog::closeOptions(void)
{

// First check if there's anything unsaved in the recipe
 if (unsavedChanges)
 	{

	switch( KMessageBox::questionYesNoCancel( this,i18n("This recipe contains unsaved changes.\n" "Would you like to save it before closing?"),i18n("Unsaved changes") ) )
		{
		case KMessageBox::Yes:
			save();
			break;
		case KMessageBox::No:
			break;
		case KMessageBox::Cancel:
			return;
		}

	}

emit enableSaveOption(false);
unsavedChanges=false;

// Now close really
emit closeRecipe();


}

void RecipeInputDialog::showRecipe(void)
{
// First check if there's anything unsaved in the recipe

if (loadedRecipe->recipeID == -1)
{
switch( KMessageBox::questionYesNo( this,i18n("You need to save the recipe before displaying it. Would you like to save it now?"),i18n("Unsaved changes") ) )
		{
		case KMessageBox::Yes:
			save();
			break;
		case KMessageBox::No:
			return;
		}
}
else if (unsavedChanges)
 	{

	switch( KMessageBox::questionYesNoCancel( this,i18n("This recipe has changes that will not be displayed unless the recipe is saved. Would you like to save it now?"),i18n("Unsaved changes") ) )
		{
		case KMessageBox::Yes:
			save();
			break;
		case KMessageBox::No:
			break;
		case KMessageBox::Cancel:
			return;
		}

	}

// Now open it really
emit showRecipe(loadedRecipe->recipeID);
}

void RecipeInputDialog::addToShoppingList(void)
{
if (!(loadedRecipe->recipeID>=0))
{
switch( KMessageBox::questionYesNo( this,i18n("The recipe was not saved yet, so it cannot be added to the shopping list. Would you like to save it now?"),i18n("Unsaved changes") ) )
		{
		case KMessageBox::Yes:
			save();
			break;
		case KMessageBox::No:
			return;
		}
}

emit addRecipeToShoppingList(loadedRecipe->recipeID);
QMessageBox::information( this,
			  i18n("Recipe added"),
			  QString(i18n("The recipe titled \"%1\" was successfully added to the shopping list")).arg(loadedRecipe->title),
			  QMessageBox::Ok
			  );


}

void RecipeInputDialog::spellCheck(void)
{
	QString text = instructionsEdit->text();
	KSpellConfig default_cfg(this);
	KSpell::modalCheck( text, &default_cfg );
	KMessageBox::information( this, i18n("Spell check complete.") );

	if ( text != instructionsEdit->text() ) //check if there were changes
		instructionsEdit->setText( text );
}

void RecipeInputDialog::resizeRecipe(void)
{
	loadedRecipe->persons=servingsNumInput->value();
	ResizeRecipeDialog dlg(this,loadedRecipe);

	if ( dlg.exec() == QDialog::Accepted )
		reload();
}

