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
#include <qdatetimeedit.h>
#include <qdragobject.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>

#include <kapplication.h>
#include <kcompletionbox.h>
#include <kspell.h>
#include <kurl.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>

#include "selectauthorsdialog.h"
#include "resizerecipedialog.h"
#include "recipe.h"
#include "datablocks/categorytree.h"
#include "DBBackend/recipedb.h"
#include "selectcategoriesdialog.h"
#include "fractioninput.h"
#include "widgets/kretextedit.h"
#include "image.h" //Initializes default photo


ImageDropLabel::ImageDropLabel(QWidget *parent, QPixmap &_sourcePhoto) : QLabel(parent),
	sourcePhoto(_sourcePhoto)
{
	setAcceptDrops(TRUE);
}

void ImageDropLabel::dragEnterEvent(QDragEnterEvent* event)
{
	event->accept( QImageDrag::canDecode(event) );
}

void ImageDropLabel::dropEvent(QDropEvent* event)
{
	QImage image;
	
	if ( QImageDrag::decode(event, image) ) {
		if( (image.width() > width() || image.height() > height()) || (image.width() < width() && image.height() < height()) ){
			QPixmap pm_scaled;
			pm_scaled.convertFromImage(image.smoothScale(width(),height(), QImage::ScaleMin));
			setPixmap(pm_scaled);
		
			sourcePhoto=pm_scaled; // to save scaled later on
		}
		else{
			setPixmap( image );
			sourcePhoto = image;
		}
		
		emit changed();
	}
}

#define INGLISTVIEWITEM_RTTI 1002
#define INGGRPLISTVIEWITEM_RTTI 1003

class IngListViewItem : public QListViewItem
{
public:
	IngListViewItem(QListView* qlv, const Ingredient &i ):QListViewItem(qlv)
	{
		init(i);
	}

	IngListViewItem(QListView* qlv, QListViewItem *after, const Ingredient &i ):QListViewItem(qlv,after)
	{
		init(i);
	}

	IngListViewItem(QListViewItem* qli, QListViewItem *after, const Ingredient &i ):QListViewItem(qli,after)
	{
		init(i);
	}

	int rtti() const { return INGLISTVIEWITEM_RTTI; }

	Ingredient ingredient() const { return m_ing; }

	void setAmount( double amount )
	{
		amount_str = QString::null;

		if ( amount > 0 ) {
			KConfig *config=kapp->config();
			config->setGroup("Formatting");
	
			if ( config->readBoolEntry("Fraction"))
				amount_str = MixedNumber(amount).toString();
			else
				amount_str = beautify(KGlobal::locale()->formatNumber(amount,5));
		}
		
		m_ing.amount = amount;
	}

	void setUnit( const QString &unit )
	{
		m_ing.units = unit;
	}

	void setPrepMethod( const QString &prepMethod )
	{
		m_ing.prepMethod = prepMethod;
	}

	virtual void setText(int column, const QString &text)
	{
		switch ( column ) {
		case 0: break;
		case 1: setAmount( text.toDouble() ); break;
		case 2: setUnit(text); break;
		case 3: setPrepMethod(text); break;
		default: break;
		}
	}

protected:
	Ingredient m_ing;
	QString amount_str;

public:
	virtual QString text(int column) const {
		switch (column)
		{
		case 0: return m_ing.name; break;
		case 1: return amount_str; break;
		case 2: return m_ing.units; break;
		case 3: return m_ing.prepMethod; break;
		default: return(QString::null);
		}
	}

private:
	void init( const Ingredient &i )
	{
		m_ing = i;
		
		setAmount( i.amount );
	}
};

class IngGrpListViewItem : public QListViewItem
{
public:
	IngGrpListViewItem(QListView* qlv, QListViewItem *after, const QString &group, int id ):QListViewItem(qlv,after)
	{
		init(group,id);
	}

	int rtti() const { return INGGRPLISTVIEWITEM_RTTI; }

	QString group() const { return m_group; }
	int id() const { return m_id; }

	virtual QString text(int column) const {
		switch (column)
		{
		case 0: return m_group+":"; break;
		default: return(QString::null);
		}
	}

protected:
	QString m_group;
	int m_id;

private:
	void init( const QString &group, int id )
	{
		m_group = group;
		m_id = id;
	}
};


