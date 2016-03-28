/***************************************************************************
*   Copyright © 2003-2005 Unai Garro <ugarro@gmail.com>                   *
*   Copyright © 2003-2005 Cyril Bosselut <bosselut@b1project.com>         *
*   Copyright © 2003-2006 Jason Kivlighn <jkivlighn@gmail.com>            *
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "recipeinputdialog.h"

#include "widgets/imagedroplabel.h"
#include "dialogs/recipeinput/ratinglisteditor.h"
#include "dialogs/recipeinput/recipegeneralinfoeditor.h"
#include "ingredientseditor.h"

#include <QString>


#include <QImage>
#include <qdatetimeedit.h>
#include <QPainter>
#include <QPointer>
#include <KTabWidget>
#include <KTextBrowser>
//Added by qt3to4:
#include <QLabel>
#include <QPixmap>
#include <QMouseEvent>
#include <QGridLayout>
#include <Q3ValueList>
#include <QDragEnterEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QDropEvent>
#include <QGroupBox>

#include <kapplication.h>
#include <kcompletionbox.h>
#include <kurl.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kled.h>
#include <kdialog.h>
#include <kio/netaccess.h>
#include <kvbox.h>

#include "selectauthorsdialog.h"
#include "resizerecipedialog.h"
#include "ingredientparserdialog.h"
#include "editratingdialog.h"
#include "dialogs/createunitdialog.h"
#include "datablocks/recipe.h"
#include "datablocks/rating.h"
#include "datablocks/categorytree.h"
#include "datablocks/unit.h"
#include "datablocks/weight.h"
#include "backends/recipedb.h"
#include "selectcategoriesdialog.h"
#include "dialogs/createunitconversiondialog.h"
#include "dialogs/editpropertiesdialog.h"
#include "widgets/fractioninput.h"
#include "widgets/kretextedit.h"
#include "widgets/inglistviewitem.h"
#include "widgets/ingredientinputwidget.h"

#include "profiling.h"


RecipeInputDialog::RecipeInputDialog( QWidget* parent, RecipeDB *db ) : KVBox( parent )
{

	// Adjust internal parameters
	loadedRecipe = new Recipe();
	loadedRecipe->recipeID = -1; // No loaded recipe initially
	loadedRecipe->title.clear();
	loadedRecipe->instructions.clear();
	database = db;

	// Tabs
	tabWidget = new KTabWidget( this );
	setObjectName( "tabWidget" );
	tabWidget->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );


	//------- Recipe Tab -----------------

	m_recipeGeneralInfoEditor = new RecipeGeneralInfoEditor( this, database );

	//------- END OF Recipe Tab ---------------

	//------- Ingredients Tab -----------------

	ingredientsTab = new QFrame( this );
	ingredientsTab->setFrameStyle( QFrame::NoFrame );
	ingredientsTab->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
	QGridLayout* ingredientsLayout = new QGridLayout( ingredientsTab );

	// Border
	QSpacerItem* spacerBoxLeft = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	ingredientsLayout->addItem( spacerBoxLeft, 1, 0 );

	//Input Widgets
	ingInput = new IngredientInputWidget( database, ingredientsTab );
	ingredientsLayout->addWidget( ingInput, 1, 1, 1, 5, 0 );

	// Spacers to list and buttons
	QSpacerItem* spacerToList = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	ingredientsLayout->addItem( spacerToList, 2, 1 );
	QSpacerItem* spacerToButtons = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	ingredientsLayout->addItem( spacerToButtons, 3, 4 );

	// Add, Up,down,... buttons

	addButton = new KPushButton( ingredientsTab );
	addButton->setFixedSize( QSize( 31, 31 ) );
	addButton->setIcon(KIcon( "add_ingredient" ) );
	addButton->setIconSize( QSize( 16, 16 ) );
	addButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	ingredientsLayout->addWidget( addButton, 3, 5 );

	// Spacer to the rest of buttons
	QSpacerItem* spacerToOtherButtons = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	ingredientsLayout->addItem( spacerToOtherButtons, 4, 5 );

	upButton = new KPushButton( ingredientsTab );
	upButton->setFixedSize( QSize( 31, 31 ) );
	upButton->setIcon( KIcon( "go-up" ) );
	upButton->setIconSize( QSize( 16, 16 ) );
	upButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	ingredientsLayout->addWidget( upButton, 5, 5 );

	downButton = new KPushButton( ingredientsTab );
	downButton->setFixedSize( QSize( 31, 31 ) );
	downButton->setIcon( KIcon( "go-down" ) );
	downButton->setIconSize(  QSize( 16, 16 ) );
	downButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	ingredientsLayout->addWidget( downButton, 6, 5 );

	removeButton = new KPushButton( ingredientsTab );
	removeButton->setFixedSize( QSize( 31, 31 ) );
	removeButton->setIcon( KIcon( "list-remove" ) );
	removeButton->setIconSize( QSize( 16, 16 )  );
	removeButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	ingredientsLayout->addWidget( removeButton, 7, 5 );

	ingParserButton = new KPushButton( ingredientsTab );
	ingParserButton->setFixedSize( QSize( 31, 31 ) );
	ingParserButton->setIcon( KIcon( "edit-paste" ) );
	ingParserButton->setIconSize( QSize( 16, 16 ) );
	ingParserButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	ingredientsLayout->addWidget( ingParserButton, 8, 5 );

		addButton->setToolTip( i18nc( "@info:tooltip", "Add ingredient" ) );
		upButton->setToolTip( i18nc(  "@info:tooltip", "Move ingredient up" ) );
		downButton->setToolTip( i18nc(  "@info:tooltip", "Move ingredient down" ) );
		removeButton->setToolTip( i18nc(  "@info:tooltip", "Remove ingredient" ) );
		ingParserButton->setToolTip( i18nc(  "@info:tooltip", "Paste Ingredients" ) );

	// Ingredient List
	ingredientList = new K3ListView( ingredientsTab );
	ingredientList->addColumn( i18nc( "@title:column", "Ingredient" ) );
	ingredientList->addColumn( i18nc( "@title:column", "Amount" ) );
	ingredientList->setColumnAlignment( 1, Qt::AlignHCenter );
	ingredientList->addColumn( i18nc( "@title:column", "Units" ) );
	ingredientList->addColumn( i18nc( "@title:column", "Preparation Method" ) );
	ingredientList->setSorting( -1 ); // Do not sort
	ingredientList->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::MinimumExpanding ) );
	ingredientList->setItemsRenameable( true );
	ingredientList->setRenameable( 0, false ); //name
	ingredientList->setRenameable( 1, true ); //amount
	ingredientList->setRenameable( 2, true ); //units
	ingredientList->setRenameable( 3, true ); //prep method
	ingredientList->setDefaultRenameAction( Q3ListView::Reject );
	ingredientsLayout->addWidget( ingredientList, 3, 1, 7, 4, 0 );

	QHBoxLayout *propertyStatusLayout = new QHBoxLayout( ingredientsTab );
	propertyStatusLayout->setMargin( 0 );
	propertyStatusLayout->setSpacing( 5 );

	QLabel *propertyLabel = new QLabel( i18nc( "@label", "Property Status:"), ingredientsTab );
	propertyStatusLabel = new QLabel( ingredientsTab );
	propertyStatusLed = new KLed( ingredientsTab );
	propertyStatusLed->setFixedSize( QSize(16,16) );
	propertyStatusButton = new KPushButton( i18nc( "@action:button", "Details..."), ingredientsTab );
	//QPushButton *propertyUpdateButton = new QPushButton( i18n("Update"), ingredientsTab );
	propertyStatusLayout->addWidget( propertyLabel );
	propertyStatusLayout->addWidget( propertyStatusLabel );
	propertyStatusLayout->addWidget( propertyStatusLed );
	propertyStatusLayout->addWidget( propertyStatusButton );
	//propertyStatusLayout->addWidget( propertyUpdateButton );
	QSpacerItem* propertySpacerRight = new QSpacerItem( 10, 10, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
	propertyStatusLayout->addItem( propertySpacerRight );

	KGuiItem updateGuiItem;
	updateGuiItem.setText( i18nc("@action:inmenu Updates the loaded information", "Update") );
	updateGuiItem.setIcon( KIcon( "view-refresh" ) );

	propertyStatusDialog = new KDialog( this );
	propertyStatusDialog->setObjectName( "propertyStatusDialog" );
	propertyStatusDialog->setModal( false );
	propertyStatusDialog->showButtonSeparator( false );
	propertyStatusDialog->setCaption( i18nc( "@title:window ", "Property details") );
	propertyStatusDialog->setButtons( KDialog::Close | KDialog::User1 | KDialog::Help );
	propertyStatusDialog->setDefaultButton( KDialog::Close );
	propertyStatusDialog->setButtonGuiItem( KDialog::User1, updateGuiItem );
	propertyStatusDialog->setHelp("property-status");

	statusTextView = new KTextBrowser(0);
	statusTextView->setOpenLinks( false );
	propertyStatusDialog->setMainWidget( statusTextView );
	propertyStatusDialog->resize( 400, 300 );

	ingredientsLayout->addLayout( propertyStatusLayout, 10, 1, 1, 4, 0 );

	// ------- Recipe Instructions Tab -----------

	instructionsTab = new QFrame( this );
	instructionsTab->setFrameStyle( QFrame::NoFrame );
	instructionsTab->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );

	QVBoxLayout *instructionsLayout = new QVBoxLayout( instructionsTab );
	instructionsTab->setLayout( instructionsLayout );

	instructionsEdit = new KreTextEdit( instructionsTab );
	instructionsEdit->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
	instructionsEdit->setTabChangesFocus ( true );
	instructionsLayout->addWidget( instructionsEdit );

	// ------- END OF Recipe Instructions Tab -----------


	// ------- Recipe Ratings Tab -----------

	ratingListEditor = new RatingListEditor( &loadedRecipe->ratingList, database );
	connect( ratingListEditor, SIGNAL(changed()), this, SLOT(recipeChanged()) );

	// ------- END OF Recipe Ratings Tab -----------


	tabWidget->insertTab( -1, m_recipeGeneralInfoEditor, i18nc( "@title:tab", "Recipe" ) );
	tabWidget->insertTab( -1, ingredientsTab, i18nc( "@title:tab", "Ingredients" ) );
	ingredientsEditor = new IngredientsEditor;
	ingredientsEditor->setDatabase( database );
	//TODO: uncomment this to try the new ingredients editor
	//tabWidget->insertTab( -1, ingredientsEditor, i18nc( "@title:tab", "Ingredients" ) );
	tabWidget->insertTab( -1, instructionsTab, i18nc( "@title:tab", "Instructions" ) );
	tabWidget->insertTab( -1, ratingListEditor, i18nc( "@title:tab", "Ratings" ) );


	// Functions Box
	QHBoxLayout* functionsLayout = new QHBoxLayout;

	functionsBox = new QFrame( this );
	//functionsBox->setFrameStyle( QFrame::NoFrame );
	functionsBox->setLayout( functionsLayout );

	saveButton = new QToolButton;
	saveButton->setIcon( KIcon( "document-save" ) );
	saveButton->setEnabled( false );
	showButton = new QToolButton;
	showButton->setIcon( KIcon( "zoom-original" ) );
	closeButton = new QToolButton;
	closeButton->setIcon( KIcon( "window-close" ) );
	resizeButton = new QToolButton;
	resizeButton->setIcon( KIcon( "arrow-up-double" ) ); //TODO: give me an icon :)

	saveButton->setText( i18nc( "@action:button", "Save recipe" ) );
	saveButton->setToolTip( i18nc( "@info:tooltip", "Save recipe" ) );
	saveButton->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
	showButton->setText( i18nc( "@action:button", "Show recipe" ) );
	showButton->setToolTip( i18nc( "@info:tooltip", "Show recipe" ) );
	showButton->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
	closeButton->setText( i18nc( "@action:button", "Close" ) );
	closeButton->setToolTip( i18nc( "@info:tooltip", "Close" ) );
	closeButton->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
	resizeButton->setText( i18nc( "@action:button", "Resize recipe" ) );
	resizeButton->setToolTip( i18nc( "@info:tooltip", "Resize recipe" ) );
	resizeButton->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );

	functionsLayout->layout()->addItem( new QSpacerItem( 10, 10, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed ) );
	functionsLayout->addWidget( saveButton );
	functionsLayout->addWidget( resizeButton );
	functionsLayout->addWidget( showButton );
	functionsLayout->addWidget( closeButton );

	// Dialog design
	tabWidget->resize( size().expandedTo( minimumSizeHint() ) );
	//KDE4 port
	//clearWState( WState_Polished );

	// Initialize internal data
	unsavedChanges = false; // Indicates if there's something not saved yet.
	enableChangedSignal(); // Enables the signal "changed()"

	// Connect signals & Slots
	connect( m_recipeGeneralInfoEditor , SIGNAL( titleChanged( const QString& ) ),
		this, SLOT( recipeChanged( const QString& ) ) );
	connect( m_recipeGeneralInfoEditor, SIGNAL( changed() ), this, SIGNAL( changed() ) );

	connect( ingredientsEditor, SIGNAL(changed()), this, SLOT(recipeChanged()) );

	connect( upButton, SIGNAL( clicked() ), this, SLOT( moveIngredientUp() ) );
	connect( downButton, SIGNAL( clicked() ), this, SLOT( moveIngredientDown() ) );
	connect( removeButton, SIGNAL( clicked() ), this, SLOT( removeIngredient() ) );
	connect( addButton, SIGNAL( clicked() ), ingInput, SLOT( addIngredient() ) );
	connect( ingParserButton, SIGNAL( clicked() ), this, SLOT( slotIngredientParser() ) );
	connect( this, SIGNAL( changed() ), this, SLOT( recipeChanged() ) );
	connect( instructionsEdit, SIGNAL( textChanged() ), this, SLOT( recipeChanged() ) );
	connect( ingredientList, SIGNAL( itemRenamed( Q3ListViewItem*, const QString &, int ) ), SLOT( syncListView( Q3ListViewItem*, const QString &, int ) ) );

	connect ( ingInput, SIGNAL( ingredientEntered(const Ingredient&) ), this, SLOT( addIngredient(const Ingredient&) ) );
	connect ( ingInput, SIGNAL( headerEntered(const Element&) ), this, SLOT( addIngredientHeader(const Element&) ) );

	connect( propertyStatusLed, SIGNAL(clicked()), SLOT(updatePropertyStatus()) );
	connect( propertyStatusDialog, SIGNAL(user1Clicked()), SLOT(updatePropertyStatus()) );
	connect( propertyStatusButton, SIGNAL(clicked()), propertyStatusDialog, SLOT(show()) );
	connect( statusTextView, SIGNAL(anchorClicked(const QUrl&)), this, SLOT( statusLinkClicked(const QUrl &) ) );

	// Function buttons
	connect ( saveButton, SIGNAL( clicked() ), this, SLOT( save() ) );
	connect ( closeButton, SIGNAL( clicked() ), this, SLOT( closeOptions() ) );
	connect ( showButton, SIGNAL( clicked() ), this, SLOT( showRecipe() ) );
	connect ( resizeButton, SIGNAL( clicked() ), this, SLOT( resizeRecipe() ) );
	connect ( this, SIGNAL( enableSaveOption( bool ) ), this, SLOT( enableSaveButton( bool ) ) );

	connect ( database, SIGNAL( recipeRemoved(int) ), this, SLOT( recipeRemoved(int) ) );

}


RecipeInputDialog::~RecipeInputDialog()
{
	delete loadedRecipe;
}

void RecipeInputDialog::recipeRemoved( int id )
{
	if ( loadedRecipe->recipeID == id ) {
		loadedRecipe->recipeID = -1;
		recipeChanged();
	}
}

int RecipeInputDialog::loadedRecipeID() const
{
	return loadedRecipe->recipeID;
}

void RecipeInputDialog::loadRecipe( int recipeID )
{
	emit enableSaveOption( false );
	unsavedChanges = false;

	//Disable changed() signals
	enableChangedSignal( false );

	//Empty current recipe
	loadedRecipe->empty();

	//Set back to the first page
	tabWidget->setCurrentIndex( 0 );

	// Load specified Recipe ID
	database->loadRecipe( loadedRecipe, RecipeDB::All ^ RecipeDB::Meta ^ RecipeDB::Properties, recipeID );

	reload();

	propertyStatusDialog->hide();
	updatePropertyStatus();

	//Enable changed() signals
	enableChangedSignal();

}

void RecipeInputDialog::reload( void )
{
	m_recipeGeneralInfoEditor->loadRecipe( loadedRecipe );

	//Load Values in Interface
	instructionsEdit->clearCompletionItems();
	instructionsEdit->setPlainText( loadedRecipe->instructions );

	//show ingredient list
	ingredientsEditor->loadIngredientList( &loadedRecipe->ingList );

	//show ingredient list
	IngredientList list_copy = loadedRecipe->ingList;
	for ( IngredientList group_list = list_copy.firstGroup(); group_list.count() != 0; group_list = list_copy.nextGroup() ) {
		Q3ListViewItem * lastElement = ingredientList->lastItem();
		Q3ListViewItem *ing_header = 0;

		QString group = group_list.first().group;
		if ( !group.isEmpty() ) {
			if ( lastElement && lastElement->parent() )
				lastElement = lastElement->parent();

			ing_header = new IngGrpListViewItem( ingredientList, lastElement, group_list[ 0 ].group, group_list[ 0 ].groupID );
			ing_header->setOpen( true );
			lastElement = ing_header;
		}

		for ( IngredientList::const_iterator ing_it = group_list.begin(); ing_it != group_list.end(); ++ing_it ) {
			//Insert ingredient after last one
			if ( ing_header ) {
				lastElement = new IngListViewItem ( ing_header, lastElement, *ing_it );
			}
			else {
				if ( lastElement && lastElement->parent() )
					lastElement = lastElement->parent();
				lastElement = new IngListViewItem ( ingredientList, lastElement, *ing_it );
			}

			for ( Ingredient::SubstitutesList::const_iterator sub_it = (*ing_it).substitutes.begin(); sub_it != (*ing_it).substitutes.end(); ++sub_it ) {
				new IngSubListViewItem ( lastElement, *sub_it );
				lastElement->setOpen(true);
			}

			//update completion
			instructionsEdit->addCompletionItem( ( *ing_it ).name );
		}
	}

	// Show ratings
	ratingListEditor->refresh();
}

void RecipeInputDialog::moveIngredientUp( void )
{
	Q3ListViewItem * it = ingredientList->selectedItem();
	if ( !it || it->rtti() == INGSUBLISTVIEWITEM_RTTI )
		return ;

	Q3ListViewItem *iabove = it->itemAbove();
	while ( iabove && iabove->rtti() == INGSUBLISTVIEWITEM_RTTI )
		iabove = iabove->itemAbove();

	if ( iabove ) {
		if ( it->rtti() == INGGRPLISTVIEWITEM_RTTI ) {
			if ( iabove->parent() )
				iabove = iabove->parent();

			int it_index = ingItemIndex( ingredientList, it );
			int iabove_index = ingItemIndex( ingredientList, iabove );

			iabove->moveItem( it ); //Move the Item

			loadedRecipe->ingList.move( iabove_index, ( iabove->rtti() == INGGRPLISTVIEWITEM_RTTI ) ? iabove->childCount() : 1, it_index + it->childCount() - 1 );
		}
		else {
			const int it_index = ingItemIndex( ingredientList, it );
			int iabove_index = ingItemIndex( ingredientList, iabove );
			//IngredientList::iterator ing = loadedRecipe->ingList.at( it_index );

			if ( iabove->parent() != it->parent() ) {
				if ( iabove->rtti() == INGGRPLISTVIEWITEM_RTTI && it->parent() ) { //move the item out of the group
					it->parent() ->takeItem( it );
					ingredientList->insertItem( it );
					it->moveItem( ( iabove->itemAbove() ->parent() ) ? iabove->itemAbove() ->parent() : iabove->itemAbove() ); //Move the Item
				}
				else { //move the item into the group
					ingredientList->takeItem( it );
					iabove->parent() ->insertItem( it );
					it->moveItem( iabove ); //Move the Item
				}

				ingredientList->setCurrentItem( it ); //Keep selected
			}
			else {
				iabove->moveItem( it ); //Move the Item
				loadedRecipe->ingList.move( it_index, iabove_index );
			}

			IngListViewItem *ing_item = (IngListViewItem*)it;
			int newGroupID;
			if ( it->parent() ) {
				newGroupID = ( ( IngGrpListViewItem* ) it->parent() ) ->id();
			} else {
				newGroupID = -1;
			}
			loadedRecipe->ingList[it_index].groupID = newGroupID;
			ing_item->setGroup( newGroupID );
		}

		emit changed();
	}
}

void RecipeInputDialog::moveIngredientDown( void )
{
	Q3ListViewItem * it = ingredientList->selectedItem();
	if ( !it || it->rtti() == INGSUBLISTVIEWITEM_RTTI )
		return ;

	Q3ListViewItem *ibelow = it->itemBelow();
	while ( ibelow && ibelow->rtti() == INGSUBLISTVIEWITEM_RTTI )
		ibelow = ibelow->itemBelow();

	if ( ibelow ) {
		if ( it->rtti() == INGGRPLISTVIEWITEM_RTTI ) {
			Q3ListViewItem * next_sibling = it->nextSibling();

			if ( next_sibling ) {
				int it_index = ingItemIndex( ingredientList, it );
				int ibelow_index = ingItemIndex( ingredientList, next_sibling );

				it->moveItem( next_sibling ); //Move the Item

				int skip = 0;
				if ( next_sibling->childCount() > 0 )
					skip = next_sibling->childCount() - 1;

				loadedRecipe->ingList.move( it_index, it->childCount(), ibelow_index + skip );
			}
		}
		else {
			int it_index = ingItemIndex( ingredientList, it );
			int ibelow_index = ingItemIndex( ingredientList, ibelow );
			//IngredientList::iterator ing = loadedRecipe->ingList.at( it_index );

			if ( ibelow->rtti() == INGGRPLISTVIEWITEM_RTTI || ( ibelow->parent() != it->parent() ) ) {
				if ( ibelow->rtti() == INGGRPLISTVIEWITEM_RTTI && !it->parent() ) { //move the item into the group
					if ( !it->parent() )
						ingredientList->takeItem( it );
					else
						it->parent() ->takeItem( it );

					ibelow->insertItem( it );
				}
				else { //move the item out of the group
					Q3ListViewItem *parent = it->parent(); //store this because we can't get it after we do it->takeItem()
					parent->takeItem( it );
					ingredientList->insertItem( it );
					it->moveItem( parent ); //Move the Item
				}

				ingredientList->setCurrentItem( it ); //Keep selected
			}
			else {
				it->moveItem( ibelow ); //Move the Item
				loadedRecipe->ingList.move( it_index, ibelow_index );
			}

			IngListViewItem *ing_item = (IngListViewItem*)it;
			int newGroupID;
			if ( it->parent() ) {
				newGroupID = ( ( IngGrpListViewItem* ) it->parent() ) ->id();
			} else {
				newGroupID = -1;
			}
			loadedRecipe->ingList[it_index].groupID = newGroupID;
			ing_item->setGroup( newGroupID );
		}

		emit changed();
	}
	else if ( it->parent() ) {
		it->parent() ->takeItem( it );
		ingredientList->insertItem( it );
		it->moveItem( ( ingredientList->lastItem() ->parent() ) ? ingredientList->lastItem() ->parent() : ingredientList->lastItem() ); //Move the Item
		ingredientList->setCurrentItem( it ); //Keep selected

		int it_index = ingItemIndex( ingredientList, it );
		//IngredientList::iterator ing = loadedRecipe->ingList.at( it_index );
		IngListViewItem *ing_item = (IngListViewItem*)it;
		loadedRecipe->ingList[it_index].groupID = -1;
		ing_item->setGroup( -1 );

		emit changed();
	}
}

void RecipeInputDialog::removeIngredient( void )
{
	Q3ListViewItem * it = ingredientList->selectedItem();
	if ( it && (it->rtti() == INGLISTVIEWITEM_RTTI || it->rtti() == INGSUBLISTVIEWITEM_RTTI) ) {
		Q3ListViewItem *iselect = it->itemBelow();
		while ( iselect && iselect->rtti() == INGSUBLISTVIEWITEM_RTTI )
			iselect = iselect->itemBelow();

		if ( !iselect ) {
			iselect = it->itemAbove();
			while ( iselect && iselect->rtti() == INGSUBLISTVIEWITEM_RTTI )
				iselect = iselect->itemAbove();
		}

		IngListViewItem *ing_item = (IngListViewItem*)it; //we can cast IngSubListViewItem to this too, it's a subclass

		IngredientData &ing = loadedRecipe->ingList.findSubstitute( ing_item->ingredient() );

		//Remove it from the instruction's completion
		instructionsEdit->removeCompletionItem( ing.name );

		loadedRecipe->ingList.removeSubstitute( ing );

		int ingID = ing_item->ingredient().ingredientID;
		QMap<int,QString>::iterator map_it;
		if ( (map_it = propertyStatusMapRed.find(ingID)) != propertyStatusMapRed.end() )
			propertyStatusMapRed.erase( map_it );
		else if ( (map_it = propertyStatusMapYellow.find(ingID)) != propertyStatusMapYellow.end() )
			propertyStatusMapYellow.erase( map_it );
		showStatusIndicator();

		//Now remove the ingredient
		it->setSelected( false );
		delete it;
		if ( iselect )
			ingredientList->setSelected( iselect, true ); // be careful iselect->setSelected doesn't work this way.

		emit changed();
	}
	else if ( it && it->rtti() == INGGRPLISTVIEWITEM_RTTI ) {
		IngGrpListViewItem * header = ( IngGrpListViewItem* ) it;

		for ( IngListViewItem * sub_item = (IngListViewItem*)header->firstChild(); sub_item; sub_item = (IngListViewItem*)sub_item->nextSibling() ) {
			IngredientData &ing = loadedRecipe->ingList.findSubstitute( sub_item->ingredient() );

			//Remove it from the instruction's completion
			instructionsEdit->removeCompletionItem( ing.name );

			loadedRecipe->ingList.removeSubstitute( ing );

			int ingID = sub_item->ingredient().ingredientID;
			QMap<int,QString>::iterator map_it;
			if ( (map_it = propertyStatusMapRed.find(ingID)) != propertyStatusMapRed.end() )
				propertyStatusMapRed.erase( map_it );
			else if ( (map_it = propertyStatusMapYellow.find(ingID)) != propertyStatusMapYellow.end() )
				propertyStatusMapYellow.erase( map_it );
			showStatusIndicator();
		}

		delete header;

		emit changed();
	}

}

void RecipeInputDialog::syncListView( Q3ListViewItem* it, const QString &new_text, int col )
{
	if ( it->rtti() != INGLISTVIEWITEM_RTTI && it->rtti() != INGSUBLISTVIEWITEM_RTTI )
		return ;

	IngListViewItem *ing_item = ( IngListViewItem* ) it;

	IngredientData &new_ing = loadedRecipe->ingList.findSubstitute( ing_item->ingredient() );

	switch ( col ) {
	case 1:  //amount
		{
			bool ok;

			Ingredient new_ing_amount;
			new_ing_amount.setAmount(new_text,&ok);

			if ( ok )
			{
				if ( new_ing.amount != new_ing_amount.amount ||
					 new_ing.amount_offset != new_ing_amount.amount_offset ) {
					new_ing.amount = new_ing_amount.amount;
					new_ing.amount_offset = new_ing_amount.amount_offset;
					if ( !new_text.isEmpty() )
						ing_item->setAmount( new_ing_amount.amount, new_ing_amount.amount_offset );

					new_ing.amount = new_ing_amount.amount;
					new_ing.amount_offset = new_ing_amount.amount_offset;
					emit changed();
				}
			}
			else
			{
				if ( !new_text.isEmpty() )
					ing_item->setAmount( new_ing.amount, new_ing.amount_offset );
			}

			break;
		}
	case 2:  //unit
		{
			Unit old_unit = new_ing.units;

			if ( new_text.length() > int(database->maxUnitNameLength()) )
			{
				KMessageBox::error( this, i18ncp( "@info", "Unit name cannot be longer than 1 character.", "Unit name cannot be longer than %1 characters.", database->maxUnitNameLength() ) );
				ing_item->setUnit( old_unit );
				break;
			}

			QString approp_unit = new_ing.units.determineName(new_ing.amount, /*useAbbrev=*/false);
			if ( approp_unit != new_text.trimmed() )
			{
				Unit new_unit;
				int new_id = IngredientInputWidget::createNewUnitIfNecessary( new_text.trimmed(), new_ing.amount > 1, ing_item->ingredient().ingredientID, new_unit, database );

				if ( new_id != -1 ) {
					new_ing.units = new_unit;
					new_ing.units.setId(new_id);

					ing_item->setUnit( new_ing.units );

					updatePropertyStatus();
					emit changed();
				}
				else {
					ing_item->setUnit( old_unit );
				}
			}
			break;
		}
	case 3:  //prep method
		{
			QString old_text = new_ing.prepMethodList.join(",");

			QStringList prepMethodList;
			if (new_text.isEmpty())
				prepMethodList = QStringList();
			else
				prepMethodList = new_text.split( ',', QString::SkipEmptyParts);

			for ( QStringList::const_iterator it = prepMethodList.constBegin(); it != prepMethodList.constEnd(); ++it ) {
				if ( (*it).trimmed().length() > int(database->maxPrepMethodNameLength()) )
				{
					KMessageBox::error( this, i18ncp( "@info", "Preparation method cannot be longer than 1 character.", "Preparation method cannot be longer than %1 characters." , database->maxPrepMethodNameLength() ) );
					ing_item->setPrepMethod( old_text );
					break;
				}
			}

			if ( old_text != new_text.trimmed() )
			{
				new_ing.prepMethodList = ElementList::split(",",new_text.trimmed());
				QList<int> new_ids = IngredientInputWidget::createNewPrepIfNecessary( new_ing.prepMethodList, database );

				QList<int>::const_iterator id_it = new_ids.constBegin();
				for ( ElementList::iterator it = new_ing.prepMethodList.begin(); it != new_ing.prepMethodList.end(); ++it, ++id_it ) {
					(*it).id = *id_it;
				}

				updatePropertyStatus();
				emit changed();
			}
			break;
		}
	}
}

