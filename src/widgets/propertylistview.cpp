/***************************************************************************
 *   Copyright (C) 2004 by                                                 *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
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
#include <kpopupmenu.h>

#include "DBBackend/recipedb.h"
#include "dialogs/createpropertydialog.h"

PropertyCheckListItem::PropertyCheckListItem(QListView* klv, const IngredientProperty &property ) : QCheckListItem(klv,QString::null,QCheckListItem::CheckBox),
	m_property(property)
{
	setOn(false); // Set unchecked by default
}

PropertyCheckListItem::PropertyCheckListItem(QListViewItem* it, const IngredientProperty &property ) : QCheckListItem(it,QString::null,QCheckListItem::CheckBox),
	m_property(property)
{
	setOn(false); // Set unchecked by default
}

QString PropertyCheckListItem::text(int column) const
{
	switch (column)
	{
	case 0: return QString::number(m_property.id); break;
	case 1: return m_property.name; break;
	case 2: return m_property.units; break;
	}
	
	return QString::null;
}

PropertyListView::PropertyListView( QWidget *parent, RecipeDB *db ) : KListView(parent),
  database(db)
{
	setAllColumnsShowFocus(true);
	setDefaultRenameAction(QListView::Reject);

	connect(db,SIGNAL(propertyCreated(const IngredientProperty &)),SLOT(createProperty(const IngredientProperty &)));
	connect(db,SIGNAL(propertyRemoved(int)),SLOT(removeProperty(int)));
}

void PropertyListView::reload()
{
	clear(); // Clear the view

	IngredientPropertyList propertyList;
	database->loadProperties(&propertyList);
	
	//Populate this data into the KListView
	for ( IngredientProperty *prop =propertyList.getFirst(); prop; prop =propertyList.getNext() )
		createProperty(*prop);
}



StdPropertyListView::StdPropertyListView( QWidget *parent, RecipeDB *db, bool editable ) : PropertyListView(parent,db)
{
	KConfig *config = KGlobal::config();
	config->setGroup( "Advanced" );
	bool show_id = config->readBoolEntry("ShowID",false);
	addColumn( i18n("Id"), show_id ? -1 : 0 );

	addColumn(i18n("Property"));
	addColumn(i18n("Units"));

	setSorting(1);

	if ( editable ) {
		setRenameable(1, true);

		KIconLoader *il = new KIconLoader;

		kpop = new KPopupMenu( this );
		kpop->insertItem( il->loadIcon("filenew", KIcon::NoGroup,16),i18n("&Create"), this, SLOT(createNew()), CTRL+Key_C );
		kpop->insertItem( il->loadIcon("editdelete", KIcon::NoGroup,16),i18n("Remove"), this, SLOT(remove()), Key_Delete );
		kpop->insertItem( il->loadIcon("edit", KIcon::NoGroup,16), i18n("&Rename"), this, SLOT(rename()), CTRL+Key_R );
		kpop->polish();

		delete il;
	
		connect(this,SIGNAL(contextMenu(KListView *, QListViewItem *, const QPoint &)), SLOT(showPopup(KListView *, QListViewItem *, const QPoint &)));
		connect(this,SIGNAL(doubleClicked( QListViewItem* )),this, SLOT(modProperty( QListViewItem* )));
		connect(this,SIGNAL(itemRenamed(QListViewItem*)),this, SLOT(saveProperty(QListViewItem*)));
	}
}

void StdPropertyListView::showPopup(KListView */*l*/, QListViewItem *i, const QPoint &p)
{
	if ( i )
		kpop->exec(p);
}

void StdPropertyListView::createNew()
{
	ElementList list;
	database->loadUnits(&list);
	CreatePropertyDialog* propertyDialog=new CreatePropertyDialog(this,&list);
	
	if ( propertyDialog->exec() == QDialog::Accepted ) {
		QString name = propertyDialog->newPropertyName();
		QString units= propertyDialog->newUnitsName();
		if (!((name.isNull()) || (units.isNull()))) // Make sure none of the fields are empty
		database->addProperty(name, units);
	}
	delete propertyDialog;
}

void StdPropertyListView::remove()
{
	QListViewItem *item = currentItem();

	if ( item )
	{
		switch (KMessageBox::warningContinueCancel(this,i18n("Are you sure you want to remove this property?")))
		{
		case KMessageBox::Continue: database->removeProperty(item->text(0).toInt()); break;
		default: break;
		}
	}
}

void StdPropertyListView::rename()
{
	QListViewItem *item = currentItem();
	
	if ( item )
		PropertyListView::rename( item, 1 );
}

void StdPropertyListView::removeProperty(int id)
{
	QListViewItem *item = findItem(QString::number(id),0);
	
	Q_ASSERT(item);
	
	delete item;
}

void StdPropertyListView::createProperty(const IngredientProperty &property)
{
	(void)new QListViewItem(this,QString::number(property.id),property.name,property.units);
}

void StdPropertyListView::modProperty(QListViewItem* i)
{
	if ( i )
		PropertyListView::rename(i, 1);
}

void StdPropertyListView::saveProperty(QListViewItem* i)
{
	int existing_id = database->findExistingPropertyByName( i->text(1) );
	int prop_id = i->text(0).toInt();
	if ( existing_id != -1 && existing_id != prop_id ) //category already exists with this label... merge the two
	{  
		switch (KMessageBox::warningContinueCancel(this,i18n("This property already exists.  Continuing will merge these two properties into one.  Are you sure?")))
		{
		case KMessageBox::Continue:
		{
			database->mergeProperties(existing_id,prop_id);
			break;
		}
		default: reload(); break;
		}
	}
	else
		database->modProperty(prop_id, i->text(1));
}



PropertyConstraintListView::PropertyConstraintListView( QWidget *parent, RecipeDB *db ) : PropertyListView(parent,db)
{
	addColumn(i18n("Enabled"));
	addColumn(i18n("Property"));
	addColumn(i18n("Min. Value"));
	addColumn(i18n("Max. Value"));
	addColumn("Id",0); //hidden, only for internal purposes

	setRenameable(0, true);
}

void PropertyConstraintListView::removeProperty(int id)
{
	QListViewItem *item = findItem(QString::number(id),4);
	
	Q_ASSERT(item);
	
	delete item;
}

void PropertyConstraintListView::createProperty(const IngredientProperty &property)
{
	(void)new ConstraintsListItem(this,property);
}

#include "propertylistview.moc"
