
/***************************************************************************
*   Copyright (C) 2004 by                                                 *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
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
#include <kpopupmenu.h>
#include <kconfig.h>
#include <kglobal.h>

#include "backends/recipedb.h"
#include "datablocks/categorytree.h"
#include "dialogs/createcategorydialog.h"
//Added by qt3to4:
#include <QPixmap>
#include <Q3ValueList>

CategoryCheckListItem::CategoryCheckListItem( CategoryCheckListView* klv, const Element &category, bool _exclusive ) : Q3CheckListItem( klv, QString::null, Q3CheckListItem::CheckBox ),
		locked( false ),
		exclusive( _exclusive ),
		ctyStored( category ),
		m_listview(klv)
{
	setOn( false ); // Set unchecked by default
}

CategoryCheckListItem::CategoryCheckListItem( Q3ListViewItem* it, const Element &category, bool _exclusive ) : Q3CheckListItem( it, QString::null, Q3CheckListItem::CheckBox ),
		locked( false ),
		exclusive( _exclusive ),
		ctyStored( category ),
		m_listview((CategoryCheckListView*)it->listView())
{
	setOn( false ); // Set unchecked by default
}

CategoryCheckListItem::CategoryCheckListItem( CategoryCheckListView* klv, Q3ListViewItem* it, const Element &category, bool _exclusive ) : Q3CheckListItem( klv, it, QString::null, Q3CheckListItem::CheckBox ),
		locked( false ),
		exclusive( _exclusive ),
		ctyStored( category ),
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
		ctyStored( category )
{}

CategoryListItem::CategoryListItem( Q3ListViewItem* it, const Element &category ) : Q3ListViewItem( it ),
		ctyStored( category )
{}

CategoryListItem::CategoryListItem( Q3ListView* klv, Q3ListViewItem* it, const Element &category ) : Q3ListViewItem( klv, it ),
		ctyStored( category )
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



CategoryListView::CategoryListView( QWidget *parent, RecipeDB *db ) : DBListViewBase( parent, db, db->categoryTopLevelCount() )
{
	connect( db, SIGNAL( categoryCreated( const Element &, int ) ), SLOT( createCategory( const Element &, int ) ) );
	connect( db, SIGNAL( categoryRemoved( int ) ), SLOT( removeCategory( int ) ) );
	connect( db, SIGNAL( categoryModified( const Element & ) ), SLOT( modifyCategory( const Element & ) ) );
	connect( db, SIGNAL( categoryModified( int, int ) ), SLOT( modifyCategory( int, int ) ) );
	connect( db, SIGNAL( categoriesMerged( int, int ) ), SLOT( mergeCategories( int, int ) ) );

	connect( this, SIGNAL( spacePressed(Q3ListViewItem*) ), SLOT( open(Q3ListViewItem*) ) );
	connect( this, SIGNAL( returnPressed(Q3ListViewItem*) ), SLOT( open(Q3ListViewItem*) ) );
	connect( this, SIGNAL( executed(Q3ListViewItem*) ), SLOT( open(Q3ListViewItem*) ) );

	setRootIsDecorated( true );
	setAllColumnsShowFocus( true );
	setDefaultRenameAction( Q3ListView::Reject );
}

// (Re)loads the data from the database
void CategoryListView::load( int limit, int offset )
{
	CategoryTree categoryTree;
	database->loadCategories( &categoryTree, limit, offset, -1, false );

	for ( CategoryTree * child_it = categoryTree.firstChild(); child_it; child_it = child_it->nextSibling() ) {
		createCategory( child_it->category, -1 );
	}
}

void CategoryListView::populate( Q3ListViewItem *item )
{
	if ( item->firstChild() ) return;

	int id;
	if ( item->rtti() == CATEGORYLISTITEM_RTTI ) {
		CategoryListItem *cat_item = (CategoryListItem*)item;
		id = cat_item->categoryId();
	}
	else if ( item->rtti() == CATEGORYCHECKLISTITEM_RTTI ) {
		CategoryCheckListItem *cat_item = (CategoryCheckListItem*)item;
		id = cat_item->categoryId();
	}
	else
		return;

	CategoryTree categoryTree;
	database->loadCategories( &categoryTree, -1, 0, id, false );
	
	for ( CategoryTree * child_it = categoryTree.firstChild(); child_it; child_it = child_it->nextSibling() ) {
		createCategory( child_it->category, id );
	}
}

void CategoryListView::open( Q3ListViewItem *item )
{
	Q_ASSERT( item );
	if ( childCount() == 0 ) return;

	if ( !item->firstChild() )
		populate(item);

	item->setOpen(true);
}

void CategoryListView::checkCreateCategory( const Element &el, int parent_id )
{
	if ( parent_id == -1 && handleElement(el.name) ) { //only create this category if the base class okays it and it is a top level item
		createCategory(el,parent_id);
	}
}


StdCategoryListView::StdCategoryListView( QWidget *parent, RecipeDB *db, bool editable ) : CategoryListView( parent, db ),
		clipboard_item( 0 ),
		clipboard_parent( 0 )
{
	addColumn( i18n( "Category" ) );

	KConfig *config = KGlobal::config();
	config->setGroup( "Advanced" );
	bool show_id = config->readBoolEntry( "ShowID", false );
	addColumn( i18n( "Id" ), show_id ? -1 : 0 );

	if ( editable ) {
		setRenameable( 0, true );
		setDragEnabled( true );
		setAcceptDrops( true );

		KIconLoader *il = new KIconLoader;

		kpop = new KPopupMenu( this );
		kpop->insertItem( il->loadIcon( "filenew", KIcon::NoGroup, 16 ), i18n( "&Create" ), this, SLOT( createNew() ), Qt::CTRL + Qt::Key_C );
		kpop->insertItem( il->loadIcon( "editdelete", KIcon::NoGroup, 16 ), i18n( "&Delete" ), this, SLOT( remove
			                  () ), Qt::Key_Delete );
		kpop->insertItem( il->loadIcon( "edit", KIcon::NoGroup, 16 ), i18n( "&Rename" ), this, SLOT( rename() ), Qt::CTRL + Qt::Key_R );
		kpop->insertSeparator();
		kpop->insertItem( il->loadIcon( "editcut", KIcon::NoGroup, 16 ), i18n( "Cu&t" ), this, SLOT( cut() ), Qt::CTRL + Qt::Key_X );
		kpop->insertItem( il->loadIcon( "editpaste", KIcon::NoGroup, 16 ), i18n( "&Paste" ), this, SLOT( paste() ), Qt::CTRL + Qt::Key_V );
		kpop->insertItem( il->loadIcon( "editpaste", KIcon::NoGroup, 16 ), i18n( "Paste as Subcategory" ), this, SLOT( pasteAsSub() ), Qt::CTRL + Qt::SHIFT + Qt::Key_V );
		kpop->polish();

		delete il;

		connect( kpop, SIGNAL( aboutToShow() ), SLOT( preparePopup() ) );
		connect( this, SIGNAL( contextMenu( KListView *, Q3ListViewItem *, const QPoint & ) ), SLOT( showPopup( KListView *, Q3ListViewItem *, const QPoint & ) ) );
		connect( this, SIGNAL( doubleClicked( Q3ListViewItem*, const QPoint &, int ) ), SLOT( modCategory( Q3ListViewItem* ) ) );
		connect( this, SIGNAL( itemRenamed ( Q3ListViewItem* ) ), SLOT( saveCategory( Q3ListViewItem* ) ) );
		connect( this, SIGNAL( moved( Q3ListViewItem *, Q3ListViewItem *, Q3ListViewItem * ) ), SLOT( changeCategoryParent( Q3ListViewItem *, Q3ListViewItem *, Q3ListViewItem * ) ) );
	}
}

StdCategoryListView::~StdCategoryListView()
{
	delete clipboard_item;
}

void StdCategoryListView::load(int limit, int offset)
{
	items_map.clear();

	CategoryListView::load(limit,offset);
}

void StdCategoryListView::setPixmap( const QPixmap &icon )
{
	m_folder_icon = icon;
}

void StdCategoryListView::preparePopup()
{
	//only enable the paste items if clipboard_item isn't null
	kpop->setItemEnabled( kpop->idAt( 5 ), clipboard_item );
	kpop->setItemEnabled( kpop->idAt( 6 ), clipboard_item );
}

void StdCategoryListView::showPopup( KListView * /*l*/, Q3ListViewItem *i, const QPoint &p )
{
	if ( i )
		kpop->exec( p );
}

