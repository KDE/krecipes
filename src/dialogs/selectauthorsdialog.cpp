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

#include "selectauthorsdialog.h"

#include <qmessagebox.h>
#include <qhbox.h>

#include <kconfig.h>
#include <kdialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kglobal.h>

#include "DBBackend/recipedb.h"

SelectAuthorsDialog::SelectAuthorsDialog( QWidget *parent, const ElementList &currentAuthors, RecipeDB *db ) : QDialog( parent, 0, true )
{


	database = db;

	//Design UI

	layout = new QGridLayout( this, 1, 1, KDialog::marginHint(), KDialog::spacingHint() );

	// Combo to Pick authors

	authorsCombo = new KComboBox( true, this );
	layout->addWidget( authorsCombo, 0, 0 );
	authorsCombo->completionObject() ->setCompletionMode( KGlobalSettings::CompletionPopupAuto );
	authorsCombo->lineEdit() ->disconnect( authorsCombo ); //so hitting enter doesn't enter the item into the box
	
	// Add/Remove buttons

	il = new KIconLoader;
	addAuthorButton = new QPushButton( this );
	QPixmap pm = il->loadIcon( "down", KIcon::NoGroup, 16 );
	addAuthorButton->setIconSet( pm );
	layout->addWidget( addAuthorButton, 0, 1 );
	removeAuthorButton = new QPushButton( this );
	pm = il->loadIcon( "up", KIcon::NoGroup, 16 );
	removeAuthorButton->setIconSet( pm );
	layout->addWidget( removeAuthorButton, 0, 2 );


	// Author List

	authorListView = new KListView( this );
	authorListView->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );

	KConfig * config = KGlobal::config();
	config->setGroup( "Advanced" );
	bool show_id = config->readBoolEntry( "ShowID", false );
	authorListView->addColumn( i18n( "Id" ), show_id ? -1 : 0 );
	authorListView->addColumn( i18n( "Author" ) );
	authorListView->setAllColumnsShowFocus( true );
	layout->addMultiCellWidget( authorListView, 1, 5, 0, 2 );
	layout->setColStretch( 1, 1 ); //give the column with the list view added space when the dialog grows


	//Ok/Cancel buttons
	QSpacerItem* buttonSpacer = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout->addItem( buttonSpacer, 6, 0 );

	QHBox *okCancelButtonBox = new QHBox( this );
	okCancelButtonBox->setSpacing( 10 );
	layout->addMultiCellWidget( okCancelButtonBox, 7, 7, 0, 2 );
	layout->setRowStretch( 5, 1 ); //this will expand only the list view when the dialog grows vertically

	okButton = new QPushButton( okCancelButtonBox );
	okButton->setText( i18n( "&OK" ) );
	okButton->setFlat( true );


	QSpacerItem* spacerBetweenButtons = new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum );
	layout->addItem( spacerBetweenButtons, 5, 1 );

	cancelButton = new QPushButton( okCancelButtonBox );
	cancelButton->setText( i18n( "&Cancel" ) );
	cancelButton->setFlat( true );

	// Load the list
	loadAuthors( currentAuthors );

	adjustSize();
	resize(450, height());

	// Connect signals & Slots
	connect ( okButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect ( cancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
	connect ( addAuthorButton, SIGNAL( clicked() ), this, SLOT( addAuthor() ) );
	connect ( removeAuthorButton, SIGNAL( clicked() ), this, SLOT( removeAuthor() ) );
}

SelectAuthorsDialog::~SelectAuthorsDialog()
{}

void SelectAuthorsDialog::getSelectedAuthors( ElementList *newAuthors )
{

	for ( QListViewItem * it = authorListView->firstChild();it; it = it->nextSibling() ) {
		Element author;
		author.id = it->text( 0 ).toInt();
		author.name = it->text( 1 );
		newAuthors->append( author );
	}

}

void SelectAuthorsDialog::loadAuthors( const ElementList &currentAuthors )
{

	// Load the combo
	reloadAuthorsCombo();

	// Load the ListView with the authors of this recipe
	authorListView->clear();
	for ( ElementList::const_iterator author_it = currentAuthors.begin(); author_it != currentAuthors.end(); ++author_it ) {
		( void ) new QListViewItem( authorListView, QString::number( ( *author_it ).id ), ( *author_it ).name );
	}

}

void SelectAuthorsDialog::addAuthor( void )
{
	//check bounds first
	if ( authorsCombo->currentText().length() > database->maxAuthorNameLength() ) {
		KMessageBox::error( this, QString( i18n( "Author name cannot be longer than %1 characters." ) ).arg( database->maxAuthorNameLength() ) );
		authorsCombo->lineEdit() ->selectAll();
		return ;
	}

	if ( authorsCombo->contains( authorsCombo->currentText() ) )
		authorsCombo->setCurrentItem( authorsCombo->currentText() );

	createNewAuthorIfNecessary();

	int currentItem = authorsCombo->currentItem();
	Element currentElement = authorList.getElement( currentItem );

	( void ) new QListViewItem( authorListView, QString::number( currentElement.id ), currentElement.name );

}

void SelectAuthorsDialog::removeAuthor( void )
{
	// Find the selected item first
	QListViewItem * it;
	it = authorListView->selectedItem();

	if ( it ) {  // Check if an author is selected first
		delete it;
	}

}

void SelectAuthorsDialog::createNewAuthorIfNecessary( void )
{

	if ( !authorsCombo->contains( authorsCombo->currentText() ) &&
	        !( authorsCombo->currentText().stripWhiteSpace() ).isEmpty() )  // author is not in the list and is not empty
	{ // Create new author
		QString newAuthorName = authorsCombo->currentText();
		database->createNewAuthor( newAuthorName );
		//List again the authors
		reloadAuthorsCombo();

		// Select the newly created author
		authorsCombo->setCurrentItem( newAuthorName );
		QMessageBox::information( this,
		                          i18n( "New author created" ),
		                          QString( i18n( "A new author \"%1\" was successfully added to the list of authors" ) ).arg( newAuthorName ),
		                          QMessageBox::Ok
		                        );
	}
}


void SelectAuthorsDialog::reloadAuthorsCombo( void )
{

	//Load the author list
	database->loadAuthors( &authorList );

	// Load combo with all the authors
	authorsCombo->clear();
	authorsCombo->completionObject() ->clear();

	for ( ElementList::const_iterator author_it = authorList.begin(); author_it != authorList.end(); ++author_it ) {
		authorsCombo->insertItem( ( *author_it ).name );
		authorsCombo->completionObject() ->addItem( ( *author_it ).name );
	}

}


#include "selectauthorsdialog.moc"