void RecipeInputDialog::recipeChanged( void )
{
	if ( changedSignalEnabled ) {
		// Enable Save Button
		emit enableSaveOption( true );
		unsavedChanges = true;

	}

}

void RecipeInputDialog::recipeChanged( const QString & t )
{
	ingredientsEditor->setRecipeTitle( t );
	emit createButton( this, t );
	recipeChanged();
}

void RecipeInputDialog::enableChangedSignal( bool en )
{
	changedSignalEnabled = en;
}

bool RecipeInputDialog::save ( void )
{
	emit enableSaveOption( false );
	saveRecipe();
	unsavedChanges = false;

	return true;
}

void RecipeInputDialog::saveRecipe()
{
	// Some elements wasn't stored before for performance.
	// (recipeID is already there)
	m_recipeGeneralInfoEditor->updateRecipe();
	loadedRecipe->instructions = instructionsEdit->toPlainText();

	// Dump the contents of the ingredient list editor to
	// loadedRecipe->ingredientList
	ingredientsEditor->updateIngredientList();

	// The recipe instructions and ratings are now already updated
	// in loadedRecipe, save everything in the database
	database->saveRecipe( loadedRecipe );
}

void RecipeInputDialog::newRecipe( void )
{
	loadedRecipe->empty();

	m_recipeGeneralInfoEditor->loadRecipe( loadedRecipe );

	ingredientList->clear();
	ingInput->clear();

	ingredientsEditor->loadIngredientList( &loadedRecipe->ingList );

	instructionsEdit->setText( i18nc( "@label:textbox", "Write the recipe instructions here" ) );
	instructionsEdit->selectAll();

	ratingListEditor->clear();

	//Set back to the first page
	tabWidget->setCurrentIndex( 0 );

	//clear status info
	propertyStatusMapRed.clear();
	propertyStatusMapYellow.clear();
	showStatusIndicator();

	// Enable Save Button
	emit enableSaveOption( true );
	emit createButton( this, i18n( "New Recipe" ) );
	unsavedChanges = true;

	// Enable the changed signals.
	enableChangedSignal();
}

