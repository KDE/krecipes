/***************************************************************************
*   Copyright (C) 2004 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "recipelistview.h"

#include <qintdict.h>
#include <qdatastream.h>
#include <qtooltip.h>

#include <kapplication.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kprogress.h>

#include "backends/recipedb.h"

class UncategorizedItem : public QListViewItem
{
public:
	UncategorizedItem( QListView *lv ) : QListViewItem( lv, i18n("Uncategorized") ){}
	int rtti() const { return 1006; }
};

RecipeItemDrag::RecipeItemDrag( RecipeListItem *recipeItem, QWidget *dragSource, const char *name )
		: QStoredDrag( RECIPEITEMMIMETYPE, dragSource, name )
{
	if ( recipeItem ) {
		QByteArray data;
		QDataStream out( data, IO_WriteOnly );
		out << recipeItem->recipeID();
		out << recipeItem->title();
		setEncodedData( data );
	}
}

bool RecipeItemDrag::canDecode( QMimeSource* e )
{
	return e->provides( RECIPEITEMMIMETYPE );
}

bool RecipeItemDrag::decode( const QMimeSource* e, RecipeListItem& item )
{
	if ( !e )
		return false;

	QByteArray data = e->encodedData( RECIPEITEMMIMETYPE );
	if ( data.isEmpty() )
		return false;

	QString title;
	int recipeID;
	QDataStream in( data, IO_ReadOnly );
	in >> recipeID;
	in >> title;

	item.setTitle( title );
	item.setRecipeID( recipeID );

	return true;
}

class RecipeListToolTip : public QToolTip
{
public:
	RecipeListToolTip( RecipeListView *view ) : QToolTip(view->viewport()), m_view(view)
	{}

	void maybeTip( const QPoint &point )
	{
		QListViewItem *item = m_view->itemAt( point );
		if ( item ) {
			QString text = m_view->tooltip(item,0);
			if ( !text.isEmpty() )
				tip( m_view->itemRect( item ), text );
		}
	}

private:
	RecipeListView *m_view;

};


RecipeListView::RecipeListView( QWidget *parent, RecipeDB *db ) : StdCategoryListView( parent, db ),
		flat_list( false ),
		m_uncat_item(0),
		m_progress_dlg(0)
{
	setColumnText( 0, i18n( "Recipe" ) );

	KConfig *config = KGlobal::config(); config->setGroup( "Performance" );
	curr_limit = config->readNumEntry("CategoryLimit",-1);

	KIconLoader il;
	setPixmap( il.loadIcon( "categories", KIcon::NoGroup, 16 ) );

	setSelectionMode( QListView::Extended );

	(void)new RecipeListToolTip(this);
}

void RecipeListView::init()
{
	connect( database, SIGNAL( recipeCreated( const Element &, const ElementList & ) ), SLOT( createRecipe( const Element &, const ElementList & ) ) );
	connect( database, SIGNAL( recipeRemoved( int ) ), SLOT( removeRecipe( int ) ) );
	connect( database, SIGNAL( recipeRemoved( int, int ) ), SLOT( removeRecipe( int, int ) ) );
	connect( database, SIGNAL( recipeModified( const Element &, const ElementList & ) ), SLOT( modifyRecipe( const Element &, const ElementList & ) ) );

	StdCategoryListView::init();
}

QDragObject *RecipeListView::dragObject()
{
	RecipeListItem * item = dynamic_cast<RecipeListItem*>( currentItem() );
	if ( item != 0 ) {
		RecipeItemDrag * obj = new RecipeItemDrag( item, this, "Recipe drag item" );
		/*const QPixmap *pm = item->pixmap(0);
		if( pm )
			obj->setPixmap( *pm );*/ 
		return obj;
	}
	return 0;
}

bool RecipeListView::acceptDrag( QDropEvent *event ) const
{
	return RecipeItemDrag::canDecode( event );
}

QString RecipeListView::tooltip(QListViewItem *item, int column) const
{
	if ( item->rtti() == RECIPELISTITEM_RTTI ) {
		RecipeListItem *recipe_it = (RecipeListItem*)item;

		Recipe r;
 		database->loadRecipe(&r,RecipeDB::Meta|RecipeDB::Noatime,recipe_it->recipeID() );

		KLocale *locale = KGlobal::locale();

		return QString("<center><b>%7</b></center><center>__________</center>%1 %2<br />%3 %4<br />%5 %6")
		   .arg(i18n("Created:")).arg(locale->formatDateTime(r.ctime))
		   .arg(i18n("Modified:")).arg(locale->formatDateTime(r.mtime))
		   .arg(i18n("Last Accessed:")).arg(locale->formatDateTime(r.atime))
		   .arg(recipe_it->title());
	}/* Maybe this would be handy
	else if ( item->rtti() == CATEGORYLISTITEM_RTTI ) {
		CategoryListItem *cat_it = (CategoryListItem*)item;

		return QString("<b>%1</b><hr />%2: %3")
		   .arg(cat_it->categoryName())
		   .arg(i18n("Recipes"))
		   .arg(QString::number(WHATEVER THE CHILD COUNT IS));
	}*/

	return QString::null;
}