void StdCategoryListView::createNew()
{
	ElementList categories;
	database->loadCategories( &categories );
	CreateCategoryDialog* categoryDialog = new CreateCategoryDialog( this, categories );

	if ( categoryDialog->exec() == QDialog::Accepted ) {
		QString result = categoryDialog->newCategoryName();
		int subcategory = categoryDialog->subcategory();

		//check bounds first
		if ( checkBounds( result ) )
			database->createNewCategory( result, subcategory ); // Create the new category in the database
	}
	delete categoryDialog;
}

void StdCategoryListView::remove
	()
{
	Q3ListViewItem * item = currentItem();

	if ( item ) {
		switch ( KMessageBox::warningContinueCancel( this, i18n( "Are you sure you want to delete this category and all its subcategories?" ) ) ) {
		case KMessageBox::Continue:
			database->removeCategory( item->text( 1 ).toInt() );
			break;
		default:
			break;
		}
	}
}

void StdCategoryListView::rename()
{
	Q3ListViewItem * item = currentItem();

	if ( item )
		CategoryListView::rename( item, 0 );
}

void StdCategoryListView::cut()
{
	//restore a never used cut
	if ( clipboard_item ) {
		if ( clipboard_parent )
			clipboard_parent->insertItem( clipboard_item );
		else
			insertItem( clipboard_item );
		clipboard_item = 0;
	}

	Q3ListViewItem *item = currentItem();

	if ( item ) {
		clipboard_item = item;
		clipboard_parent = item->parent();

		if ( item->parent() )
			item->parent() ->takeItem( item );
		else
			takeItem( item );
	}
}