bool RecipeInputDialog::everythingSaved()
{
	return ( !( unsavedChanges ) );
}

void RecipeInputDialog::enableSaveButton( bool enabled )
{
	saveButton->setEnabled( enabled );
}

void RecipeInputDialog::closeOptions( void )
{

	// First check if there's anything unsaved in the recipe
	if ( unsavedChanges ) {

		switch ( KMessageBox::questionYesNoCancel( this, i18nc( "@info", "This recipe contains unsaved changes.\n" "Would you like to save it before closing?" ), i18nc( "@title:window", "Unsaved changes" ) ) ) {
		case KMessageBox::Yes:
			save();
			break;
		case KMessageBox::No:
			break;
		case KMessageBox::Cancel:
			return ;
		}

	}

	emit enableSaveOption( false );
	unsavedChanges = false;

	// Now close really
	enableChangedSignal( false );
	emit closeRecipe();
}

void RecipeInputDialog::showRecipe( void )
{
	// First check if there's anything unsaved in the recipe

	if ( loadedRecipe->recipeID == -1 ) {
		switch ( KMessageBox::questionYesNo( this, i18nc( "@info", "You need to save the recipe before displaying it. Would you like to save it now?" ), i18nc( "@title:window", "Unsaved changes" ) ) ) {
		case KMessageBox::Yes:
			save();
			break;
		case KMessageBox::No:
			return ;
		}
	}
	else if ( unsavedChanges ) {

		switch ( KMessageBox::questionYesNoCancel( this, i18nc( "@info", "This recipe has changes that will not be displayed unless the recipe is saved. Would you like to save it now?" ), i18nc( "@title:window", "Unsaved changes" ) ) ) {
		case KMessageBox::Yes:
			save();
			break;
		case KMessageBox::No:
			break;
		case KMessageBox::Cancel:
			return ;
		}

	}

	// Now open it really
	emit showRecipe( loadedRecipe->recipeID );
}

