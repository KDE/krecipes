/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2004 Unai Garro <ugarro@gmail.com>                        *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "categorylistview.h"

#include <klocale.h>
#include <kmessagebox.h>
#include <kiconloader.h>
#include <kmenu.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kdebug.h>
#include <QPointer>

#include "backends/recipedb.h"
#include "datablocks/categorytree.h"
#include "dialogs/createcategorydialog.h"
#include "dialogs/dependanciesdialog.h"
//Added by qt3to4:
#include <Q3ValueList>
#include <QPixmap>

CategoryCheckListItem::CategoryCheckListItem( CategoryCheckListView* klv, const Element &category, bool _exclusive ) : Q3CheckListItem( klv, QString(), Q3CheckListItem::CheckBox ), CategoryItemInfo( category ),
		locked( false ),
		exclusive( _exclusive ),
		m_listview(klv)
{
	setOn( false ); // Set unchecked by default
}

CategoryCheckListItem::CategoryCheckListItem( Q3ListViewItem* it, const Element &category, bool _exclusive ) : Q3CheckListItem( it, QString(), Q3CheckListItem::CheckBox ), CategoryItemInfo( category ),
		locked( false ),
		exclusive( _exclusive ),
		m_listview((CategoryCheckListView*)it->listView())
{
	setOn( false ); // Set unchecked by default
}

CategoryCheckListItem::CategoryCheckListItem( CategoryCheckListView* klv, Q3ListViewItem* it, const Element &category, bool _exclusive ) : Q3CheckListItem( klv, it, QString(), Q3CheckListItem::CheckBox ), CategoryItemInfo( category ),
		locked( false ),
		exclusive( _exclusive ),
		m_listview(klv)
{
	setOn( false ); // Set unchecked by default
}

QString CategoryCheckListItem::text( int column ) const
{
	if ( column == 1 )
		return ( QString::number( ctyStored.id ) );
	else
		return ( ctyStored.name );
}

void CategoryCheckListItem::setText( int column, const QString &text )
{
	switch ( column ) {
	case 0:
		ctyStored.name = text;
		break;
	default:
		break;
	}
}

void CategoryCheckListItem::stateChange( bool on )
{
	m_listview->stateChange(this,on);

	if ( locked )
		return;

	if ( on && exclusive ) {
		setParentsState( false );
		setChildrenState( false );
	}
}

void CategoryCheckListItem::setChildrenState( bool on )
{
	if ( !isPopulated() )
		return;

	for ( CategoryCheckListItem * cat_it = ( CategoryCheckListItem* ) firstChild(); cat_it; cat_it = ( CategoryCheckListItem* ) cat_it->nextSibling() ) {
		cat_it->locked = true;
		cat_it->setOn( on );
		cat_it->setChildrenState( on );
		cat_it->locked = false;
	}
}

void CategoryCheckListItem::setParentsState( bool on )
{
	locked = true;

	CategoryCheckListItem *cat_it;
	for ( cat_it = ( CategoryCheckListItem* ) parent(); cat_it; cat_it = ( CategoryCheckListItem* ) cat_it->parent() )
		cat_it->setOn( on );

	locked = false;
}




CategoryListItem::CategoryListItem( Q3ListView* klv, const Element &category ) : Q3ListViewItem( klv ),
	CategoryItemInfo(category)
{}

CategoryListItem::CategoryListItem( Q3ListViewItem* it, const Element &category ) : Q3ListViewItem( it ),
	CategoryItemInfo(category)
{}

CategoryListItem::CategoryListItem( Q3ListView* klv, Q3ListViewItem* it, const Element &category ) : Q3ListViewItem( klv, it ),
	CategoryItemInfo(category)
{}

QString CategoryListItem::text( int column ) const
{
	if ( column == 1 )
		return ( QString::number( ctyStored.id ) );
	else
		return ( ctyStored.name );
}

void CategoryListItem::setText( int column, const QString &text )
{
	if ( column == 0 )
		ctyStored.name = text;
}



CategoryListView::CategoryListView( QWidget *parent, RecipeDB *db ) : DBListViewBase( parent, db, db->categoryTopLevelCount() ),
	m_item_to_delete(0)
{
	//connect( this, SIGNAL( spacePressed(QListViewItem*) ), SLOT( open(QListViewItem*) ) );
	//connect( this, SIGNAL( returnPressed(QListViewItem*) ), SLOT( open(QListViewItem*) ) );
	//connect( this, SIGNAL( executed(QListViewItem*) ), SLOT( open(QListViewItem*) ) );

	connect( this, SIGNAL( expanded(Q3ListViewItem*) ), SLOT( open(Q3ListViewItem*) ) );

	setRootIsDecorated( true );
	setAllColumnsShowFocus( true );
	setDefaultRenameAction( Q3ListView::Reject );

	connect( database, SIGNAL( categoryCreated( const Element &, int ) ), SLOT( checkCreateCategory( const Element &, int ) ) );
	connect( database, SIGNAL( categoryRemoved( int ) ), SLOT( removeCategory( int ) ) );
	connect( database, SIGNAL( categoryModified( const Element & ) ), SLOT( modifyCategory( const Element & ) ) );
	connect( database, SIGNAL( categoryModified( int, int ) ), SLOT( modifyCategory( int, int ) ) );
	connect( database, SIGNAL( categoriesMerged( int, int ) ), SLOT( mergeCategories( int, int ) ) );
}

