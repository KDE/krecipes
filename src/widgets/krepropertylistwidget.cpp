/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                  *
*   Copyright © 2015 José Manuel Santamaría Lema <pnafaust@gmail.com>      *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#include "krepropertylistwidget.h"

#include "ui_kregenericlistwidget.h"


#include <kdebug.h>

#include <KLocale>
//#include <kmessagebox.h>
#include <KConfigGroup>
#include <KGlobal>
//#include <kmenu.h>
//#include <kdebug.h>
//#include <QPointer>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

#include "widgets/propdisplayeddelegate.h"
#include "backends/recipedb.h"
//#include "dialogs/createpropertydialog.h"
#include "datablocks/ingredientpropertylist.h"


KrePropertyListWidget::KrePropertyListWidget( QWidget * parent, RecipeDB * db ):
	KreGenericListWidget( parent, db )
{
	//The horizontal column labels.
	QStringList horizontalLabels;
	horizontalLabels << i18nc( "@title:column", "Id" )
		<< i18nc( "@title:column", "Property" )
		<< i18nc( "@title:column", "Units" )
		<< i18nc( "@title:column", "Property Displayed?" );
        m_sourceModel->setHorizontalHeaderLabels( horizontalLabels );

	//Connect signals and slots.
	connect( m_database, SIGNAL( propertyCreated( const IngredientProperty & ) ),
		this, SLOT( createProperty( const IngredientProperty & ) ) );
	connect( m_database, SIGNAL( propertyRemoved( int ) ),
		this, SLOT( removeProperty( int ) ) );
}

int KrePropertyListWidget::elementCount()
{
	//TODO
	return 0;
}

void KrePropertyListWidget::load(int limit, int offset)
{
	//We are not using limits here because usually the number of properties
	//in the database will be low, even for large databases.
	Q_UNUSED(limit)
	Q_UNUSED(offset)

	IngredientPropertyList propList;
	int numberOfProps = m_database->loadProperties( &propList );
	m_sourceModel->setRowCount( numberOfProps );

	KConfigGroup config( KGlobal::config(), "Formatting");
	QStringList hiddenList = config.readEntry("HiddenProperties", QStringList());

	//PropDisplayedDelegate * propDisplayedDelegate = new PropDisplayedDelegate;
	//ui->m_treeView->setItemDelegateForColumn(5, propDisplayedDelegate);
	kDebug() << hiddenList;

	int current_row = 0;
	QModelIndex index;
	for ( IngredientPropertyList::const_iterator it = propList.constBegin(); it != propList.constEnd(); ++it ) {
		// Write the database id in the model.
		index = m_sourceModel->index( current_row, 0 );
		m_sourceModel->setData( index, QVariant(it->id), Qt::EditRole );
		m_sourceModel->itemFromIndex( index )->setEditable( false );
		// Write the name of the property in the model.
		index = m_sourceModel->index( current_row, 1 );
		m_sourceModel->setData( index, QVariant(it->name), Qt::EditRole );
		m_sourceModel->itemFromIndex( index )->setEditable( true );
		// Write the units the property is using in the model.
		index = m_sourceModel->index( current_row, 2 );
		m_sourceModel->setData( index, QVariant(it->units), Qt::EditRole );
		//FIXME: Property units cannot be edited, this is a bug the database design.
		m_sourceModel->itemFromIndex( index )->setEditable( false );
		// Item showing if the property is displayed in "show recipe" and printing
		QStandardItem * checkItem = new QStandardItem;
		checkItem->setCheckable( true );
		if ( hiddenList.contains(it->name) ) {
			checkItem->setCheckState( Qt::Unchecked );
		} else {
			checkItem->setCheckState( Qt::Checked );
		}
		m_sourceModel->setItem( current_row, 3, checkItem );
		// Increment the row counter.
		++current_row;
	}

	//For some reason the list isn't ordered after adding that QStandardItem's
	//to the model (see above).
	m_proxyModel->sort(1);

	emit loadFinishedPrivate();

}

void KrePropertyListWidget::cancelLoad()
{
	//We do nothing here because we are not using threads
}

int KrePropertyListWidget::idColumn()
{
 	//This function must return the column number where the id is stored.
	return 0;
}


void KrePropertyListWidget::createProperty( const IngredientProperty &property )
{
	//FIXME: This could be more efficient.
	Q_UNUSED( property )
	reload( ForceReload );
}

void KrePropertyListWidget::removeProperty( int id )
{
	//FIXME: This could be more efficient.
	Q_UNUSED( id )
	reload( ForceReload );
}

/*void HidePropertyCheckListItem::stateChange( bool on )
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
}*/

/*PropertyListView::PropertyListView( QWidget *parent, RecipeDB *db ) :
	DBListViewBase( parent, db, 0)
{
	setAllColumnsShowFocus( true );
	setDefaultRenameAction( Q3ListView::Reject );

	connect( db, SIGNAL( propertyCreated( const IngredientProperty & ) ), SLOT( createProperty( const IngredientProperty & ) ) );
	connect( db, SIGNAL( propertyRemoved( int ) ), SLOT( removeProperty( int ) ) );
}*/

/*void PropertyListView::reload()
{
	clear(); // Clear the view

	m_loading = true;

	IngredientPropertyList propertyList;
	database->loadProperties( &propertyList );

	//Populate this data into the K3ListView
	IngredientPropertyList::const_iterator prop_it;
	for ( prop_it = propertyList.constBegin(); prop_it != propertyList.constEnd(); ++prop_it )
		createProperty( *prop_it );

	m_loading = false;
}*/

/*void PropertyListView::load(int, int)
{
	reload();
}*/

/*void StdPropertyListView::removeProperty( int id )
{
	Q3ListViewItem * item = findItem( QString::number( id ), 2 );

	Q_ASSERT( item );

	delete item;
}*/

/*void StdPropertyListView::createProperty( const IngredientProperty &property )
{
	( void ) new Q3ListViewItem( this, property.name, property.units, QString::number( property.id ) );
}*/



/*PropertyConstraintListView::PropertyConstraintListView( QWidget *parent, RecipeDB *db ) : PropertyListView( parent, db )
{
	addColumn( i18nc( "@title:column", "Enabled" ) );
	addColumn( i18nc( "@title:column", "Property" ) );
	addColumn( i18nc( "@title:column", "Min. Value" ) );
	addColumn( i18nc( "@title:column", "Max. Value" ) );
	addColumn( "Id", 0 ); //hidden, only for internal purposes

	setRenameable( 0, true );
	setRenameable( 1, true );
}*/

/*void PropertyConstraintListView::removeProperty( int id )
{
	Q3ListViewItem * item = findItem( QString::number( id ), 4 );

	Q_ASSERT( item );

	delete item;
}*/

/*void PropertyConstraintListView::createProperty( const IngredientProperty &property )
{
	( void ) new ConstraintsListItem( this, property );
}*/


/*CheckPropertyListView::CheckPropertyListView( QWidget *parent, RecipeDB *db, bool editable ) : StdPropertyListView( parent, db, editable )
{
}*/

/*void CheckPropertyListView::createProperty( const IngredientProperty &property )
{
	( void ) new HidePropertyCheckListItem( this, property, (m_loading)?false:true );
}*/