void RecipeInputDialog::addCategory( void )
{
       QPointer<SelectCategoriesDialog> editCategoriesDialog = new SelectCategoriesDialog( this, loadedRecipe->categoryList, database );

       if ( editCategoriesDialog->exec() == QDialog::Accepted ) { // user presses Ok
		loadedRecipe->categoryList.clear();
		editCategoriesDialog->getSelectedCategories( &( loadedRecipe->categoryList ) ); // get the category list chosen
		emit( recipeChanged() ); //Indicate that the recipe changed
       }

       delete editCategoriesDialog;
}


void RecipeInputDialog::resizeRecipe( void )
{
	m_recipeGeneralInfoEditor->updateRecipe();
	QPointer<ResizeRecipeDialog> dlg = new ResizeRecipeDialog( this, loadedRecipe );

	if ( dlg->exec() == QDialog::Accepted )
		reload();
	
	delete dlg;
}

int RecipeInputDialog::ingItemIndex( Q3ListView *listview, const Q3ListViewItem *item ) const
{
	if ( !item )
		return -1;

	if ( item == listview->firstChild() )
		return 0;
	else {
		Q3ListViewItemIterator it( listview->firstChild() );
		int j = 0;
		for ( ; it.current() && it.current() != item; ++it ) {
			if ( it.current() ->rtti() == INGLISTVIEWITEM_RTTI ) {
				if ( !it.current()->parent() || it.current()->parent()->rtti() == INGGRPLISTVIEWITEM_RTTI )
					j++;
			}
		}

		if ( !it.current() )
			return -1;

		return j;
	}
}

