/***************************************************************************
*   Copyright (C) 2004 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*                                                                         *
*   Copyright (C) 2006 Jason Kivlighn (jkivlighn@gmail.com)               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "unitlistview.h"

#include <qcombobox.h>
#include <qheader.h>

#include <kmessagebox.h>
#include <kconfig.h>
#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kpopupmenu.h>
#include <kdebug.h>

#include "backends/recipedb.h"
#include "dialogs/createunitdialog.h"
#include "dialogs/dependanciesdialog.h"
#include "datablocks/unit.h"

class UnitListViewItem : public QListViewItem
{
public:
	UnitListViewItem( QListView* qlv, const Unit &u ) : QListViewItem( qlv ), m_unit(u)
	{
		updateType(m_unit.type);
	}

	virtual QString text( int column ) const
	{
		switch ( column ) {
		case 0: return m_unit.name;
		case 1: return m_unit.name_abbrev;
		case 2: return m_unit.plural;
		case 3: return m_unit.plural_abbrev;
		case 4: return m_type;
		case 5: return QString::number(m_unit.id);
		default: return QString::null;
		}
	}

	void setType( Unit::Type type ){ m_unit.type = type; updateType(type); }

	Unit unit() const { return m_unit; };
	void setUnit( const Unit &u ) { m_unit = u; }

protected:
	virtual void setText( int column, const QString &text ) {
		switch ( column ) {
		case 0: m_unit.name = text; break;
		case 1: m_unit.name_abbrev = text; break;
		case 2: m_unit.plural = text; break;
		case 3: m_unit.plural_abbrev = text; break;
		}
	}

private:
	void updateType( Unit::Type t ) {
		switch ( t ) {
		case Unit::Other: m_type = i18n("Other"); break;
		case Unit::Mass: m_type = i18n("Mass"); break;
		case Unit::Volume: m_type = i18n("Volume"); break;
		default: break;
		}
	}

	Unit m_unit;
	QString m_type;
};

UnitListView::UnitListView( QWidget *parent, RecipeDB *db ) : DBListViewBase( parent,db,db->unitCount() )
{
	setAllColumnsShowFocus( true );
	setDefaultRenameAction( QListView::Reject );
}

void UnitListView::init()
{
	connect( database, SIGNAL( unitCreated( const Unit & ) ), SLOT( checkCreateUnit( const Unit & ) ) );
	connect( database, SIGNAL( unitRemoved( int ) ), SLOT( removeUnit( int ) ) );
}

void UnitListView::load( int limit, int offset )
{
	UnitList unitList;
	database->loadUnits( &unitList, Unit::All, limit, offset );

	for ( UnitList::const_iterator it = unitList.begin(); it != unitList.end(); ++it ) {
		if ( !( *it ).name.isEmpty() || !( *it ).plural.isEmpty() )
			createUnit( *it );
	}
}

void UnitListView::checkCreateUnit( const Unit &el )
{
	if ( handleElement(el.name) ) { //only create this unit if the base class okays it
		createUnit(el);
	}
}


StdUnitListView::StdUnitListView( QWidget *parent, RecipeDB *db, bool editable ) : UnitListView( parent, db )
{
	addColumn( i18n( "Unit" ) );
	addColumn( i18n( "Abbreviation" ) );
	addColumn( i18n( "Plural" ) );
	addColumn( i18n( "Abbreviation" ) );
	addColumn( i18n( "Type" ) );

	KConfig * config = KGlobal::config();
	config->setGroup( "Advanced" );
	bool show_id = config->readBoolEntry( "ShowID", false );
	addColumn( i18n( "Id" ), show_id ? -1 : 0 );

	if ( editable ) {
		setRenameable( 0, true );
		setRenameable( 1, true );
		setRenameable( 2, true );
		setRenameable( 3, true );
		setRenameable( 4, true );

		KIconLoader *il = KGlobal::iconLoader();

		kpop = new KPopupMenu( this );
		kpop->insertItem( il->loadIcon( "filenew", KIcon::NoGroup, 16 ), i18n( "&Create" ), this, SLOT( createNew() ), CTRL + Key_C );
		kpop->insertItem( il->loadIcon( "editdelete", KIcon::NoGroup, 16 ), i18n( "&Delete" ), this, SLOT( remove
			                  () ), Key_Delete );
		kpop->insertItem( il->loadIcon( "edit", KIcon::NoGroup, 16 ), i18n( "&Rename" ), this, SLOT( rename() ), CTRL + Key_R );
		kpop->polish();

		typeComboBox = new QComboBox( false, viewport() );
		typeComboBox->insertItem(i18n("Other"));
		typeComboBox->insertItem(i18n("Mass"));
		typeComboBox->insertItem(i18n("Volume"));
		addChild( typeComboBox );
		typeComboBox->hide();

		connect( typeComboBox, SIGNAL( activated(int) ), SLOT( updateType(int) ) );
		connect( this, SIGNAL( selectionChanged() ), SLOT( hideTypeCombo() ) );

		connect( this, SIGNAL( contextMenu( KListView *, QListViewItem *, const QPoint & ) ), SLOT( showPopup( KListView *, QListViewItem *, const QPoint & ) ) );
		connect( this, SIGNAL( doubleClicked( QListViewItem*, const QPoint &, int ) ), this, SLOT( modUnit( QListViewItem*, const QPoint &, int ) ) );
		connect( this, SIGNAL( itemRenamed( QListViewItem*, const QString &, int ) ), this, SLOT( saveUnit( QListViewItem*, const QString &, int ) ) );
	}
}

void StdUnitListView::insertTypeComboBox( QListViewItem* it )
{
	QRect r;

	// Constraints Box1
	r = header() ->sectionRect( 4 ); //start at the section 2 header
	r.moveBy( 0, itemRect( it ).y() ); //Move down to the item, note that its height is same as header's right now.

	r.setHeight( it->height() ); // Set the item's height
	r.setWidth( header() ->sectionRect( 4 ).width() ); // and width
	typeComboBox->setGeometry( r );

	UnitListViewItem *unit_it = (UnitListViewItem*)it;
	typeComboBox->setCurrentItem( unit_it->unit().type );

	typeComboBox->show();
}

void StdUnitListView::updateType( int type )
{
	UnitListViewItem *unit_it = (UnitListViewItem*)currentItem();
	unit_it->setType((Unit::Type)type);

	database->modUnit( unit_it->unit() );
}

void StdUnitListView::hideTypeCombo()
{
	typeComboBox->hide();
}

void StdUnitListView::showPopup( KListView * /*l*/, QListViewItem *i, const QPoint &p )
{
	if ( i )
		kpop->exec( p );
}