RecipeInputDialog::RecipeInputDialog( QWidget* parent, RecipeDB *db): QVBox( parent)
{

// Adjust internal parameters
loadedRecipe=new Recipe();
loadedRecipe->recipeID=-1; // No loaded recipe initially
loadedRecipe->title=QString::null;
loadedRecipe->instructions=QString::null;
database=db;

il=new KIconLoader;

// Tabs
    tabWidget = new QTabWidget( this, "tabWidget" );
    tabWidget->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));
    //tabWidget->setAutoMask( TRUE );



    // Spacers to ingredients and instructions
    //QSpacerItem* spacer_bottomphoto = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
    //layout->addItem( spacer_bottomphoto,6,1);
    //QSpacerItem* spacerToIngredients = new QSpacerItem( 10,200, QSizePolicy::Minimum, QSizePolicy::Fixed );
    //layout->addMultiCell( spacerToIngredients, 1,4,3,3 );
    //QSpacerItem* spacer_rightGBox = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::MinimumExpanding );
    //layout->addItem( spacer_rightGBox,7,4 );

    //------- Recipe Tab -----------------
    // Recipe Photo

    recipeTab =new QGroupBox(tabWidget);
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

    photoLabel=new ImageDropLabel(recipeTab,sourcePhoto);
    photoLabel->setPixmap(image1);
    photoLabel->setFixedSize(QSize(221,166));
    photoLabel->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
    photoLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    recipeLayout->addMultiCellWidget(photoLabel,3,7,1,1);

    QVBox *photoButtonsBox=new QVBox(recipeTab);
    
    changePhotoButton=new QPushButton(photoButtonsBox);
    changePhotoButton->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,QSizePolicy::Ignored));
    changePhotoButton->setText("...");
    QToolTip::add(changePhotoButton,i18n("Select photo"));

    QPushButton *clearPhotoButton=new QPushButton(photoButtonsBox);
    clearPhotoButton->setPixmap(il->loadIcon("clear_left", KIcon::NoGroup,16));
    QToolTip::add(clearPhotoButton,i18n("Clear photo"));

    recipeLayout->addMultiCellWidget(photoButtonsBox,3,7,2,2);


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

    QHBox *serv_prep_box = new QHBox(recipeTab);
    
    QVBox *servingsBox = new QVBox(serv_prep_box);
    servingsBox->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed));
    servingsBox->setSpacing(5);

    servingsLabel = new QLabel(i18n("Servings"),servingsBox);
    servingsNumInput = new KIntNumInput(servingsBox);
    servingsNumInput->setMinValue(1);
    
    QVBox *prepTimeBox = new QVBox(serv_prep_box);
    prepTimeBox->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed));
    prepTimeBox->setSpacing(5);

    (void)new QLabel(i18n("Preparation Time"),prepTimeBox);
    prepTimeEdit = new QTimeEdit(prepTimeBox);
    prepTimeEdit->setMinValue( QTime(0,0) );
    prepTimeEdit->setDisplay( QTimeEdit::Hours | QTimeEdit::Minutes );

    recipeLayout->addWidget(serv_prep_box,6,4);

    //------- END OF Recipe Tab ---------------

    //------- Ingredients Tab -----------------

    ingredientGBox = new QGroupBox(recipeTab);
    ingredientGBox->setFrameStyle(QFrame::NoFrame);
    ingredientGBox->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));
    QGridLayout* ingredientsLayout=new QGridLayout(ingredientGBox);

    // Border
    QSpacerItem* spacerBoxLeft = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );
    ingredientsLayout->addItem( spacerBoxLeft,1,0);
    QSpacerItem* spacerBoxTop = new QSpacerItem( 10,20, QSizePolicy::Minimum, QSizePolicy::Fixed );
    ingredientsLayout->addItem(spacerBoxTop, 0,1);

    //Input Widgets
    QHBox *allInputHBox = new QHBox( ingredientGBox );
    allInputHBox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed));
    
    QVBox *ingredientVBox = new QVBox( allInputHBox );
    QHBox *typeHBox = new QHBox( ingredientVBox );
    QRadioButton *ingredientRadioButton = new QRadioButton( i18n("Ingredient:"), typeHBox );
    QRadioButton *headerRadioButton = new QRadioButton( i18n("Header:"), typeHBox );
    typeButtonGrp = new QButtonGroup();
    typeButtonGrp->insert(ingredientRadioButton);
    typeButtonGrp->insert(headerRadioButton);
    typeButtonGrp->setButton( 0 );
    connect( typeButtonGrp, SIGNAL(clicked(int)), SLOT(typeButtonClicked(int)) );
    //ingredientLabel = new QLabel( i18n("Ingredient:"), ingredientVBox );
    ingredientBox = new KComboBox( TRUE, ingredientVBox );
    ingredientBox->setAutoCompletion( TRUE );
    ingredientBox->lineEdit()->disconnect(ingredientBox); //so hitting enter doesn't enter the item into the box
    ingredientBox->setSizePolicy(QSizePolicy(QSizePolicy::Ignored,QSizePolicy::Fixed));

    QVBox *amountVBox = new QVBox( allInputHBox );
    amountLabel = new QLabel( i18n("Amount:"), amountVBox );
    amountEdit = new FractionInput( amountVBox);
    amountEdit->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed));

    QVBox *unitVBox = new QVBox( allInputHBox );
    unitLabel = new QLabel( i18n("Unit:"), unitVBox );
    unitBox = new KComboBox( TRUE, unitVBox );
    unitBox->setAutoCompletion( TRUE );
    unitBox->lineEdit()->disconnect(unitBox); //so hitting enter doesn't enter the item into the box
    unitBox->setSizePolicy(QSizePolicy(QSizePolicy::Ignored,QSizePolicy::Fixed));
    
    QVBox *prepMethodVBox = new QVBox( allInputHBox );
    prepMethodLabel = new QLabel( i18n("Preparation Method:"), prepMethodVBox );
    prepMethodBox = new KComboBox( TRUE, prepMethodVBox );
    prepMethodBox->setAutoCompletion( TRUE );
    prepMethodBox->lineEdit()->disconnect(prepMethodBox); //so hitting enter doesn't enter the item into the box
    prepMethodBox->setSizePolicy(QSizePolicy(QSizePolicy::Ignored,QSizePolicy::Fixed));
    
    allInputHBox->setStretchFactor( ingredientVBox, 5 );
    allInputHBox->setStretchFactor( amountVBox, 1 );
    allInputHBox->setStretchFactor( unitVBox, 2 );
    allInputHBox->setStretchFactor( prepMethodVBox, 3 );

    ingredientsLayout->addMultiCellWidget(allInputHBox,1,1,1,5);

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
    ingredientList->addColumn(i18n("Preparation Method"));
    ingredientList->setSorting(-1); // Do not sort
    ingredientList->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::MinimumExpanding));
    ingredientList->setItemsRenameable(true);
    ingredientList->setRenameable(0, false); //name
    ingredientList->setRenameable(1, true ); //amount
    ingredientList->setRenameable(2, true ); //units
    ingredientList->setRenameable(3, true ); //prep method
    ingredientList->setDefaultRenameAction(QListView::Reject);
    ingredientsLayout->addMultiCellWidget(ingredientList,3,8,1,3);

    // ------- Recipe Instructions Tab -----------

    instructionsTab = new QGroupBox(recipeTab);
    instructionsTab->setFrameStyle(QFrame::NoFrame);
    instructionsTab->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));

    QVBoxLayout *instructionsLayout = new QVBoxLayout( instructionsTab );

    ElementList ing_list; database->loadIngredients( &ing_list );
    instructionsEdit = new KreTextEdit( instructionsTab, ing_list );
    instructionsEdit->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));
    instructionsEdit->setTabChangesFocus ( true );
    instructionsLayout->addWidget(instructionsEdit);

    spellCheckButton=new QToolButton( instructionsTab );
    spellCheckButton->setIconSet(il->loadIconSet("spellcheck", KIcon::Small));
    QToolTip::add(spellCheckButton, i18n("Check spelling"));
    instructionsLayout->addWidget(spellCheckButton);

    // ------- END OF Recipe Instructions Tab -----------

    tabWidget->insertTab( recipeTab, i18n( "Recipe") );
    tabWidget->insertTab( ingredientGBox, i18n( "Ingredients" ) );
    tabWidget->insertTab( instructionsTab,i18n( "Instructions" ) );
    
    
    // Functions Box
    QHBox* functionsLayout=new QHBox(this);

    functionsBox=new QGroupBox(1,Qt::Vertical,functionsLayout);
    functionsBox->setFrameStyle(QFrame::NoFrame);
    
    saveButton=new QToolButton(functionsBox); saveButton->setIconSet(il->loadIconSet("filesave", KIcon::Small)); saveButton->setEnabled(false);
    showButton=new QToolButton(functionsBox); showButton->setIconSet(il->loadIconSet("viewmag", KIcon::Small));
    closeButton=new QToolButton(functionsBox); closeButton->setIconSet(il->loadIconSet("fileclose", KIcon::Small));
    resizeButton=new QToolButton(functionsBox); resizeButton->setIconSet(il->loadIconSet("2uparrow", KIcon::Small)); //TODO: give me an icon :)
    
    saveButton->setTextLabel(i18n("Save recipe"),true); saveButton->setUsesTextLabel(true);
    showButton->setTextLabel(i18n("Show recipe"),true); showButton->setUsesTextLabel(true);
    closeButton->setTextLabel(i18n("Close"),true); closeButton->setUsesTextLabel(true);
    resizeButton->setTextLabel(i18n("Resize recipe"),true); resizeButton->setUsesTextLabel(true);
    
    functionsLayout->layout()->addItem( new QSpacerItem( 10,10, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed ) );

    // Dialog design
    tabWidget->resize( size().expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // Initialize internal data
    ingredientComboList=new ElementList;
    unitComboList=new ElementList;
    prepMethodComboList=new ElementList;
    unsavedChanges=false; // Indicates if there's something not saved yet.
    enableChangedSignal(); // Enables the signal "changed()"

    // Initialize lists
    reloadCombos();

    // Connect signals & Slots
    connect(ingredientBox, SIGNAL(activated(int)), this, SLOT(reloadUnitsCombo(int)));
    connect(changePhotoButton, SIGNAL(clicked()), this, SLOT(changePhoto()));
    connect(clearPhotoButton, SIGNAL(clicked()), SLOT(clearPhoto()));
    connect(upButton, SIGNAL(clicked()), this, SLOT(moveIngredientUp()));
    connect(downButton, SIGNAL(clicked()), this, SLOT(moveIngredientDown()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeIngredient()));
    connect(addButton, SIGNAL(clicked()), this, SLOT(addIngredient()));
    connect(photoLabel, SIGNAL(changed()), this, SIGNAL(changed()));
    connect(this, SIGNAL(changed()), this, SLOT(recipeChanged()));
    connect(servingsNumInput, SIGNAL(valueChanged(int)), this, SLOT(recipeChanged()));
    connect(prepTimeEdit, SIGNAL(valueChanged(const QTime &)), SLOT(recipeChanged()));
    connect(titleEdit, SIGNAL(textChanged(const QString&)), this, SLOT(recipeChanged(const QString&)));
    connect(instructionsEdit, SIGNAL(textChanged()), this, SLOT(recipeChanged()));
    connect(addCategoryButton,SIGNAL(clicked()),this,SLOT(addCategory()));
    connect(ingredientBox->lineEdit(), SIGNAL(lostFocus()), this, SLOT(slotIngredientBoxLostFocus()) );
    connect(unitBox->lineEdit(), SIGNAL(lostFocus()), this, SLOT(slotUnitBoxLostFocus()) );
    connect(prepMethodBox->lineEdit(), SIGNAL(lostFocus()), this, SLOT(slotPrepMethodBoxLostFocus()) );
    connect(addAuthorButton,SIGNAL(clicked()),this,SLOT(addAuthor()));
    connect(titleEdit,SIGNAL(textChanged(const QString&)),this, SLOT(prepTitleChanged(const QString&)));
    connect(ingredientList,SIGNAL(itemRenamed(QListViewItem*,const QString &,int)), SLOT( syncListView(QListViewItem*,const QString &,int) ));
    
    connect(unitBox->lineEdit(),SIGNAL(returnPressed()),this, SLOT(addIngredient()) );
    connect(ingredientBox->lineEdit(),SIGNAL(returnPressed()),this, SLOT(addIngredient()) );
    connect(prepMethodBox->lineEdit(),SIGNAL(returnPressed()),this, SLOT(addIngredient()) );
    connect(amountEdit,SIGNAL(returnPressed(const QString &)),this, SLOT(addIngredient()) );

    	// Function buttons
    connect (saveButton,SIGNAL(clicked()),this,SLOT(save()));
    connect (closeButton,SIGNAL(clicked()),this,SLOT(closeOptions()));
    connect (showButton,SIGNAL(clicked()),this,SLOT(showRecipe()));
    connect (resizeButton,SIGNAL(clicked()),this,SLOT(resizeRecipe()));
    connect (spellCheckButton,SIGNAL(clicked()),this,SLOT(spellCheck()));
    connect (this, SIGNAL(enableSaveOption(bool)),this,SLOT(enableSaveButton(bool)));
}