void RecipeInputDialog::slotIngredientParser()
{
	UnitList units;
	database->loadUnits(&units);
	QPointer<IngredientParserDialog> dlg = new IngredientParserDialog(units,this);
	if ( dlg->exec() == QDialog::Accepted ) {
		IngredientList ings = dlg->ingredients();
		QStringList usedGroups;
		bool haveHeader = ingredientList->lastItem() && ingredientList->lastItem()->rtti() == INGGRPLISTVIEWITEM_RTTI;
		for ( IngredientList::iterator it = ings.begin(); it != ings.end(); ++it ) {
			if ( !(*it).group.isEmpty() && usedGroups.indexOf((*it).group) == -1 ) {
				int id = IngredientInputWidget::createNewGroupIfNecessary((*it).group,database);
				addIngredientHeader( Element((*it).group, id) );
				haveHeader = true;
				usedGroups << (*it).group;
			}
			(*it).ingredientID = IngredientInputWidget::createNewIngredientIfNecessary((*it).name,database);
			(*it).units.setId(IngredientInputWidget::createNewUnitIfNecessary((*it).units.name(),false,(*it).ingredientID,(*it).units,database));

			QList<int> prepIDs = IngredientInputWidget::createNewPrepIfNecessary((*it).prepMethodList,database);
			QList<int>::const_iterator prep_id_it = prepIDs.constBegin();
			for ( ElementList::iterator prep_it = (*it).prepMethodList.begin(); prep_it != (*it).prepMethodList.end(); ++prep_it, ++prep_id_it ) {
				(*prep_it).id = *prep_id_it;
			}

			addIngredient( *it, !haveHeader );

			if ( usedGroups.count() > 0 && (*it).group.isEmpty() ) {
				Q3ListViewItem *last_item = ingredientList->lastItem();
				if ( last_item->parent() ) {
					last_item->parent()->takeItem( last_item );
					ingredientList->insertItem( last_item );
					last_item->moveItem( ingredientList->lastItem()->parent() );
				}
			}
		}
	}
	delete dlg;
}

