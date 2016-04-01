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

#include "recipegeneralinfoeditor.h"
#include "ingredientseditor.h"
#include "ratinglisteditor.h"
#include "resizerecipedialog.h"

#include "widgets/kretextedit.h"

#include <KLocale>
#include <KMessageBox>
#include <KTabWidget>

#include <QToolButton>
#include <QHBoxLayout>

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

	ingredientsEditor = new IngredientsEditor;
	ingredientsEditor->setDatabase( database );

	//------- END OF Ingredients Tab ----------

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

	//Update instructions edit completion
	//TODO
	//IngredientList::const_iterator = loadedRecipe->ingList;
	//instructionsEdit->addCompletionItem( ( *ng_it->name );

	// Show ratings
	ratingListEditor->refresh();
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
	m_recipeGeneralInfoEditor->editCategoriesSlot();
}


void RecipeInputDialog::resizeRecipe( void )
{
	m_recipeGeneralInfoEditor->updateRecipe();
	QPointer<ResizeRecipeDialog> dlg = new ResizeRecipeDialog( this, loadedRecipe );

	if ( dlg->exec() == QDialog::Accepted )
		reload();
	
	delete dlg;
}

void RecipeInputDialog::reloadCheckSpelling()
{
	if ( instructionsEdit->checkSpellingEnabled() ) {                                                                                                              
		instructionsEdit->setCheckSpellingEnabled( false );
		instructionsEdit->setCheckSpellingEnabled( true );
	}
}

#include "recipeinputdialog.moc"