RecipeInputDialog::~RecipeInputDialog()
{
	delete loadedRecipe;
	delete ingredientComboList;
	delete unitComboList;
	delete prepMethodComboList;
}

void RecipeInputDialog::prepTitleChanged(const QString &title)
{
	//we don't want the menu to grow due to a long title
	//### KStringHandler::rsqueeze does this but I can't remember when it was added (compatibility issue...)
	QString short_title = title.left(20);
	if ( title.length() > 20 )
		short_title.append("...");

	emit titleChanged(short_title);
}

int RecipeInputDialog::loadedRecipeID() const
{
	return loadedRecipe->recipeID;
}

void RecipeInputDialog::loadRecipe(int recipeID)
{
emit enableSaveOption(false);
unsavedChanges=false;

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
typeButtonGrp->setButton(0); //put back to ingredient input
typeButtonClicked(0);

ingredientComboList->clear();
unitComboList->clear();
reloadCombos();
servingsNumInput->setValue(1);
amountEdit->clear();
ingredientList->clear();

//Load Values in Interface
titleEdit->setText(loadedRecipe->title);
instructionsEdit->setText(loadedRecipe->instructions);
servingsNumInput->setValue(loadedRecipe->persons);
prepTimeEdit->setTime(loadedRecipe->prepTime);

	//show ingredient list
	IngredientList list_copy = loadedRecipe->ingList;
	for ( IngredientList group_list = list_copy.firstGroup(); group_list.count() != 0; group_list = list_copy.nextGroup() )
	{
		QListViewItem* lastElement=ingredientList->lastItem();
		QListViewItem *ing_header = 0;

		QString group = group_list[0].group;
		if ( !group.isEmpty() ) {
			if ( lastElement && lastElement->parent() )
				lastElement = lastElement->parent();

			ing_header = new IngGrpListViewItem(ingredientList,lastElement,group_list[0].group, group_list[0].groupID);
			ing_header->setOpen(true);
		}

		for ( IngredientList::const_iterator ing_it = group_list.begin(); ing_it != group_list.end(); ++ing_it )
		{
			lastElement=ingredientList->lastItem();

			//Insert ingredient after last one
			if ( ing_header ) {
				(void)new IngListViewItem (ing_header,lastElement,*ing_it);
			}
			else {
				if ( lastElement && lastElement->parent() )
					lastElement = lastElement->parent();
				(void)new IngListViewItem (ingredientList,lastElement,*ing_it);
			}

			//update completion
			instructionsEdit->addCompletionItem((*ing_it).name);
		}
	}
// 
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
		photoLabel->setPixmap(QPixmap(defaultPhoto));
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
	QString store_unit = unitBox->currentText();
	unitBox->clear(); // Empty the combo first
	unitBox->completionObject()->clear();

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
	unitBox->lineEdit()->setText(store_unit);
}