void RecipeInputDialog::addIngredient( const Ingredient &ing, bool noHeader )
{
	Ingredient ingCopy = ing;

	//Append to the ListView
	Q3ListViewItem* lastElement = ingredientList->lastItem();
	while ( lastElement && lastElement->rtti() == INGSUBLISTVIEWITEM_RTTI )
		lastElement = lastElement->itemAbove();

	if ( noHeader && lastElement )
		lastElement = (lastElement->parent())?lastElement->parent():lastElement;

	if ( !noHeader && lastElement &&
		( lastElement->rtti() == INGGRPLISTVIEWITEM_RTTI || ( lastElement->parent() && lastElement->parent() ->rtti() == INGGRPLISTVIEWITEM_RTTI ) ) )
	{
		IngGrpListViewItem * header = ( lastElement->parent() ) ? ( IngGrpListViewItem* ) lastElement->parent() : ( IngGrpListViewItem* ) lastElement;

		ingCopy.groupID = header->id();

		lastElement = new IngListViewItem( header, lastElement, ingCopy );
		for ( Ingredient::SubstitutesList::const_iterator it = ingCopy.substitutes.constBegin(); it != ingCopy.substitutes.constEnd(); ++it ) {
			new IngSubListViewItem( lastElement, *it );
		}
		lastElement->setOpen(true);
	}
	else {
		lastElement = new IngListViewItem( ingredientList, lastElement, ingCopy );
		for ( Ingredient::SubstitutesList::const_iterator it = ing.substitutes.constBegin(); it != ing.substitutes.constEnd(); ++it ) {
			new IngSubListViewItem( lastElement, *it );
		}
		lastElement->setOpen(true);
	}

	//append to recipe
	loadedRecipe->ingList.append( ingCopy );

	//update the completion in the instructions edit
	instructionsEdit->addCompletionItem( ingCopy.name );

	updatePropertyStatus( ingCopy, true );

	emit changed();
}

