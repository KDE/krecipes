/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2012 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "selectauthorsdialog.h"

#include <kvbox.h>

#include <kconfig.h>
#include <kdialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kglobal.h>
#include <KPushButton>

#include <QLineEdit>
#include <QStandardItemModel>
#include <QTreeView>
#include <QHeaderView>
#include <QSortFilterProxyModel>

#include "backends/recipedb.h"

SelectAuthorsDialog::SelectAuthorsDialog( QWidget *parent, const ElementList &currentAuthors, RecipeDB *db )
		: KDialog(parent ),
		database(db)
{
	setCaption(i18nc("@title:window", "Authors" ));
	setButtons(KDialog::Ok | KDialog::Cancel);
	setDefaultButton(KDialog::Ok);
	setModal( true );
	KVBox *page = new KVBox( this );
	setMainWidget( page );
	//Design UI

	// Combo to Pick authors
	KHBox *topBox = new KHBox(page);
	topBox->setSpacing(6);

	authorsCombo = new KComboBox( true, topBox );
	authorsCombo->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
	authorsCombo->completionObject() ->setCompletionMode( KGlobalSettings::CompletionPopupAuto );
	authorsCombo->lineEdit() ->disconnect( authorsCombo ); //so hitting enter doesn't enter the item into the box

	connect( authorsCombo->lineEdit(), SIGNAL( returnPressed() ),
					 this, SLOT( addAuthor() ) );

	// Add/Remove buttons

	addAuthorButton = new KPushButton( topBox );
	addAuthorButton->setIcon( KIcon( "list-add" ) );

	removeAuthorButton = new KPushButton( topBox );
	removeAuthorButton->setIcon( KIcon( "list-remove" ) );

	// Author List

	authorListModel = new QStandardItemModel( 0, 2, this );

	authorListView = new QTreeView( page );
	authorListView->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
	authorListView->setAllColumnsShowFocus( true );
	authorListView->setRootIsDecorated( false );

	QStringList horizontalLabels;
	horizontalLabels << i18nc( "@title:column", "Id" ) << i18nc( "@title:column", "Author" );
	authorListModel->setHorizontalHeaderLabels( horizontalLabels );
		
	authorListProxyModel = new QSortFilterProxyModel(this);
	authorListProxyModel->setSourceModel(authorListModel);
	authorListProxyModel->setDynamicSortFilter( true );
	authorListView->setModel( authorListProxyModel );
	
	KConfigGroup config( KGlobal::config(), "Advanced" );
	if ( !config.readEntry( "ShowID", false ) ) {
		authorListView->hideColumn( 0 );
		authorListView->header()->hide();
	}

	// Load the list
	loadAuthors( currentAuthors );

	adjustSize();
	resize(450, height());

	// Connect signals & Slots
	connect ( addAuthorButton, SIGNAL( clicked() ), this, SLOT( addAuthor() ) );
	connect ( removeAuthorButton, SIGNAL( clicked() ), this, SLOT( removeAuthor() ) );

	authorsCombo->setEditText(QString());
	authorsCombo->lineEdit()->setFocus();
}

SelectAuthorsDialog::~SelectAuthorsDialog()
{}

void SelectAuthorsDialog::getSelectedAuthors( ElementList *newAuthors )
{
	int row_count = authorListModel->rowCount();
	for ( int i = 0; i < row_count; i++ ) {
		Element author;
		author.id = authorListModel->item(i, 0 )->text().toInt();
		author.name = authorListModel->item( i, 1 )->text();
		newAuthors->append( author );
	}
}

void SelectAuthorsDialog::loadAuthors( const ElementList &currentAuthors )
{

	// Load the combo
	reloadAuthorsCombo();

	// Load the ListView with the authors of this recipe
	//authorListModel->clear();
	for ( ElementList::const_iterator author_it = currentAuthors.begin(); author_it != currentAuthors.end(); ++author_it ) {
		QStandardItem *itemId = new QStandardItem;
		itemId->setData( QVariant(author_it->id), Qt::EditRole );
		itemId->setEditable( false ); 
		QStandardItem *itemAuthor = new QStandardItem( author_it->name );
		itemAuthor->setEditable( false );
		QList<QStandardItem*> items;
		items << itemId << itemAuthor;
		authorListModel->appendRow( items );
	}
	
	authorListView->setSortingEnabled( true );
	authorListView->sortByColumn( 1, Qt::AscendingOrder);
}

void SelectAuthorsDialog::addAuthor( void )
{
	//check bounds first
	if ( authorsCombo->currentText().length() > int(database->maxAuthorNameLength()) ) {
		KMessageBox::error( this, i18ncp( "@info", "Author name cannot be longer than 1 character.", "Author name cannot be longer than %1 characters." , database->maxAuthorNameLength() ) );
		authorsCombo->lineEdit() ->selectAll();
		return ;
	}

	if ( authorsCombo->lineEdit()->text().isEmpty() )
		return;

	if ( authorsCombo->contains( authorsCombo->currentText() ) )
		authorsCombo->setCurrentItem( authorsCombo->currentText() );

	createNewAuthorIfNecessary();

	int currentItem = authorsCombo->currentIndex();
	Element currentElement = authorList.getElement( currentItem );

	QStandardItem *itemId = new QStandardItem;
	itemId->setData( QVariant(currentElement.id), Qt::EditRole );
	itemId->setEditable( false ); 
	QStandardItem *itemAuthor = new QStandardItem( currentElement.name );
	itemAuthor->setEditable( false );
	QList<QStandardItem*> items;
	items << itemId << itemAuthor;
	authorListModel->appendRow( items );

	authorsCombo->lineEdit()->clear();
}

void SelectAuthorsDialog::removeAuthor( void )
{
	// Find the selected item first
	int row = authorListProxyModel->mapToSource(authorListView->currentIndex()).row();

	authorListModel->removeRows( row, 1 );
}

void SelectAuthorsDialog::createNewAuthorIfNecessary( void )
{

	if ( !authorsCombo->contains( authorsCombo->currentText() ) &&
		!( authorsCombo->currentText().trimmed() ).isEmpty() )  // author is not in the list and is not empty
	{ // Create new author
		QString newAuthorName = authorsCombo->currentText();
		database->createNewAuthor( newAuthorName );
		//List again the authors
		reloadAuthorsCombo();

		// Select the newly created author
		authorsCombo->setCurrentItem( newAuthorName );
	}
}


void SelectAuthorsDialog::reloadAuthorsCombo( void )
{

	//Load the author list
	database->loadAuthors( &authorList );

	// Load combo with all the authors
	authorsCombo->clear();
	authorsCombo->completionObject() ->clear();

	for ( ElementList::const_iterator author_it = authorList.constBegin(); author_it != authorList.constEnd(); ++author_it ) {
		authorsCombo->insertItem( authorsCombo->count(), ( *author_it ).name );
		authorsCombo->completionObject() ->addItem( ( *author_it ).name );
	}

}


#include "selectauthorsdialog.moc"
