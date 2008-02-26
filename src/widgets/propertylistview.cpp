/***************************************************************************
*   Copyright (C) 2004 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "propertylistview.h"

#include <klocale.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kmenu.h>
#include <kdebug.h>

#include "backends/recipedb.h"
#include "dialogs/createpropertydialog.h"

PropertyCheckListItem::PropertyCheckListItem( Q3ListView* klv, const IngredientProperty &property ) : Q3CheckListItem( klv, QString::null, Q3CheckListItem::CheckBox ),
		m_property( property )
{
	//setOn( false ); // Set unchecked by default
}

PropertyCheckListItem::PropertyCheckListItem( Q3ListViewItem* it, const IngredientProperty &property ) : Q3CheckListItem( it, QString::null, Q3CheckListItem::CheckBox ),
		m_property( property )
{
	//setOn( false ); // Set unchecked by default
}

QString PropertyCheckListItem::text( int column ) const
{
	switch ( column ) {
	case 0:
		return m_property.name;
		break;
	case 1:
		return m_property.units;
		break;
	case 2:
		return QString::number( m_property.id );
		break;

	}

	return QString::null;
}


HidePropertyCheckListItem::HidePropertyCheckListItem( Q3ListView* klv, const IngredientProperty &property, bool enable ) : PropertyCheckListItem( klv, property )
{
	m_holdSettings = true;
	setOn( enable ); // Set checked by default
	m_holdSettings = false;
}

HidePropertyCheckListItem::HidePropertyCheckListItem( Q3ListViewItem* it, const IngredientProperty &property, bool enable ) : PropertyCheckListItem( it, property )
{
	m_holdSettings = true;
	setOn( enable ); // Set checked by default
	m_holdSettings = false;
}

void HidePropertyCheckListItem::stateChange( bool on )
{
	if ( !m_holdSettings ) {
		KConfigGroup config = KGlobal::config()->group("Formatting");
	
		config.sync();
		QStringList hiddenList = config.readEntry("HiddenProperties", QStringList());
		if ( on )
			hiddenList.removeAll(m_property.name);
		else if ( !hiddenList.contains(m_property.name) )
			hiddenList.append(m_property.name);
	
		config.writeEntry("HiddenProperties",hiddenList);
	}
}

PropertyListView::PropertyListView( QWidget *parent, RecipeDB *db ) : K3ListView( parent ),
		database( db )
{
	setAllColumnsShowFocus( true );
	setDefaultRenameAction( Q3ListView::Reject );

	connect( db, SIGNAL( propertyCreated( const IngredientProperty & ) ), SLOT( createProperty( const IngredientProperty & ) ) );	
	connect( db, SIGNAL( propertyRemoved( int ) ), SLOT( removeProperty( int ) ) );
}

void PropertyListView::reload()
{
	clear(); // Clear the view

	m_loading = true;

	IngredientPropertyList propertyList;
	database->loadProperties( &propertyList );

	//Populate this data into the K3ListView
	IngredientPropertyList::const_iterator prop_it;
	for ( prop_it = propertyList.begin(); prop_it != propertyList.end(); ++prop_it )
		createProperty( *prop_it );

	m_loading = false;
}



StdPropertyListView::StdPropertyListView( QWidget *parent, RecipeDB *db, bool editable ) : PropertyListView( parent, db )
{
	addColumn( i18n( "Property" ) );
	addColumn( i18n( "Units" ) );

	KConfigGroup config = KGlobal::config()->group( "Advanced" );
	bool show_id = config.readEntry( "ShowID", false );
	addColumn( i18n( "Id" ), show_id ? -1 : 2 );

	setSorting( 0 );

	if ( editable ) {
		setRenameable( 0, true );

		KIconLoader *il = KIconLoader::global();

		kpop = new KMenu( this );
		kpop->insertItem( il->loadIcon( "document-new", KIconLoader::NoGroup, 16 ), i18n( "&Create" ), this, SLOT( createNew() ), Qt::CTRL + Qt::Key_C );
		kpop->insertItem( il->loadIcon( "edit-delete", KIconLoader::NoGroup, 16 ), i18n( "&Delete" ), this, SLOT( remove
			                  () ), Qt::Key_Delete );
		kpop->insertItem( il->loadIcon( "edit", KIconLoader::NoGroup, 16 ), i18n( "&Rename" ), this, SLOT( rename() ), Qt::CTRL + Qt::Key_R );
		kpop->polish();

		connect( this, SIGNAL( contextMenu( K3ListView *, Q3ListViewItem *, const QPoint & ) ), SLOT( showPopup( K3ListView *, Q3ListViewItem *, const QPoint & ) ) );
		connect( this, SIGNAL( doubleClicked( Q3ListViewItem* ) ), this, SLOT( modProperty( Q3ListViewItem* ) ) );
		connect( this, SIGNAL( itemRenamed( Q3ListViewItem* ) ), this, SLOT( saveProperty( Q3ListViewItem* ) ) );
	}
}

void StdPropertyListView::showPopup( K3ListView * /*l*/, Q3ListViewItem *i, const QPoint &p )
{
	if ( i )
		kpop->exec( p );
}