// (Re)loads the data from the database
void CategoryListView::load( int limit, int offset )
{
	items_map.clear();

	CategoryTree list;
	CategoryTree *p_list = &list;
	database->loadCachedCategories( &p_list, limit, offset, -1, false );

	setTotalItems(p_list->count());

	for ( CategoryTree * child_it = p_list->firstChild(); child_it; child_it = child_it->nextSibling() ) {
		createCategory( child_it->category, -1 );
	}
}

void CategoryListView::movableDropEvent( Q3ListViewItem * parent, Q3ListViewItem * afterme )
{
	if ( parent )
		if ( (parent->rtti() == PREVLISTITEM_RTTI) ||
		(parent->rtti() == NEXTLISTITEM_RTTI) )
			return;
	if ( (selectedItem()->rtti() == PREVLISTITEM_RTTI) ||
	(selectedItem()->rtti() == NEXTLISTITEM_RTTI) )
		return;
	K3ListView::movableDropEvent( parent, afterme );
}

void CategoryListView::populate( Q3ListViewItem *item )
{
	CategoryItemInfo *cat_item = dynamic_cast<CategoryItemInfo*>(item);
	if ( !cat_item || cat_item->isPopulated() ) return;

	if ( item->firstChild() && item->firstChild()->rtti() != PSEUDOLISTITEM_RTTI )
		return;

	delete item->firstChild(); //delete the "pseudo item"

	int id = cat_item->categoryId();
	cat_item->setPopulated(true);

	CategoryTree categoryTree;
	database->loadCategories( &categoryTree, -1, 0, id, false );

	for ( CategoryTree * child_it = categoryTree.firstChild(); child_it; child_it = child_it->nextSibling() ) {
		createCategory( child_it->category, id );
	}
}

void CategoryListView::populateAll( Q3ListViewItem *parent )
{
	if ( !parent )
		parent = firstChild();

	for ( Q3ListViewItem *item = parent; item; item = item->nextSibling() ) {
		populate( item );
		if ( item->firstChild() )
			populateAll( item->firstChild() );
	}
}

void CategoryListView::open( Q3ListViewItem *item )
{
	Q_ASSERT( item );
	if ( !item->firstChild() || item->firstChild()->rtti() != PSEUDOLISTITEM_RTTI ) return;

	populate(item);

	item->setOpen(true);
}

void CategoryListView::checkCreateCategory( const Element &el, int parent_id )
{
	if ( parent_id != -1 || handleElement(el.name) ) { //only create this category if the base class okays it; allow all non-top-level items
		createCategory(el,parent_id);
	}
}

void CategoryListView::modifyCategory( const Element &category )
{
	Q3ListViewItem * item = items_map[ category.id ];

	if ( item )
		item->setText( 0, category.name );
}

void CategoryListView::modifyCategory( int id, int parent_id )
{
	QMap<int,Q3ListViewItem*>::iterator item_it = items_map.find(id);
	if ( item_it != items_map.end() ) {
		Q3ListViewItem *item = *item_it;
		Q_ASSERT( item );

		removeElement(item,false);
		if ( !item->parent() )
			takeItem( item );
		else
			item->parent() ->takeItem( item );

		if ( parent_id == -1 ) {
			insertItem(item);
			createElement(item);
		}
		else {
			QMap<int,Q3ListViewItem*>::iterator parent_item_it = items_map.find(parent_id);
			if ( parent_item_it != items_map.end() &&
			dynamic_cast<CategoryItemInfo*>(*parent_item_it)->isPopulated() ) {
				(*parent_item_it)->insertItem( item );
				createElement(item);
			}
			else {
				if ( !(*parent_item_it)->firstChild() )
					new PseudoListItem( *parent_item_it );

				//removeElement() was already called on this item, so we just delete it
				//we can't delete it just yet because this function is called by a slot
				delete m_item_to_delete;
				m_item_to_delete = item;
			}
		}
	}
}

