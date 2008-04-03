/***************************************************************************
*   Copyright (C) 2004 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "prepmethodlistview.h"

#include <kmessagebox.h>
#include <kconfig.h>
#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kmenu.h>

#include "backends/recipedb.h"
#include "dialogs/createelementdialog.h"
#include "dialogs/dependanciesdialog.h"

PrepMethodListView::PrepMethodListView( QWidget *parent, RecipeDB *db ) : DBListViewBase( parent,db,db->prepMethodCount())
{
	setAllColumnsShowFocus( true );
	setDefaultRenameAction( Q3ListView::Reject );
}

void PrepMethodListView::init()
{
	connect( database, SIGNAL( prepMethodCreated( const Element & ) ), SLOT( checkCreatePrepMethod( const Element & ) ) );
	connect( database, SIGNAL( prepMethodRemoved( int ) ), SLOT( removePrepMethod( int ) ) );
}

void PrepMethodListView::load( int limit, int offset )
{
	ElementList prepMethodList;
	database->loadPrepMethods( &prepMethodList, limit, offset );

	setTotalItems(prepMethodList.count());

	for ( ElementList::const_iterator ing_it = prepMethodList.begin(); ing_it != prepMethodList.end(); ++ing_it )
		createPrepMethod( *ing_it );
}

void PrepMethodListView::checkCreatePrepMethod( const Element &el )
{
	if ( handleElement(el.name) ) { //only create this prep method if the base class okays it
		createPrepMethod(el);
	}
}


StdPrepMethodListView::StdPrepMethodListView( QWidget *parent, RecipeDB *db, bool editable ) : PrepMethodListView( parent, db )
{
	addColumn( i18n( "Preparation Method" ) );

	KConfigGroup config = KGlobal::config()->group( "Advanced" );
	bool show_id = config.readEntry( "ShowID", false );
	addColumn( i18n( "Id" ), show_id ? -1 : 0 );

	if ( editable ) {
		setRenameable( 0, true );

		KIconLoader il;

		kpop = new KMenu( this );
		kpop->addAction( il.loadIcon( "document-new", KIconLoader::NoGroup, 16 ), i18n( "&Create" ), this, SLOT( createNew() ), Qt::CTRL + Qt::Key_C );
		kpop->addAction( il.loadIcon( "edit-delete", KIconLoader::NoGroup, 16 ), i18n( "&Delete" ), this, SLOT( remove
			                  () ), Qt::Key_Delete );
		kpop->addAction( il.loadIcon( "edit", KIconLoader::NoGroup, 16 ), i18n( "&Rename" ), this, SLOT( slotRename() ), Qt::CTRL + Qt::Key_R );
		kpop->ensurePolished();

		connect( this, SIGNAL( contextMenu( K3ListView *, Q3ListViewItem *, const QPoint & ) ), SLOT( showPopup( K3ListView *, Q3ListViewItem *, const QPoint & ) ) );
		connect( this, SIGNAL( doubleClicked( Q3ListViewItem* ) ), this, SLOT( modPrepMethod( Q3ListViewItem* ) ) );
		connect( this, SIGNAL( itemRenamed( Q3ListViewItem* ) ), this, SLOT( savePrepMethod( Q3ListViewItem* ) ) );
	}
}

void StdPrepMethodListView::showPopup( K3ListView * /*l*/, Q3ListViewItem *i, const QPoint &p )
{
	if ( i )
		kpop->exec( p );
}

void StdPrepMethodListView::createNew()
{
	CreateElementDialog * elementDialog = new CreateElementDialog( this, i18n( "New Preparation Method" ) );

	if ( elementDialog->exec() == QDialog::Accepted ) {
		QString result = elementDialog->newElementName();

		//check bounds first
		if ( checkBounds( result ) )
			database->createNewPrepMethod( result ); // Create the new prepMethod in the database
	}
}

void StdPrepMethodListView::remove
	()
{
	Q3ListViewItem * item = currentItem();

	if ( item ) {
		ElementList dependingRecipes;
		int prepMethodID = item->text( 1 ).toInt();
		database->findPrepMethodDependancies( prepMethodID, &dependingRecipes );
		if ( dependingRecipes.isEmpty() )
			database->removePrepMethod( prepMethodID );
		else // Need Warning!
		{
			ListInfo info;
			info.list = dependingRecipes;
			info.name = i18n("Recipes");
			DependanciesDialog warnDialog( this, info );
			warnDialog.setCustomWarning( i18n("You are about to permanantly delete recipes from your database.") );
			if ( warnDialog.exec() == QDialog::Accepted )
				database->removePrepMethod( prepMethodID );
		}
	}
}

void StdPrepMethodListView::slotRename()
{
    rename( 0, 0 );
}

void StdPrepMethodListView::rename( Q3ListViewItem* /*item*/,int /*c*/ )
{
	Q3ListViewItem * item = currentItem();

	if ( item )
		PrepMethodListView::rename( item, 0 );
}

void StdPrepMethodListView::createPrepMethod( const Element &ing )
{
	createElement(new Q3ListViewItem( this, ing.name, QString::number( ing.id ) ));
}

void StdPrepMethodListView::removePrepMethod( int id )
{
	Q3ListViewItem * item = findItem( QString::number( id ), 1 );
	removeElement(item);
}

void StdPrepMethodListView::modPrepMethod( Q3ListViewItem* i )
{
	if ( i )
		PrepMethodListView::rename( i, 0 );
}

void StdPrepMethodListView::savePrepMethod( Q3ListViewItem* i )
{
	if ( !checkBounds( i->text( 0 ) ) ) {
		reload(ForceReload); //reset the changed text
		return ;
	}

	int existing_id = database->findExistingPrepByName( i->text( 0 ) );
	int prep_id = i->text( 1 ).toInt();
	if ( existing_id != -1 && existing_id != prep_id )  //category already exists with this label... merge the two
	{
		switch ( KMessageBox::warningContinueCancel( this, i18n( "This preparation method already exists.  Continuing will merge these two into one.  Are you sure?" ) ) )
		{
		case KMessageBox::Continue: {
				database->mergePrepMethods( existing_id, prep_id );
				break;
			}
		default:
			reload(ForceReload);
			break;
		}
	}
	else {
		database->modPrepMethod( ( i->text( 1 ) ).toInt(), i->text( 0 ) );
	}
}

bool StdPrepMethodListView::checkBounds( const QString &name )
{
	if ( name.length() > int(database->maxPrepMethodNameLength()) ) {
		KMessageBox::error( this, i18n( "Preparation method cannot be longer than %1 characters." , database->maxPrepMethodNameLength() ) );
		return false;
	}

	return true;
}

#include "prepmethodlistview.moc"
