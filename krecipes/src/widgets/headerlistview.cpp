/***************************************************************************
*   Copyright (C) 2004 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "headerlistview.h"

#include <kmessagebox.h>
#include <kconfig.h>
#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kmenu.h>

#include "backends/recipedb.h"
#include "dialogs/createelementdialog.h"
#include "dialogs/dependanciesdialog.h"

HeaderListView::HeaderListView( QWidget *parent, RecipeDB *db ) : DBListViewBase( parent,db,db->unitCount() )
{
	setAllColumnsShowFocus( true );
	setDefaultRenameAction( Q3ListView::Reject );
}

void HeaderListView::init()
{
	connect( database, SIGNAL( ingGroupCreated( const Element & ) ), SLOT( checkCreateHeader( const Element & ) ) );
	connect( database, SIGNAL( ingGroupRemoved( int ) ), SLOT( removeHeader( int ) ) );
}

void HeaderListView::load( int /*limit*/, int /*offset*/ )
{
	ElementList headerList;
	database->loadIngredientGroups( &headerList );

	setTotalItems(headerList.count());

	for ( ElementList::const_iterator it = headerList.begin(); it != headerList.end(); ++it ) {
		createHeader( *it );
	}
}

void HeaderListView::checkCreateHeader( const Element &el )
{
	if ( handleElement(el.name) ) { //only create this header if the base class okays it
		createHeader(el);
	}
}


StdHeaderListView::StdHeaderListView( QWidget *parent, RecipeDB *db, bool editable ) : HeaderListView( parent, db )
{
	addColumn( i18n( "Header" ) );

	KConfig * config = KGlobal::config();
	config->setGroup( "Advanced" );
	bool show_id = config->readBoolEntry( "ShowID", false );
	addColumn( i18n( "Id" ), show_id ? -1 : 0 );

	if ( editable ) {
		setRenameable( 0, true );

		KIconLoader *il = KIconLoader::global();

		kpop = new KMenu( this );
		kpop->insertItem( il->loadIcon( "document-new", KIconLoader::NoGroup, 16 ), i18n( "&Create" ), this, SLOT( createNew() ), CTRL + Key_C );
		kpop->insertItem( il->loadIcon( "edit-delete", KIconLoader::NoGroup, 16 ), i18n( "&Delete" ), this, SLOT( remove
			                  () ), Key_Delete );
		kpop->insertItem( il->loadIcon( "edit", KIconLoader::NoGroup, 16 ), i18n( "&Rename" ), this, SLOT( rename() ), CTRL + Key_R );
		kpop->polish();

		connect( this, SIGNAL( contextMenu( K3ListView *, Q3ListViewItem *, const QPoint & ) ), SLOT( showPopup( K3ListView *, Q3ListViewItem *, const QPoint & ) ) );
		connect( this, SIGNAL( doubleClicked( Q3ListViewItem*, const QPoint &, int ) ), this, SLOT( modHeader( Q3ListViewItem*, const QPoint &, int ) ) );
		connect( this, SIGNAL( itemRenamed( Q3ListViewItem*, const QString &, int ) ), this, SLOT( saveHeader( Q3ListViewItem*, const QString &, int ) ) );
	}
}

void StdHeaderListView::showPopup( K3ListView * /*l*/, Q3ListViewItem *i, const QPoint &p )
{
	if ( i )
		kpop->exec( p );
}

void StdHeaderListView::createNew()
{
	CreateElementDialog * headerDialog = new CreateElementDialog( this, i18n("Header") );

	if ( headerDialog->exec() == QDialog::Accepted ) {
		QString result = headerDialog->newElementName();

		//check bounds first
		if ( checkBounds( result ) )
			database->createNewIngGroup( result );
	}
	delete headerDialog;
}

void StdHeaderListView::remove()
{
	// Find selected header item
	Q3ListViewItem * it = selectedItem();

	if ( it ) {
		int headerID = it->text( 1 ).toInt();

		ElementList recipeDependancies;
		database->findUseOfIngGroupInRecipes( &recipeDependancies, headerID );

		if ( recipeDependancies.isEmpty() )
			database->removeIngredientGroup( headerID );
		else { // need warning!	
			ListInfo info;
			info.list = recipeDependancies;
			info.name = i18n( "Recipes" );

			DependanciesDialog warnDialog( this, info );
			warnDialog.setCustomWarning( i18n("You are about to permanantly delete recipes from your database.") );
			if ( warnDialog.exec() == QDialog::Accepted )
				database->removeIngredientGroup( headerID );
		}
	}
}

void StdHeaderListView::rename()
{
	Q3ListViewItem * item = currentItem();

	if ( item )
		HeaderListView::rename( item, 0 );
}

void StdHeaderListView::createHeader( const Element &header )
{
	createElement(new Q3ListViewItem( this, header.name, QString::number( header.id ) ));
}

void StdHeaderListView::removeHeader( int id )
{
	Q3ListViewItem * item = findItem( QString::number( id ), 1 );
	removeElement(item);
}

void StdHeaderListView::modHeader( Q3ListViewItem* i, const QPoint & /*p*/, int c )
{
	if ( i )
		HeaderListView::rename( i, c );
}

void StdHeaderListView::saveHeader( Q3ListViewItem* i, const QString &text, int /*c*/ )
{
	if ( !checkBounds( text ) ) {
		reload(ForceReload); //reset the changed text
		return ;
	}

	int existing_id = database->findExistingIngredientGroupByName( text );
	int header_id = i->text( 1 ).toInt();
	if ( existing_id != -1 && existing_id != header_id ) { //already exists with this label... merge the two
		switch ( KMessageBox::warningContinueCancel( this, i18n( "This header already exists.  Continuing will merge these two headers into one.  Are you sure?" ) ) ) {
		case KMessageBox::Continue: {
				database->modIngredientGroup( header_id, i->text( 0 ) );
				database->mergeIngredientGroups( header_id, existing_id );
				break;
			}
		default:
			reload(ForceReload);
			break;
		}
	}
	else {
		database->modIngredientGroup( header_id, i->text( 0 ) );
	}
}

bool StdHeaderListView::checkBounds( const QString &header )
{
	if ( header.length() > uint(database->maxIngGroupNameLength()) ) {
		KMessageBox::error( this, QString( i18n( "Header cannot be longer than %1 characters." ) ).arg( database->maxIngGroupNameLength() ) );
		return false;
	}
	else if ( header.trimmed().isEmpty() )
		return false;

	return true;
}

#include "headerlistview.moc"
