/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                  *
*   Copyright © 2009-2015 José Manuel Santamaría Lema <panfaust@gmail.com> *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#include "krepropertyactionshandler.h"

#include <kdebug.h>

#include <KLocale>
//#include <K3ListView>
//#include <KMenu>
//#include <QPointer>
#include <KMessageBox>
//class Q3ListViewItem;
#include <QModelIndex>
#include <QStandardItemModel>

//#include "datablocks/elementlist.h"

#include "backends/recipedb.h"

#include "dialogs/createpropertydialog.h"
//#include "dialogs/dependanciesdialog.h"

#include "widgets/kregenericlistwidget.h"


KrePropertyActionsHandler::KrePropertyActionsHandler( KreGenericListWidget * listWidget, RecipeDB * db ):
	KreGenericActionsHandler( listWidget, db )
{
}

void KrePropertyActionsHandler::createNew()
{
	UnitList list;
	m_database->loadUnits( &list );
	QPointer<CreatePropertyDialog> propertyDialog = new CreatePropertyDialog( (QWidget*)m_listWidget, &list );

	if ( propertyDialog->exec() == QDialog::Accepted ) {
		QString name = propertyDialog->newPropertyName();
		QString units = propertyDialog->newUnitsName();
		if ( !( ( name.isEmpty() ) || ( units.isEmpty() ) ) )  // Make sure none of the fields are empty
		{
			//check bounds first
			if ( checkBounds( name ) )
				m_database->addProperty( name, units );
		}
	}
	delete propertyDialog;
}

void KrePropertyActionsHandler::remove()
{
	int prop_id = m_listWidget->selectedRowId();

	// If there is no property selected, do nothing.
	if ( prop_id == -1 ) {
		return;
	}

	// Remove the property after asking
	switch ( KMessageBox::warningContinueCancel( m_listWidget,
		i18n( "Are you sure you want to delete this property?" ) ) ) {
	case KMessageBox::Continue:
		m_database->removeProperty( prop_id );
		break;
	default:
		break;
	}
}


bool KrePropertyActionsHandler::checkBounds( const QString &name )
{
	if ( name.length() > m_database->maxPropertyNameLength() ) {
		KMessageBox::error( m_listWidget,
			i18np( "Property name cannot be longer than 1 character.",
			"Property name cannot be longer than %1 characters." ,
			m_database->maxPropertyNameLength() ) );
		return false;
	}
	return true;
}


void KrePropertyActionsHandler::saveElement( const QModelIndex & topLeft,
	const QModelIndex & bottomRight )
{
	//Not used parameters.
	Q_UNUSED( bottomRight )

	if ( topLeft.column() == 3 ) {
		KConfigGroup config = KGlobal::config()->group("Formatting");
	
		config.sync();
		QStringList hiddenList = config.readEntry("HiddenProperties", QStringList());
		QString propName = m_listWidget->getData( topLeft.row(), 1 ).toString();
		if ( m_listWidget->getItem( topLeft.row(), topLeft.column() )->checkState() == Qt::Checked ) {
			hiddenList.removeAll( propName );
		} else if ( !hiddenList.contains(propName) ) {
			hiddenList.append( propName );
		}
		kDebug() << hiddenList;

		config.writeEntry( "HiddenProperties", hiddenList );
	} else {	
		//Revert the changed text if the new name is longer than the maximum length. 
		QString newPropName = topLeft.data().toString();
		if ( !checkBounds(newPropName) ) {
			m_listWidget->reload( ForceReload ); //reset the changed text
			return ;
		}

		int existing_id = m_database->findExistingPropertyByName( newPropName );
		int prop_id = m_listWidget->selectedRowId();
		kDebug() << existing_id << prop_id;
		if ( (existing_id != -1) && (existing_id != prop_id) )  //already exists with this label... merge the two
		{
			switch ( KMessageBox::warningContinueCancel( m_listWidget,
			i18n( "This property already exists.  Continuing will merge these two properties into one.  Are you sure?" ) ) )
			{
			case KMessageBox::Continue: {
					m_database->mergeProperties( existing_id, prop_id );
					break;
				}
			default:
				m_listWidget->reload( ForceReload );
				break;
			}
		} else {
			m_database->modProperty( prop_id, newPropName );
		}
	}
}