void CategoryListView::mergeCategories( int id1, int id2 )
{
	Q3ListViewItem * to_item = items_map[ id1 ];
	Q3ListViewItem *from_item = items_map[ id2 ];

	CategoryItemInfo *info_item = dynamic_cast<CategoryItemInfo*>(to_item);

	if ( to_item && info_item->isPopulated() && from_item ) {
		//note that this takes care of any recipes that may be children as well
		Q3ListViewItem *next_sibling;
		for ( Q3ListViewItem * it = from_item->firstChild(); it; it = next_sibling ) {
			next_sibling = it->nextSibling(); //get the sibling before we move the item

			removeElement(it,false);
			from_item->takeItem( it );

			to_item->insertItem( it );
			createElement(it);
		}
	}

	removeCategory( id2 );
}


StdCategoryListView::StdCategoryListView( QWidget *parent, RecipeDB *db, bool editable ) :
	CategoryListView( parent, db )
{
	addColumn( i18n( "Category" ) );

	//FIXME: for some reason when I have "ShowID=true" this code results in
	//krecipes eating 100% of my cpu time.
	//KConfigGroup config = KGlobal::config()->group( "Advanced" );
	//bool show_id = config.readEntry( "ShowID", false );
	//addColumn( "Id" , show_id ? -1 : 0 );
	addColumn( "Id" , 0 );

	if ( editable ) {
		setRenameable( 0, true );
		setDragEnabled( true );
		setAcceptDrops( true );
	}
}

void StdCategoryListView::setPixmap( const QPixmap &icon )
{
	m_folder_icon = icon;
}

void StdCategoryListView::removeCategory( int id )
{
	Q3ListViewItem * item = items_map[ id ];

	items_map.remove( id );
	removeElement(item);
}

void StdCategoryListView::createCategory( const Element &category, int parent_id )
{
	CategoryListItem * new_item = 0;
	if ( parent_id == -1 ) {
		new_item = new CategoryListItem( this, category );
	}
	else {
		CategoryListItem *parent = (CategoryListItem*)items_map[ parent_id ];

		if ( parent ) {
			if ( parent->isPopulated() )
				new_item = new CategoryListItem( parent, category );
			else if ( !parent->firstChild() ) {
				new PseudoListItem( parent );
				parent->setOpen(true);
			}
		}
	}

	if ( new_item ) {
		items_map.insert( category.id, new_item );
		new_item->setPixmap( 0, m_folder_icon );
		createElement(new_item);//new QListViewItem(new_item);

		CategoryTree list;
		CategoryTree *p_list = &list;
		database->loadCachedCategories( &p_list, 1, 0, category.id, false );

		if ( p_list->firstChild() )
			new PseudoListItem( new_item );
	}
}



CategoryCheckListView::CategoryCheckListView( QWidget *parent, RecipeDB *db, bool _exclusive, const ElementList &init_items_checked ) :
CategoryListView( parent, db ), exclusive( _exclusive )
{
	addColumn( i18n( "Category" ) );

	KConfigGroup config = KGlobal::config()->group( "Advanced" );
	bool show_id = config.readEntry( "ShowID", false );
	addColumn( "Id" , show_id ? -1 : 0 );

	for ( ElementList::const_iterator it = init_items_checked.begin(); it != init_items_checked.end(); ++it )
		m_selections.append(*it);
}

void CategoryCheckListView::removeCategory( int id )
{
	Q3ListViewItem * item = items_map[ id ];

	items_map.remove( id );
	removeElement(item);
}

void CategoryCheckListView::createCategory( const Element &category, int parent_id )
{
	CategoryCheckListItem * new_item = 0;
	if ( parent_id == -1 ) {
		new_item = new CategoryCheckListItem( this, category, exclusive );
	}
	else {
		Q3ListViewItem *parent = items_map[ parent_id ];
		if ( parent )
			new_item = new CategoryCheckListItem( parent, category, exclusive );
	}

	if ( new_item ) {
		items_map.insert( category.id, new_item );
		createElement(new_item);

		CategoryTree list;
		CategoryTree *p_list = &list;
		database->loadCachedCategories( &p_list, 1, 0, category.id, false );

		if ( p_list->firstChild() )
			new PseudoListItem( new_item );


		new_item->setOpen( false );
	}
}

void CategoryCheckListView::stateChange( CategoryCheckListItem* it, bool on )
{
	if ( !reloading() ) {
		if ( on )
			m_selections.append(it->element());
		else
			m_selections.removeAll(it->element());
	}
}

void CategoryCheckListView::load( int limit, int offset )
{
	CategoryListView::load(limit,offset);

	populateAll();

	for ( ElementList::const_iterator it = m_selections.constBegin(); it != m_selections.constEnd(); ++it ) {
		Q3CheckListItem * item = ( Q3CheckListItem* ) findItem( QString::number( (*it).id ), 1 );
		if ( item ) {
			item->setOn(true);
		}
	}
}

#include "categorylistview.moc"