void RecipeInputDialog::addIngredientHeader( const Element &header )
{
	Q3ListViewItem *last_item = ingredientList->lastItem();
	if ( last_item && last_item->parent() )
		last_item = last_item->parent();

	IngGrpListViewItem *ing_header = new IngGrpListViewItem( ingredientList, last_item, header.name, header.id );
	ing_header->setOpen( true );
}

void RecipeInputDialog::updatePropertyStatus()
{
	propertyStatusMapRed.clear();
	propertyStatusMapYellow.clear();

	for ( IngredientList::const_iterator ing_it = loadedRecipe->ingList.begin(); ing_it != loadedRecipe->ingList.end(); ++ing_it ) {
		updatePropertyStatus( *ing_it, false );
	}

	showStatusIndicator();
}

void RecipeInputDialog::updatePropertyStatus( const Ingredient &ing, bool updateIndicator )
{
	IngredientPropertyList ingPropertyList;
	database->loadProperties( &ingPropertyList, ing.ingredientID );

	if ( ingPropertyList.count() == 0 ) {
		propertyStatusMapRed.insert(ing.ingredientID, QString(
			i18nc("@info", "<b>%1:</b> No nutrient information available. "
			"<a href=\"ingredient#%2\">Provide nutrient information.</a>",
			ing.name,
			QString::number(ing.ingredientID))));
	}

	QMap<int,bool> ratioCache; //unit->conversion possible
	IngredientPropertyList::const_iterator prop_it;
	for ( prop_it = ingPropertyList.constBegin(); prop_it != ingPropertyList.constEnd(); ++prop_it ) {
		Ingredient result;

		QMap<int,bool>::const_iterator cache_it = ratioCache.constFind((*prop_it).perUnit.id());
		if ( cache_it == ratioCache.constEnd() ) {
			RecipeDB::ConversionStatus status = database->convertIngredientUnits( ing, (*prop_it).perUnit, result );
			ratioCache.insert((*prop_it).perUnit.id(),status==RecipeDB::Success||status==RecipeDB::MismatchedPrepMethod);

			switch ( status ) {
			case RecipeDB::Success: break;
			case RecipeDB::MissingUnitConversion: {
				if ( ing.units.type() != Unit::Other && ing.units.type() == (*prop_it).perUnit.type() ) {
					propertyStatusMapRed.insert(ing.ingredientID,
						i18nc( "@info", "<b>%3:</b> Unit conversion missing for conversion from '%1' to '%2'"
						,(ing.units.name().isEmpty()?i18n("-No unit-"):ing.units.name())
						,((*prop_it).perUnit.name())
						,ing.name));
				} else {
					WeightList weights = database->ingredientWeightUnits( ing.ingredientID );
					Q3ValueList< QPair<int,int> > usedIds;
					QStringList missingConversions;
					for ( WeightList::const_iterator weight_it = weights.constBegin(); weight_it != weights.constEnd(); ++weight_it ) {
						//skip entries that only differ in how it's prepared
						QPair<int,int> usedPair((*weight_it).perAmountUnitId(),(*weight_it).weightUnitId());
						if ( usedIds.find(usedPair) != usedIds.end() )
							continue;

						QString toUnit = database->unitName((*weight_it).perAmountUnitId()).name();
						if ( toUnit.isEmpty() ) toUnit = i18nc( "@info", "-No unit-");

						QString fromUnit = database->unitName((*weight_it).weightUnitId()).name();
						if ( fromUnit.isEmpty() ) fromUnit = i18nc( "@info", "-No unit-");

						QString ingUnit = ing.units.name();
						if ( ingUnit.isEmpty() ) ingUnit = i18nc( "@info", "-No unit-");

						QString propUnit = (*prop_it).perUnit.name();
						if ( propUnit.isEmpty() ) propUnit = i18nc( "@info", "-No unit-");

						missingConversions << conversionPath( ingUnit, toUnit, fromUnit, propUnit);
					}
					propertyStatusMapRed.insert(ing.ingredientID,
						i18nc("@info", "<b>%1:</b> Either <a href=\"ingredient#%3\">enter an appropriate "
						"ingredient weight entry</a>, or provide conversion information to "
						"perform one of the following conversions: %2",
					  	ing.name,
						("<ul><li>"+missingConversions.join("</li><li>")+"</li></ul>"),
						QString::number(ing.ingredientID))
					);
				}
				break;
			}
			case RecipeDB::MissingIngredientWeight:
				propertyStatusMapRed.insert(ing.ingredientID, QString(
					i18nc("@info", "<b>%1:</b> No ingredient weight entries. <a href=\"ingredient#%2\">Provide "
					"ingredient weight.</a>",
					ing.name, QString::number(ing.ingredientID))));
				break;
			case RecipeDB::MismatchedPrepMethod:
				if ( ing.prepMethodList.count() == 0 )
					propertyStatusMapRed.insert(ing.ingredientID,QString(
						i18nc("@info", "<b>%1:</b> There is no ingredient weight entry for when no "
						"preparation method is specified. <a href=\"ingredient#%2\">Provide "
						"ingredient weight.</a>",
						ing.name, QString::number(ing.ingredientID))));
				else
					propertyStatusMapRed.insert(ing.ingredientID,QString(
						i18nc("@info", "<b>%1:</b> There is no ""ingredient weight entry for when prepared "
						"in any of the following manners: %2<a href=\"ingredient#%3\">Provide "
						"ingredient weight.</a>",
						ing.name,
						"<ul><li>"+ing.prepMethodList.join("</li><li>")+"</li></ul>",
						QString::number(ing.ingredientID))));
				break;
			case RecipeDB::MismatchedPrepMethodUsingApprox:
				propertyStatusMapYellow.insert(ing.ingredientID,QString(
					i18nc("@info", "<b>%1:</b> There is no ingredient weight entry for when prepared in any of "
					"the following manners (defaulting to a weight entry without a preparation "
					"method specified): "
					"%2<a href=\"ingredient#%3\">Provide ingredient weight.</a>",
					ing.name,
					"<ul><li>"+ing.prepMethodList.join("</li><li>")+"</li></ul>",
					QString::number(ing.ingredientID))));
				break;
			default: kDebug()<<"Code error: Unhandled conversion status code "<<status; break;
			}
		}
	}

	if ( updateIndicator )
		showStatusIndicator();
}