void StdUnitListView::createNew()
{
	CreateUnitDialog * unitDialog = new CreateUnitDialog( this );

	if ( unitDialog->exec() == QDialog::Accepted ) {
		Unit result = unitDialog->newUnit();

		//check bounds first
		if ( checkBounds( result )
		  && database->findExistingUnitByName( result.name ) == -1
		  && database->findExistingUnitByName( result.plural ) == -1
		) {
			database->createNewUnit( result );
		}
	}
	delete unitDialog;
}

void StdUnitListView::remove()
{
	// Find selected unit item
	UnitListViewItem* it = (UnitListViewItem*)currentItem();

	if ( it ) {
		int unitID = it->unit().id;

		ElementList recipeDependancies, propertyDependancies, weightDependancies;
		database->findUnitDependancies( unitID, &propertyDependancies, &recipeDependancies, &weightDependancies );

		QValueList<ListInfo> lists;
		if ( !recipeDependancies.isEmpty() ) {
			ListInfo info;
			info.list = recipeDependancies;
			info.name = i18n("Recipes");
			lists << info;
		}
		if ( !propertyDependancies.isEmpty() ) {
			ListInfo info;
			info.list = propertyDependancies;
			info.name = i18n("Properties");
			lists << info;
		}
		if ( !weightDependancies.isEmpty() ) {
			ListInfo info;
			info.list = weightDependancies;
			info.name = i18n("Ingredient Weights");
			lists << info;
		}

		if ( lists.isEmpty() )
			database->removeUnit( unitID );
		else { // need warning!
			DependanciesDialog warnDialog( this, lists );
			if ( !recipeDependancies.isEmpty() )
				warnDialog.setCustomWarning( i18n("You are about to permanantly delete recipes from your database.") );
			if ( warnDialog.exec() == QDialog::Accepted )
				database->removeUnit( unitID );
		}
	}
}