void RecipeInputDialog::loadPrepMethodListCombo(void)
{
	QString store_prep = prepMethodBox->currentText();
	database->loadPrepMethods(prepMethodComboList);

	//Populate this data into the ComboBox
	prepMethodBox->clear();
	prepMethodBox->completionObject()->clear();
	for ( ElementList::const_iterator prep_it = prepMethodComboList->begin(); prep_it != prepMethodComboList->end(); ++prep_it )
	{
		prepMethodBox->insertItem((*prep_it).name);
		prepMethodBox->completionObject()->addItem((*prep_it).name);
	}

	prepMethodBox->setCurrentText(store_prep);
}

void RecipeInputDialog::reloadUnitsCombo(int)
{
loadUnitListCombo();
}

void RecipeInputDialog::changePhoto(void)
{
// standard filedialog
    KURL filename = KFileDialog::getOpenURL(QString::null, QString("*.png *.jpg *.jpeg *.xpm *.gif|%1 (*.png *.jpg *.jpeg *.xpm *.gif)").arg(i18n("Images")), this);
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
 
void RecipeInputDialog::clearPhoto(void)
{
	sourcePhoto.resize(0,0);
	photoLabel->setPixmap(QPixmap(defaultPhoto));

	emit changed();
}

void RecipeInputDialog::moveIngredientUp(void)
{
QListViewItem *it=ingredientList->selectedItem();
if ( !it ) return;

QListViewItem *iabove = it->itemAbove();

if (iabove) {
	if ( it->rtti() == INGGRPLISTVIEWITEM_RTTI ) {
		if ( iabove->parent() )
			iabove = iabove->parent();

		int it_index=ingItemIndex(ingredientList,it);
		int iabove_index=ingItemIndex(ingredientList,iabove);

		iabove->moveItem(it); //Move the Item

		loadedRecipe->ingList.move(iabove_index,(iabove->rtti()==INGGRPLISTVIEWITEM_RTTI)?iabove->childCount():1,it_index+it->childCount()-1);
	}
	else {
		int it_index=ingItemIndex(ingredientList,it);
		int iabove_index=ingItemIndex(ingredientList,iabove);
		IngredientList::iterator ing = loadedRecipe->ingList.at(it_index);
	
		if ( iabove->parent() != it->parent() ) {
			if ( iabove->rtti() == INGGRPLISTVIEWITEM_RTTI && it->parent() ) { //move the item out of the group
				it->parent()->takeItem(it);
				ingredientList->insertItem(it);
				it->moveItem((iabove->itemAbove()->parent())?iabove->itemAbove()->parent():iabove->itemAbove()); //Move the Item
			}
			else { //move the item into the group
				ingredientList->takeItem(it);
				iabove->parent()->insertItem(it); 
				it->moveItem(iabove); //Move the Item
			}
	
			ingredientList->setCurrentItem(it); //Keep selected
		}
		else {
			iabove->moveItem(it); //Move the Item
			loadedRecipe->ingList.move(it_index,iabove_index);
		}
	
		if ( it->parent() )
			(*ing).groupID = ((IngGrpListViewItem*)it->parent())->id();
		else
			(*ing).groupID = -1;
	}

	emit changed();
}
}

void RecipeInputDialog::moveIngredientDown(void)
{
QListViewItem *it=ingredientList->selectedItem();
if ( !it ) return;

QListViewItem *ibelow = it->itemBelow();

if (ibelow) {
	if ( it->rtti() == INGGRPLISTVIEWITEM_RTTI ) {
		QListViewItem *next_sibling = it->nextSibling();

		if ( next_sibling ) {
			int it_index=ingItemIndex(ingredientList,it);
			int ibelow_index=ingItemIndex(ingredientList,next_sibling);
	
			it->moveItem(next_sibling); //Move the Item
	
			int skip = 0;
			if ( next_sibling->childCount() > 0 )
				skip = next_sibling->childCount() - 1;
	
			loadedRecipe->ingList.move(it_index,it->childCount(),ibelow_index+skip);
		}
	}
	else {
		int it_index=ingItemIndex(ingredientList,it);
		int ibelow_index=ingItemIndex(ingredientList,ibelow);
		IngredientList::iterator ing = loadedRecipe->ingList.at(it_index);
	
		if ( ibelow->rtti() == INGGRPLISTVIEWITEM_RTTI || (ibelow->parent() != it->parent()) ) {
			if ( ibelow->rtti() == INGGRPLISTVIEWITEM_RTTI && !it->parent() ) { //move the item into the group
				if ( !it->parent() )
					ingredientList->takeItem(it);
				else
					it->parent()->takeItem(it);
	
				ibelow->insertItem(it); 
			}
			else { //move the item out of the group
				QListViewItem *parent = it->parent(); //store this because we can't get it after we do it->takeItem()
				parent->takeItem(it);
				ingredientList->insertItem(it);
				it->moveItem(parent); //Move the Item
			}
	
			ingredientList->setCurrentItem(it); //Keep selected
		}
		else {
			it->moveItem(ibelow); //Move the Item
			loadedRecipe->ingList.move(it_index,ibelow_index);
		}
	
		if ( it->parent() )
			(*ing).groupID = ((IngGrpListViewItem*)it->parent())->id();
		else
			(*ing).groupID = -1;
	}

	emit changed();
}
else if ( it->parent() ) {
	it->parent()->takeItem(it);
	ingredientList->insertItem(it);
	it->moveItem((ingredientList->lastItem()->parent())?ingredientList->lastItem()->parent():ingredientList->lastItem()); //Move the Item
	ingredientList->setCurrentItem(it); //Keep selected

	int it_index = ingItemIndex(ingredientList,it);
	IngredientList::iterator ing = loadedRecipe->ingList.at(it_index);
	(*ing).groupID = -1;
	
	emit changed();
}
}

void RecipeInputDialog::removeIngredient(void)
{
QListViewItem *it=ingredientList->selectedItem();
if ( it && it->rtti() == INGLISTVIEWITEM_RTTI )
{
	// Find the one below or above, and save index first
	QListViewItem *iabove, *ibelow, *iselect=0;
	if ( (ibelow=it->itemBelow()) ) iselect=ibelow;
		else if ( (iabove=it->itemAbove()) ) iselect=iabove;
	int index=ingItemIndex(ingredientList,it);

	//Remove it from the instruction's completion
	instructionsEdit->removeCompletionItem(it->text(0));

	//Now remove the ingredient
	it->setSelected(false);
	delete it;
	if (iselect) ingredientList->setSelected(iselect,true); // be careful iselect->setSelected doesn't work this way.

	// Remove it from the recipe also
	loadedRecipe->ingList.remove( loadedRecipe->ingList.at(index) ); // Note index=0...n in KListView, same as in QPtrlist

	emit changed();
}
else if ( it && it->rtti() == INGGRPLISTVIEWITEM_RTTI ) {
	IngGrpListViewItem *header = (IngGrpListViewItem*)it;

	int index=ingItemIndex(ingredientList,header->firstChild()); //use this same index because after an item is deleted, the next to delete is still the same index number
	for ( QListViewItem* sub_item = header->firstChild(); sub_item; sub_item = sub_item->nextSibling() ) {
		loadedRecipe->ingList.remove( loadedRecipe->ingList.at(index) );

		//Remove it from the instruction's completion
		instructionsEdit->removeCompletionItem(sub_item->text(0));
	}

	delete header;

	emit changed();
}

}

void RecipeInputDialog::createNewIngredientIfNecessary()
{
	if ( !ingredientBox->currentText().stripWhiteSpace().isEmpty() &&
	     !ingredientBox->contains(ingredientBox->currentText()) )
	{
		QString newIngredient(ingredientBox->currentText());
		database->createNewIngredient(newIngredient);

		ingredientComboList->clear();
		loadIngredientListCombo();

		QString saveUnit(unitBox->currentText());
		ingredientBox->setCurrentItem(newIngredient);
		unitBox->setCurrentText(saveUnit);
	}
}

int RecipeInputDialog::createNewUnitIfNecessary( const QString &unit, const QString &ingredient )
{
	if ( !unitBox->contains(unit) ) // returns always false if unit is empty string, even if exists
	{
		int id = database->findExistingUnitByName(unit);
		if ( -1 == id ) 
		{
			database->createNewUnit(unit);
			id = database->lastInsertID();
		}

		if ( !database->ingredientContainsUnit(
			ingredientComboList->findByName(ingredient).id,
			id ) )
		{
			database->addUnitToIngredient(
			  ingredientComboList->findByName(ingredient).id,
			  id );
		}

		loadUnitListCombo();
		return id;
	}
	else
		return unitComboList->findByName(unit).id;
}

int RecipeInputDialog::createNewPrepIfNecessary( const QString &prep )
{
	if ( prep.stripWhiteSpace().isEmpty() ) //no prep method
		return -1;
	else if ( !prepMethodBox->contains(prep) ) //creating new
	{ 
		int id = database->findExistingPrepByName(prep);
		if ( -1 == id )
		{
			database->createNewPrepMethod(prep);
			id = database->lastInsertID();
		}

		loadPrepMethodListCombo();
		return id;
	}
	else //already exists
		return prepMethodComboList->findByName(prep).id;
}

int RecipeInputDialog::createNewGroupIfNecessary( const QString &group )
{
	database->createNewIngGroup(group);
	return database->lastInsertID();
}

bool RecipeInputDialog::checkAmountEdit()
{
	if ( amountEdit->isInputValid() )
		return true;
	else
	{
		KMessageBox::error( this, i18n("Invalid input"),
		  i18n("Amount field contains invalid input.") );
		amountEdit->setFocus();
		amountEdit->selectAll();
		return false;
	}
}

bool RecipeInputDialog::checkBounds()
{
	if ( ingredientBox->currentText().length() > database->maxIngredientNameLength() )
	{
		KMessageBox::error(this,QString(i18n("Ingredient name cannot be longer than %1 characters.")).arg(database->maxIngredientNameLength()));
		ingredientBox->lineEdit()->setFocus();
		ingredientBox->lineEdit()->selectAll();
		return false;
	}

	if ( unitBox->currentText().length() > database->maxUnitNameLength() )
	{
		KMessageBox::error(this,QString(i18n("Unit name cannot be longer than %1 characters.")).arg(database->maxUnitNameLength()));
		unitBox->lineEdit()->setFocus();
		unitBox->lineEdit()->selectAll();
		return false;
	}

	if ( prepMethodBox->currentText().length() > database->maxPrepMethodNameLength() )
	{
		KMessageBox::error(this,QString(i18n("Preparation method cannot be longer than %1 characters.")).arg(database->maxPrepMethodNameLength()));
		prepMethodBox->lineEdit()->setFocus();
		prepMethodBox->lineEdit()->selectAll();
		return false;
	}

	return true;
}

void RecipeInputDialog::addIngredient(void)
{
	if ( typeButtonGrp->selectedId() == 1 ) { //Header
		if ( ingredientBox->currentText().stripWhiteSpace().isEmpty() )
			return;

		int group_id = createNewGroupIfNecessary(ingredientBox->currentText());

		QListViewItem *last_item = ingredientList->lastItem();
		if ( last_item && last_item->parent() )
			last_item = last_item->parent();

		IngGrpListViewItem *ing_header = new IngGrpListViewItem(ingredientList,last_item,ingredientBox->currentText(),group_id);
		ing_header->setOpen(true);

		typeButtonGrp->setButton(0); //put back to ingredient input
		typeButtonClicked(0);
	}
	else { //Ingredient
		if ( !checkAmountEdit() || !checkBounds() || ingredientBox->currentText().stripWhiteSpace().isEmpty() )
			return;

		slotIngredientBoxLostFocus(); //ensure that the matching item in the ingredient box combo list is selected

		createNewIngredientIfNecessary();
		int unitID = createNewUnitIfNecessary(unitBox->currentText().stripWhiteSpace(),ingredientBox->currentText().stripWhiteSpace());
		if ( unitID == -1 ) // shouldn't happen, as empty units are allowed now
			return;
		int prepID = createNewPrepIfNecessary(prepMethodBox->currentText());
	
		//Add it first to the Recipe list then to the ListView
		if ((ingredientBox->count()>0) && (unitBox->count()>0)) // Check first they're not empty otherwise getElement crashes...
		{
			Ingredient ing;
			
			ing.name=ingredientBox->currentText();
			ing.amount=amountEdit->value().toDouble();
			ing.units=unitBox->currentText();
			ing.unitID=unitID;
			ing.ingredientID=ingredientComboList->getElement(ingredientBox->currentItem()).id;
			
			ing.prepMethod=prepMethodBox->currentText();
			ing.prepMethodID=prepID;
			
			//Append also to the ListView
			QListViewItem* lastElement=ingredientList->lastItem();
			if ( lastElement && 
			  ( lastElement->rtti() == INGGRPLISTVIEWITEM_RTTI || (lastElement->parent() && lastElement->parent()->rtti() == INGGRPLISTVIEWITEM_RTTI ) ) ) {
				IngGrpListViewItem *header = (lastElement->parent())?(IngGrpListViewItem*)lastElement->parent():(IngGrpListViewItem*)lastElement;
				ing.groupID = header->id();

				(void)new IngListViewItem(header,lastElement,ing);
			}
			else
				(void)new IngListViewItem(ingredientList,lastElement,ing);

			loadedRecipe->ingList.append(ing);

			//update the completion in the instructions edit
			instructionsEdit->addCompletionItem(ing.name);
			
			emit changed();
		}
	}

	ingredientBox->setFocus(); //put cursor back to the ingredient name so user can begin next ingredient
	ingredientBox->lineEdit()->selectAll();
}

void RecipeInputDialog::syncListView( QListViewItem* it, const QString &new_text, int col )
{
	if ( it->rtti() != INGLISTVIEWITEM_RTTI ) return;

	IngListViewItem *ing_item = (IngListViewItem*)it;

	int index=ingItemIndex(ingredientList,it);
	IngredientList::iterator ing = loadedRecipe->ingList.at(index);

	switch (col)
	{
	case 1: //amount
	{
		bool ok;
		MixedNumber new_mn = MixedNumber::fromString( new_text, &ok );
		MixedNumber prev_mn( (*ing).amount );
		if (ok)
		{
			(*ing).amount = new_mn.toDouble();
			if ( !new_text.isEmpty() )
				ing_item->setAmount( new_mn.toDouble() );
		}
		else
		{
			if ( !new_text.isEmpty() )
				ing_item->setAmount( prev_mn.toDouble() );
		}

		if (new_mn != prev_mn)
		{
			emit changed();
		}
		break;
	}
	case 2: //unit
	{
		QString old_text = (*ing).units;

		if ( new_text.length() > database->maxUnitNameLength() )
		{
			KMessageBox::error(this,QString(i18n("Unit name cannot be longer than %1 characters.")).arg(database->maxUnitNameLength()));
			ing_item->setUnit(old_text);
			break;
		}

		if ( old_text != new_text.stripWhiteSpace() )
		{
			int new_id = createNewUnitIfNecessary(new_text.stripWhiteSpace(),it->text(0).stripWhiteSpace());

			if ( new_id != -1 )
			{
				(*ing).units=new_text;
				(*ing).unitID=new_id;
				
				emit changed();
			}
			else
			{
				ing_item->setUnit(old_text);
			}
		}
		break;
	}
	case 3: //prep method
	{
		QString old_text = (*ing).prepMethod;

		if ( new_text.length() > database->maxPrepMethodNameLength() )
		{
			KMessageBox::error(this,QString(i18n("Preparation method cannot be longer than %1 characters.")).arg(database->maxPrepMethodNameLength()));
			ing_item->setPrepMethod(old_text);
			break;
		}

		if ( old_text != new_text.stripWhiteSpace() )
		{
			int new_id = createNewPrepIfNecessary(new_text.stripWhiteSpace());

			(*ing).prepMethod=new_text;
			(*ing).prepMethodID=new_id;
				
			emit changed();
		}
		break;
	}
	}
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

bool RecipeInputDialog::save (void)
{
//check bounds first
if ( titleEdit->text().length() > database->maxRecipeTitleLength() )
{
	KMessageBox::error(this,QString(i18n("Recipe title cannot be longer than %1 characters.")).arg(database->maxRecipeTitleLength()),i18n("Unable to save recipe"));
	return false;
}

emit enableSaveOption(false);
saveRecipe();
unsavedChanges=false;

return true;
}

void RecipeInputDialog::saveRecipe(void)
{
// Nothing except for the ingredient list (loadedRecipe->ingList)
// was stored before for performance. (recipeID is already there)

loadedRecipe->photo=sourcePhoto;
loadedRecipe->instructions=instructionsEdit->text();
loadedRecipe->title=titleEdit->text();
loadedRecipe->persons=servingsNumInput->value();
loadedRecipe->prepTime = prepTimeEdit->time();

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
instructionsEdit->setText(i18n("Write the recipe instructions here"));
instructionsEdit->clearCompletionItems();
titleEdit->setText(i18n("Write the recipe title here"));
amountEdit->setValue(0.0);
ingredientList->clear();
authorShow->clear();
categoryShow->clear();
servingsNumInput->setValue(1);
prepTimeEdit->setTime( QTime(0,0) );

instructionsEdit->selectAll();

//Set back to the first page
tabWidget->setCurrentPage(0);

//put back to ingredient input
typeButtonGrp->setButton(0);
typeButtonClicked(0);

//Set focus to the title
titleEdit->setFocus();
titleEdit->selectAll();
}

void RecipeInputDialog::reloadCombos(void) //Reloads lists of ingredients, units, and preparation methods
{
loadIngredientListCombo();
loadUnitListCombo();
loadPrepMethodListCombo();
}

bool RecipeInputDialog::everythingSaved()
{
return (!(unsavedChanges));
}

void RecipeInputDialog::addCategory(void)
{
CategoryTree categoryTree; database->loadCategories(&categoryTree);
ElementList categoryList; database->loadCategories(&categoryList);
QMap<Element,bool> selected;
findCategoriesInRecipe(categoryList,selected);

SelectCategoriesDialog *editCategoriesDialog=new SelectCategoriesDialog(this,&categoryTree,selected,database);


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
void RecipeInputDialog::findCategoriesInRecipe(const ElementList &categoryList, QMap<Element,bool> &selected)
{

for ( ElementList::const_iterator cat_it = categoryList.begin(); cat_it != categoryList.end(); ++cat_it )
	{
	bool value;
	if ((loadedRecipe->categoryList.contains(*cat_it)) > 0)  // Recipe contains this category?
		value=true;
	else
		value=false;
	selected.insert(*cat_it,value);kdDebug()<<"category: "<<(*cat_it).name<<" (id :"<<(*cat_it).id<<") is: "<<value<<endl;
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

void RecipeInputDialog::slotUnitBoxLostFocus(void)
{
	if ( unitBox->contains(unitBox->currentText()) )
		unitBox->setCurrentItem(unitBox->currentText());
}

void RecipeInputDialog::slotPrepMethodBoxLostFocus(void)
{
	if ( prepMethodBox->contains(prepMethodBox->currentText()) )
		prepMethodBox->setCurrentItem(prepMethodBox->currentText());
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

int RecipeInputDialog::ingItemIndex( QListView *listview, const QListViewItem *item ) const
{
	if ( !item )
		return -1;

	if ( item == listview->firstChild() )
		return 0;
	else {
		QListViewItemIterator it(listview->firstChild());
		uint j = 0;
		for (; it.current() && it.current() != item; ++it ) {
			if ( it.current()->rtti() == INGLISTVIEWITEM_RTTI )
				j++;
		}
	
		if( !it.current() )
			return -1;
	
		return j;
	}
}

void RecipeInputDialog::typeButtonClicked( int button_id )
{
	amountEdit->setEnabled( !bool(button_id) );
	unitBox->setEnabled( !bool(button_id) );
	prepMethodBox->setEnabled( !bool(button_id) );

	if ( button_id == 1 ) { //Header
		ingItemsCache = ingredientBox->completionObject()->items();
		ingredientBox->completionObject()->clear();
		ingredientBox->clear();
	}
	else {
		ingredientBox->completionObject()->setItems(ingItemsCache);
		ingredientBox->insertStringList(ingItemsCache);
	}
}

#include "recipeinputdialog.moc"