void RecipeListView::load(int limit, int offset)
{
	m_uncat_item = 0;

	if ( flat_list ) {
		ElementList recipeList;
		database->loadRecipeList( &recipeList );

		ElementList::const_iterator recipe_it;
		for ( recipe_it = recipeList.begin();recipe_it != recipeList.end();++recipe_it ) {
			Recipe recipe;
			recipe.recipeID = ( *recipe_it ).id;
			recipe.title = ( *recipe_it ).name;
			createRecipe( recipe, -1 );
		}
	}
	else {
		StdCategoryListView::load(limit,offset);

		if ( offset == 0 ) {
			ElementList recipeList;
			database->loadUncategorizedRecipes( &recipeList );

			ElementList::const_iterator recipe_it;
			for ( recipe_it = recipeList.begin();recipe_it != recipeList.end();++recipe_it ) {
				Recipe recipe;
				recipe.recipeID = ( *recipe_it ).id;
				recipe.title = ( *recipe_it ).name;
				createRecipe( recipe, -1 );
			}
		}
	}
}

void RecipeListView::populate( QListViewItem *item )
{
	CategoryItemInfo *cat_item = dynamic_cast<CategoryItemInfo*>(item);
	if ( !cat_item || cat_item->isPopulated() ) return;

	delete item->firstChild(); //delete the "pseudo item"

	if ( m_progress_dlg ){
		m_progress_dlg->progressBar()->advance(1);
		kapp->processEvents();
	}

	StdCategoryListView::populate(item);

	if ( !flat_list ) {
		int id = cat_item->categoryId();

		// Now show the recipes
		ElementList recipeList;
		database->loadRecipeList( &recipeList, id );

		ElementList::const_iterator recipe_it;
		for ( recipe_it = recipeList.begin(); recipe_it != recipeList.end(); ++recipe_it ) {
			Recipe recipe;
			recipe.recipeID = ( *recipe_it ).id;
			recipe.title = ( *recipe_it ).name;
			createRecipe( recipe, id );
		}
	}
}

void RecipeListView::populateAll( QListViewItem *parent )
{
	bool first = false;
	if ( !parent ) {
		first = true;
		m_progress_dlg = new KProgressDialog(this,"populate_all_prog_dlg",QString::null,i18n("Loading recipes"),true);
		m_progress_dlg->setAllowCancel(false);
		m_progress_dlg->progressBar()->setTotalSteps(0);
		m_progress_dlg->progressBar()->setPercentageVisible(false);

		m_progress_dlg->grabKeyboard(); //don't let the user keep hitting keys

		parent = firstChild();
	}
	else {
		populate( parent );
		parent = parent->firstChild();
	}

	for ( QListViewItem *item = parent; item; item = item->nextSibling() ) {
		if ( m_progress_dlg && m_progress_dlg->wasCancelled() )
			break;

		populateAll( item );
	}

	if ( first ) {
		delete m_progress_dlg;
		m_progress_dlg = 0;
	}
}

void RecipeListView::createRecipe( const Recipe &recipe, int parent_id )
{
	if ( parent_id == -1 ) {
		if ( !m_uncat_item && curr_offset == 0 ) {
			m_uncat_item = new UncategorizedItem(this);
			if ( childCount() == 1 ) 		//only call createElement if this is the only item in the list
				createElement(m_uncat_item);	//otherwise, this item won't stay at the top
		}

		if ( m_uncat_item )
			new RecipeListItem( m_uncat_item, recipe );
	}
	else {
		CategoryListItem *parent = (CategoryListItem*)items_map[ parent_id ];
		if ( parent && parent->isPopulated() )
			createElement(new RecipeListItem( parent, recipe ));
	}
}