void StdUnitListView::rename()
{
	QListViewItem * item = currentItem();

	if ( item ) {
		CreateUnitDialog unitDialog( this, item->text(0), item->text(2), item->text(1), item->text(3), false );
		
		if ( unitDialog.exec() == QDialog::Accepted ) {
			UnitListViewItem *unit_item = (UnitListViewItem*)item;
			Unit origUnit = unit_item->unit();
			Unit newUnit = unitDialog.newUnit();

			//for each changed entry, save the change individually

			Unit unit = origUnit;

			if ( newUnit.name != origUnit.name ) {
				unit.name = newUnit.name;
				unit_item->setUnit( unit );
				saveUnit( unit_item, newUnit.name, 0 );

				//saveUnit will call database->modUnit which deletes the list item we were using
				unit_item = (UnitListViewItem*) findItem( QString::number(unit.id), 5 );
			}

			if ( newUnit.plural != origUnit.plural ) {
				unit.plural = newUnit.plural;
				unit_item->setUnit( unit );
				saveUnit( unit_item, newUnit.plural, 2 );
				unit_item = (UnitListViewItem*) findItem( QString::number(unit.id), 5 );
			}

			if ( !newUnit.name_abbrev.stripWhiteSpace().isEmpty() && newUnit.name_abbrev != origUnit.name_abbrev ) {
				unit.name_abbrev = newUnit.name_abbrev;
				unit_item->setUnit( unit );
				saveUnit( unit_item, newUnit.name_abbrev, 1 );
				unit_item = (UnitListViewItem*) findItem( QString::number(unit.id), 5 );
			}
			if ( !newUnit.plural_abbrev.stripWhiteSpace().isEmpty() && newUnit.plural_abbrev != origUnit.plural_abbrev ) {
				unit.plural_abbrev = newUnit.plural_abbrev;
				unit_item->setUnit( unit );
				saveUnit( unit_item, newUnit.plural_abbrev, 3 );
			}
		}
	}
}

void StdUnitListView::createUnit( const Unit &unit )
{
	createElement(new UnitListViewItem( this, unit ));
}

void StdUnitListView::removeUnit( int id )
{
	QListViewItem * item = findItem( QString::number( id ), 5 );
	removeElement(item);
}

void StdUnitListView::modUnit( QListViewItem* i, const QPoint & /*p*/, int c )
{
	if ( i ) {
		if ( c != 4 )
			UnitListView::rename( i, c );
		else {
			insertTypeComboBox(i);
		}
	}
}

void StdUnitListView::saveUnit( QListViewItem* i, const QString &text, int c )
{
	//skip abbreviations
	if ( c == 0 || c == 2 ) {
		if ( !checkBounds( Unit( text, text ) ) ) {
			reload(ForceReload); //reset the changed text
			return ;
		}
	}

	int existing_id = database->findExistingUnitByName( text );

	UnitListViewItem *unit_it = (UnitListViewItem*)i;
	int unit_id = unit_it->unit().id;
	if ( existing_id != -1 && existing_id != unit_id && !text.stripWhiteSpace().isEmpty() ) { //unit already exists with this label... merge the two
		switch ( KMessageBox::warningContinueCancel( this, i18n( "This unit already exists.  Continuing will merge these two units into one.  Are you sure?" ) ) ) {
		case KMessageBox::Continue: {
				database->modUnit( unit_it->unit() );
				database->mergeUnits( unit_id, existing_id );
				break;
			}
		default:
			reload(ForceReload);
			break;
		}
	}
	else {
		database->modUnit( unit_it->unit() );
	}
}

bool StdUnitListView::checkBounds( const Unit &unit )
{
	if ( unit.name.length() > uint(database->maxUnitNameLength()) || unit.plural.length() > uint(database->maxUnitNameLength()) ) {
		KMessageBox::error( this, QString( i18n( "Unit name cannot be longer than %1 characters." ) ).arg( database->maxUnitNameLength() ) );
		return false;
	}
	else if ( unit.name.stripWhiteSpace().isEmpty() || unit.plural.stripWhiteSpace().isEmpty() )
		return false;

	return true;
}

#include "unitlistview.moc"