void StdPropertyListView::createNew()
{
	UnitList list;
	database->loadUnits( &list );
	CreatePropertyDialog* propertyDialog = new CreatePropertyDialog( this, &list );

	if ( propertyDialog->exec() == QDialog::Accepted ) {
		QString name = propertyDialog->newPropertyName();
		QString units = propertyDialog->newUnitsName();
		if ( !( ( name.isEmpty() ) || ( units.isEmpty() ) ) )  // Make sure none of the fields are empty
		{
			//check bounds first
			if ( checkBounds( name ) )
				database->addProperty( name, units );
		}
	}
	delete propertyDialog;
}

void StdPropertyListView::remove
	()
{
	Q3ListViewItem * item = currentItem();

	if ( item ) {
		switch ( KMessageBox::warningContinueCancel( this, i18n( "Are you sure you want to delete this property?" ) ) ) {
		case KMessageBox::Continue:
			database->removeProperty( item->text( 2 ).toInt() );
			break;
		default:
			break;
		}
	}
}

void StdPropertyListView::rename()
{
	Q3ListViewItem * item = currentItem();

	if ( item )
		PropertyListView::rename( item, 0 );
}

void StdPropertyListView::removeProperty( int id )
{
	Q3ListViewItem * item = findItem( QString::number( id ), 2 );

	Q_ASSERT( item );

	delete item;
}

void StdPropertyListView::createProperty( const IngredientProperty &property )
{
	( void ) new Q3ListViewItem( this, property.name, property.units, QString::number( property.id ) );
}

void StdPropertyListView::modProperty( Q3ListViewItem* i )
{
	if ( i )
		PropertyListView::rename( i, 0 );
}

void StdPropertyListView::saveProperty( Q3ListViewItem* i )
{
	if ( !checkBounds( i->text( 0 ) ) ) {
		reload(); //reset the changed text
		return ;
	}
kDebug() << "saveProp: " << i->text( 0 ) << endl;
	int existing_id = database->findExistingPropertyByName( i->text( 0 ) );
	int prop_id = i->text( 2 ).toInt();
	if ( existing_id != -1 && existing_id != prop_id )  //category already exists with this label... merge the two
	{
		switch ( KMessageBox::warningContinueCancel( this, i18n( "This property already exists.  Continuing will merge these two properties into one.  Are you sure?" ) ) )
		{
		case KMessageBox::Continue: {
				database->mergeProperties( existing_id, prop_id );
				break;
			}
		default:
			reload();
			break;
		}
	}
	else
		database->modProperty( prop_id, i->text( 0 ) );
}

bool StdPropertyListView::checkBounds( const QString &name )
{
	if ( name.length() > (uint)database->maxPropertyNameLength() ) {
		KMessageBox::error( this, i18n( "Property name cannot be longer than %1 characters." , database->maxPropertyNameLength() ) );
		return false;
	}

	return true;
}



PropertyConstraintListView::PropertyConstraintListView( QWidget *parent, RecipeDB *db ) : PropertyListView( parent, db )
{
	addColumn( i18n( "Enabled" ) );
	addColumn( i18n( "Property" ) );
	addColumn( i18n( "Min. Value" ) );
	addColumn( i18n( "Max. Value" ) );
	addColumn( "Id", 0 ); //hidden, only for internal purposes

	setRenameable( 0, true );
}

void PropertyConstraintListView::removeProperty( int id )
{
	Q3ListViewItem * item = findItem( QString::number( id ), 4 );

	Q_ASSERT( item );

	delete item;
}

void PropertyConstraintListView::createProperty( const IngredientProperty &property )
{
	( void ) new ConstraintsListItem( this, property );
}


CheckPropertyListView::CheckPropertyListView( QWidget *parent, RecipeDB *db, bool editable ) : StdPropertyListView( parent, db, editable )
{
}

void CheckPropertyListView::createProperty( const IngredientProperty &property )
{
	( void ) new HidePropertyCheckListItem( this, property, (m_loading)?false:true );
}

#include "propertylistview.moc"