void StdCategoryListView::paste()
{
	Q3ListViewItem * item = currentItem();
	if ( item && clipboard_item ) {
		if ( item->parent() )
			item->parent() ->insertItem( clipboard_item );
		else
			insertItem( clipboard_item );

		database->modCategory( clipboard_item->text( 1 ).toInt(), item->parent() ? item->parent() ->text( 1 ).toInt() : -1 );
		clipboard_item = 0;
	}
}

void StdCategoryListView::pasteAsSub()
{
	Q3ListViewItem * item = currentItem();

	if ( item && clipboard_item ) {
		item->insertItem( clipboard_item );
		database->modCategory( clipboard_item->text( 1 ).toInt(), item->text( 1 ).toInt() );
		clipboard_item = 0;
	}
}

void StdCategoryListView::changeCategoryParent( Q3ListViewItem *item, Q3ListViewItem * /*afterFirst*/, Q3ListViewItem * /*afterNow*/ )
{
	int new_parent_id = -1;
	if ( Q3ListViewItem * parent = item->parent() )
		new_parent_id = parent->text( 1 ).toInt();

	int cat_id = item->text( 1 ).toInt();

	disconnect( SIGNAL( moved( Q3ListViewItem *, Q3ListViewItem *, Q3ListViewItem * ) ) );
	database->modCategory( cat_id, new_parent_id );
	connect( this, SIGNAL( moved( Q3ListViewItem *, Q3ListViewItem *, Q3ListViewItem * ) ), SLOT( changeCategoryParent( Q3ListViewItem *, Q3ListViewItem *, Q3ListViewItem * ) ) );
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
		createElement(new_item);
	}
	else {
		Q3ListViewItem *parent = items_map[ parent_id ];

		if ( parent ) {
			new_item = new CategoryListItem( parent, category );

			Q3ListViewItem *lastItem = 0;
			for ( Q3ListViewItem *it = parent->firstChild(); it; it = it->nextSibling() ) {
				lastItem = it;
			}
			new_item->moveItem( lastItem );
		}
	}

	if ( new_item ) {
		items_map.insert( category.id, new_item );
		new_item->setPixmap( 0, m_folder_icon );
	}
}

void StdCategoryListView::modifyCategory( const Element &category )
{
	Q3ListViewItem * item = items_map[ category.id ];

	if ( item )
		item->setText( 0, category.name );
}

void StdCategoryListView::modifyCategory( int id, int parent_id )
{
	Q3ListViewItem * item = items_map[ id ];
	if ( item ) {
		if ( !item->parent() )
			takeItem( item );
		else
			item->parent() ->takeItem( item );
	
		if ( parent_id == -1 )
			insertItem( item );
		else
			items_map[ parent_id ] ->insertItem( item );
	}
}