void RecipeListView::createRecipe( const Element &recipe_el, const ElementList &categories )
{
	Recipe recipe;
	recipe.recipeID = recipe_el.id;
	recipe.title = recipe_el.name;

	if ( categories.count() == 0 ) {
		createRecipe( recipe, -1 );
	}
	else {
		for ( ElementList::const_iterator cat_it = categories.begin(); cat_it != categories.end(); ++cat_it ) {
			int cur_cat_id = ( *cat_it ).id;

			QListViewItemIterator iterator( this );
			while ( iterator.current() ) {
				if ( iterator.current() ->rtti() == 1001 ) {
					CategoryListItem * cat_item = ( CategoryListItem* ) iterator.current();
					if ( cat_item->categoryId() == cur_cat_id ) {
						createRecipe( recipe, cur_cat_id );
					}
				}
				++iterator;
			}
		}
	}
}

void RecipeListView::createElement( QListViewItem *item )
{
	CategoryItemInfo *cat_item = dynamic_cast<CategoryItemInfo*>(item);
	if ( !cat_item || cat_item->isPopulated() ) return;

	#if 0
	ElementList list;
	database->loadRecipeList( &list, cat_item->categoryId() );
	if ( list.count() > 0 )
	#endif
		new PseudoListItem( item );

	CategoryListView::createElement(item);
}

void RecipeListView::modifyRecipe( const Element &recipe, const ElementList &categories )
{
	removeRecipe( recipe.id );
	createRecipe( recipe, categories );
}

void RecipeListView::removeRecipe( int id )
{
	QListViewItemIterator iterator( this );
	while ( iterator.current() ) {
		if ( iterator.current() ->rtti() == 1000 ) {
			RecipeListItem * recipe_it = ( RecipeListItem* ) iterator.current();
			if ( recipe_it->recipeID() == id )
				removeElement(recipe_it);
		}
		++iterator;
	}
}

void RecipeListView::removeRecipe( int recipe_id, int cat_id )
{
	QListViewItem * item = items_map[ cat_id ];

	//find out if this is the only category the recipe belongs to
	int finds = 0;
	QListViewItemIterator iterator( this );
	while ( iterator.current() ) {
		if ( iterator.current() ->rtti() == 1000 ) {
			RecipeListItem * recipe_it = ( RecipeListItem* ) iterator.current();

			if ( recipe_it->recipeID() == recipe_id ) {
				if ( finds > 1 )
					break;
				finds++;
			}
		}
		++iterator;
	}

	//do this to only iterate over children of 'item'
	QListViewItem *pEndItem = NULL;
	QListViewItem *pStartItem = item;
	do {
		if ( pStartItem->nextSibling() )
			pEndItem = pStartItem->nextSibling();
		else
			pStartItem = pStartItem->parent();
	}
	while ( pStartItem && !pEndItem );

	iterator = QListViewItemIterator( item );
	while ( iterator.current() != pEndItem ) {
		if ( iterator.current() ->rtti() == 1000 ) {
			RecipeListItem * recipe_it = ( RecipeListItem* ) iterator.current();

			if ( recipe_it->recipeID() == recipe_id ) {
				
				if ( finds == 1 ) {
					//the item is now uncategorized
					if ( !m_uncat_item && curr_offset == 0 )
						m_uncat_item = new UncategorizedItem(this);
					if ( m_uncat_item ) {
						Recipe r;
						r.title = recipe_it->title(); r.recipeID = recipe_id;
						new RecipeListItem(m_uncat_item,r);
					}
				}
				removeElement(recipe_it);
				break;
			}
		}
		++iterator;
	}
}

void RecipeListView::removeCategory( int id )
{
	QListViewItem * item = items_map[ id ];
	if ( !item )
		return ; //this may have been deleted already by its parent being deleted

	moveChildrenToRoot( item );

	StdCategoryListView::removeCategory( id );
}

void RecipeListView::moveChildrenToRoot( QListViewItem *item )
{
	QListViewItem * next_sibling;
	for ( QListViewItem * it = item->firstChild(); it; it = next_sibling ) {
		next_sibling = it->nextSibling();
		if ( it->rtti() == 1000 ) {
			RecipeListItem *recipe_it = (RecipeListItem*) it;
			Recipe r;
			r.title = recipe_it->title(); r.recipeID = recipe_it->recipeID();

			//the item is now uncategorized
			removeElement(it,false);
			it->parent() ->takeItem( it );
			if ( !m_uncat_item && curr_offset == 0 )
				m_uncat_item = new UncategorizedItem(this);
			if ( m_uncat_item )
				new RecipeListItem(m_uncat_item,r);
		}
		moveChildrenToRoot( it );
		delete it;
	}
}

#include "recipelistview.moc"