void RecipeInputDialog::showStatusIndicator()
{
	if ( propertyStatusMapRed.count() == 0 ) {
		if ( propertyStatusMapYellow.count() == 0 ) {
			propertyStatusLed->setColor( Qt::green );
			propertyStatusLabel->setText( i18nc(
				"@info Property information for a recipe is complete",
				"Complete") );
			propertyStatusButton->setEnabled(false);
		}
		else {
			propertyStatusLed->setColor( Qt::yellow );
			propertyStatusLabel->setText( i18nc(
				"@info Property information for a recipe is complete, but...",
				"Complete, but approximations made") );
			propertyStatusButton->setEnabled(true);
		}
	}
	else {
		propertyStatusLed->setColor( Qt::red );
		propertyStatusLabel->setText( i18nc(
			"@info Property information for a recipe is incomplete",
			"Incomplete") );
		propertyStatusButton->setEnabled(true);
	}

	if ( propertyStatusMapRed.count() == 0 && propertyStatusMapYellow.count() == 0 )
		propertyStatusDialog->hide();
	else
		statusTextView->setText(statusMessage());
}

QString RecipeInputDialog::statusMessage() const
{
	QString statusMessage;

	if ( propertyStatusMapRed.count() > 0 ) {
		statusMessage.append( i18nc("@info", "The nutrient information for this recipe is incomplete because the following information is missing:") );
		statusMessage.append("<ul>");
		for ( QMap<int,QString>::const_iterator it = propertyStatusMapRed.begin(); it != propertyStatusMapRed.end(); ++it ) {
			statusMessage.append("<li>");
			statusMessage.append(it.value());
			statusMessage.append("</li>");
		}
		statusMessage.append("</ul>");
	}

	if ( propertyStatusMapYellow.count() > 0 ) {
		statusMessage.append( i18nc("@info", "The following approximations will be made when determining nutrient information:") );
		statusMessage.append("<ul>");
		for ( QMap<int,QString>::const_iterator it = propertyStatusMapYellow.begin(); it != propertyStatusMapYellow.end(); ++it ) {
			statusMessage.append("<li>");
			statusMessage.append(it.value());
			statusMessage.append("</li>");
		}
		statusMessage.append("</ul>");
	}

	return statusMessage;
}

QString RecipeInputDialog::conversionPath( const QString &ingUnit, const QString &toUnit, const QString &fromUnit, const QString &propUnit ) const
{
	QString path = '\''+ingUnit+'\'';

	QString lastUnit = ingUnit;
	if ( lastUnit != toUnit ) {
		path.append(" =&gt; '"+toUnit+'\'');
		lastUnit = toUnit;
	}
	if ( lastUnit != fromUnit ) {
		path.append(" =&gt; '"+fromUnit+'\'');
		lastUnit = fromUnit;
	}
	if ( lastUnit != propUnit ) {
		path.append(" =&gt; '"+propUnit+'\'');
		lastUnit = propUnit;
	}
	return path;
}

void RecipeInputDialog::statusLinkClicked( const QUrl &link )
{
	QString linkString = link.toString();
	if (linkString.startsWith("ingredient#")) {
		int ingID = linkString.mid(linkString.indexOf("#")+1).toInt();
		QString ingName = database->ingredientName(ingID);
		QPointer<EditPropertiesDialog> d = new EditPropertiesDialog( ingID, ingName, database, this );
		d->exec();
		delete d;
	} else if (linkString.startsWith("unit#")) { //FIXME: Not used?
		QString unitIDs = linkString.mid(linkString.indexOf("#")+1);
		QStringList idList = unitIDs.split(',', QString::SkipEmptyParts );
		int unitFrom = idList[0].toInt();
		ElementList toUnits;
		int lastUnit = -1;
		for (int i = 1; i < idList.count(); ++i ) {
			int id = idList[i].toInt();
			if ( id != lastUnit ) {
				toUnits << Element(database->unitName(id).name(),id);
				lastUnit = id;
			}
		}
		QPointer<CreateUnitConversionDialog> dlg = new CreateUnitConversionDialog( Element(database->unitName(unitFrom).name()), toUnits, this );
		if ( dlg->exec() == QDialog::Accepted ) {
			UnitRatio ratio(dlg->toUnitID(), unitFrom, dlg->ratio());
			if (ratio.ratio() >= 0 ) {
				database->saveUnitRatio(&ratio);
			}
		}
		delete dlg;
	}
	updatePropertyStatus();
}

void RecipeInputDialog::reloadCheckSpelling()
{
	if ( instructionsEdit->checkSpellingEnabled() ) {                                                                                                              
		instructionsEdit->setCheckSpellingEnabled( false );
		instructionsEdit->setCheckSpellingEnabled( true );
	}
}

#include "recipeinputdialog.moc"