void StdCategoryListView::mergeCategories( int id1, int id2 )
{
	Q3ListViewItem * to_item = items_map[ id1 ];
	Q3ListViewItem *from_item = items_map[ id2 ];

	if ( to_item && from_item ) { 
		//note that this takes care of any recipes that may be children as well
		Q3ListViewItem *next_sibling;
		for ( Q3ListViewItem * it = from_item->firstChild(); it; it = next_sibling ) {
			next_sibling = it->nextSibling(); //get the sibling before we move the item
	
			from_item->takeItem( it );
			to_item->insertItem( it );
		}
	}

	removeCategory( id2 );
}

void StdCategoryListView::modCategory( Q3ListViewItem* i )
{
	if ( i )
		CategoryListView::rename( i, 0 );
}

void StdCategoryListView::saveCategory( Q3ListViewItem* i )
{
	CategoryListItem * cat_it = ( CategoryListItem* ) i;

	if ( !checkBounds( cat_it->categoryName() ) ) {
		reload(); //reset the changed text
		return ;
	}

	int existing_id = database->findExistingCategoryByName( cat_it->categoryName() );
	int cat_id = cat_it->categoryId();
	if ( existing_id != -1 && existing_id != cat_id )  //category already exists with this label... merge the two
	{
		switch ( KMessageBox::warningContinueCancel( this, i18n( "This category already exists.  Continuing will merge these two categories into one.  Are you sure?" ) ) )
		{
		case KMessageBox::Continue: {
				database->mergeCategories( existing_id, cat_id );
				break;
			}
		default:
			reload();
			break;
		}
	}
	else
		database->modCategory( cat_id, cat_it->categoryName() );
}

bool StdCategoryListView::checkBounds( const QString &name )
{
	if ( name.length() > database->maxCategoryNameLength() ) {
		KMessageBox::error( this, QString( i18n( "Category name cannot be longer than %1 characters." ) ).arg( database->maxCategoryNameLength() ) );
		return false;
	}

	return true;
}



CategoryCheckListView::CategoryCheckListView( QWidget *parent, RecipeDB *db, bool _exclusive, const ElementList &init_items_checked ) : CategoryListView( parent, db ),
  exclusive(_exclusive)
{
	addColumn( i18n( "Category" ) );

	KConfig *config = KGlobal::config();
	config->setGroup( "Advanced" );
	bool show_id = config->readBoolEntry( "ShowID", false );
	addColumn( i18n( "Id" ), show_id ? -1 : 0 );

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
		createElement(new_item);
	}
	else {
		Q3ListViewItem *parent = items_map[ parent_id ];
		if ( parent )
			new_item = new CategoryCheckListItem( parent, category, exclusive );
	}

	if ( new_item ) {
		items_map.insert( category.id, new_item );
		new_item->setOpen( true );
	}
}

void CategoryCheckListView::modifyCategory( const Element &category )
{
	Q3ListViewItem * item = items_map[ category.id ];

	if ( item )
		item->setText( 0, category.name );
}

void CategoryCheckListView::modifyCategory( int id, int parent_id )
{
	Q3ListViewItem * item = items_map[ id ];
	if ( item ) {
		if ( !item->parent() )
			takeItem( item );
		else
			item->parent() ->takeItem( item );
	
		Q_ASSERT( item );
	
		if ( parent_id == -1 )
			insertItem( item );
		else
			items_map[ parent_id ] ->insertItem( item );
	}
}

void CategoryCheckListView::mergeCategories( int id1, int id2 )
{
	Q3ListViewItem * to_item = items_map[ id1 ];
	Q3ListViewItem *from_item = items_map[ id2 ];

	if ( to_item && from_item ) {
		//note that this takes care of any recipes that may be children as well
		Q3ListViewItem *next_sibling;
		for ( Q3ListViewItem * it = from_item->firstChild(); it; it = next_sibling ) {
			next_sibling = it->nextSibling(); //get the sibling before we move the item
	
			from_item->takeItem( it );
			to_item->insertItem( it );
		}
	}

	removeCategory( id2 );
}

void CategoryCheckListView::stateChange( CategoryCheckListItem* it, bool on )
{
	if ( !reloading() ) {
		if ( on )
			m_selections.append(it->element());
		else
			m_selections.remove(it->element());
	}
}

void CategoryCheckListView::load( int limit, int offset )
{
	CategoryListView::load(limit,offset);

	for ( Q3ValueList<Element>::const_iterator it = m_selections.begin(); it != m_selections.end(); ++it ) {
		Q3CheckListItem * item = ( Q3CheckListItem* ) findItem( QString::number( (*it).id ), 1 );
		if ( item ) {
			item->setOn(true);
		}
	}
}

#include "categorylistview.moc"
