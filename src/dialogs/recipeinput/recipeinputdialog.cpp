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

	ingredientsTab = new QFrame;
	ingredientsTab->setFrameStyle( QFrame::NoFrame );
	ingredientsTab->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
	QGridLayout* ingredientsLayout = new QGridLayout( ingredientsTab );

	// Border
	QSpacerItem* spacerBoxLeft = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	ingredientsLayout->addItem( spacerBoxLeft, 1, 0 );

	// Spacers to list and buttons
	QSpacerItem* spacerToList = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	ingredientsLayout->addItem( spacerToList, 2, 1 );
	QSpacerItem* spacerToButtons = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	ingredientsLayout->addItem( spacerToButtons, 3, 4 );


	// Spacer to the rest of buttons
	QSpacerItem* spacerToOtherButtons = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	ingredientsLayout->addItem( spacerToOtherButtons, 4, 5 );

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
	//TODO: Remove this after removing the old editor code
	tabWidget->insertTab( -1, ingredientsTab, i18nc( "@title:tab", "Ingredients" ) );
	ingredientsEditor = new IngredientsEditor;
	ingredientsEditor->setDatabase( database );
	tabWidget->insertTab( -1, ingredientsEditor, i18nc( "@title:tab", "Ingredients" ) );
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

	connect( this, SIGNAL( changed() ), this, SLOT( recipeChanged() ) );
	connect( instructionsEdit, SIGNAL( textChanged() ), this, SLOT( recipeChanged() ) );
	connect( ingredientList, SIGNAL( itemRenamed( Q3ListViewItem*, const QString &, int ) ), SLOT( syncListView( Q3ListViewItem*, const QString &, int ) ) );

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

	ingredientsEditor->loadIngredientList( &loadedRecipe->ingList );

	instructionsEdit->setText( i18nc( "@label:textbox", "Write the recipe instructions here" ) );
	instructionsEdit->selectAll();

	ratingListEditor->clear();

	//Set back to the first page
	tabWidget->setCurrentIndex( 0 );

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

void RecipeInputDialog::reloadCheckSpelling()
{
	if ( instructionsEdit->checkSpellingEnabled() ) {                                                                                                              
		instructionsEdit->setCheckSpellingEnabled( false );
		instructionsEdit->setCheckSpellingEnabled( true );
	}
}

#include "recipeinputdialog.moc"
