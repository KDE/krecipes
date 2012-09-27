/***************************************************************************
*   Copyright © 2004 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "recipelistview.h"

#include <q3intdict.h>
#include <qdatastream.h>
#include <QToolTip>
//Added by qt3to4:
#include <QDropEvent>

#include <kapplication.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kprogressdialog.h>

#include "backends/recipedb.h"

class UncategorizedItem : public Q3ListViewItem
{
public:
	UncategorizedItem( Q3ListView *lv ) : Q3ListViewItem( lv, i18n("Uncategorized") ){}
	int rtti() const { return 1006; }
};

RecipeItemDrag::RecipeItemDrag( RecipeListItem *recipeItem, QWidget *dragSource, const char *name )
		: Q3StoredDrag( RECIPEITEMMIMETYPE, dragSource, name )
{
	if ( recipeItem ) {
		QByteArray data;
		QDataStream out( &data, QIODevice::WriteOnly );
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
	QDataStream in( &data, QIODevice::ReadOnly );
	in >> recipeID;
	in >> title;

	item.setTitle( title );
	item.setRecipeID( recipeID );

	return true;
}

class RecipeListToolTip 
{
public:
	RecipeListToolTip( RecipeListView *view ) : m_view(view)
	{}

	void maybeTip( const QPoint &point )
	{
		Q3ListViewItem *item = m_view->itemAt( point );
		if ( item ) {
			QString text = m_view->tooltip(item,0);
			if ( !text.isEmpty() )
				QToolTip::showText( point, text, m_view->viewport(), m_view->itemRect( item ) );
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

	KConfigGroup config = KGlobal::config()->group( "Performance" );
	curr_limit = config.readEntry("CategoryLimit",-1);

	KIconLoader *il = KIconLoader::global();
	setPixmap( il->loadIcon( "folder-yellow", KIconLoader::NoGroup, 16 ) );

	setSelectionMode( Q3ListView::Extended );

	(void)new RecipeListToolTip(this);

	connect( database, SIGNAL( recipeCreated( const Element &, const ElementList & ) ), SLOT( createRecipe( const Element &, const ElementList & ) ) );
	connect( database, SIGNAL( recipeRemoved( int ) ), SLOT( removeRecipe( int ) ) );
	connect( database, SIGNAL( recipeRemoved( int, int ) ), SLOT( removeRecipe( int, int ) ) );
	connect( database, SIGNAL( recipeModified( const Element &, const ElementList & ) ), SLOT( modifyRecipe( const Element &, const ElementList & ) ) );

}

Q3DragObject *RecipeListView::dragObject()
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

QString RecipeListView::tooltip(Q3ListViewItem *item, int /*column*/) const
{
	if ( item->rtti() == RECIPELISTITEM_RTTI ) {
		RecipeListItem *recipe_it = (RecipeListItem*)item;

		Recipe r;
		database->loadRecipe(&r,RecipeDB::Meta|RecipeDB::Noatime,recipe_it->recipeID() );

		KLocale *locale = KGlobal::locale();

		return QString("<center><b>%7</b></center><center>__________</center>%1 %2<br />%3 %4<br />%5 %6")
		.arg(i18nc("Recipe created", "Created:")).arg(locale->formatDateTime(r.ctime))
		.arg(i18nc("Recipe modified", "Modified:")).arg(locale->formatDateTime(r.mtime))
		.arg(i18nc("Recipe last accessed", "Last Accessed:")).arg(locale->formatDateTime(r.atime))
		.arg(recipe_it->title());
	}/* Maybe this would be handy
	else if ( item->rtti() == CATEGORYLISTITEM_RTTI ) {
		CategoryListItem *cat_it = (CategoryListItem*)item;

		return QString("<b>%1</b><hr />%2: %3")
		.arg(cat_it->categoryName())
		.arg(i18n("Recipes"))
		.arg(QString::number(WHATEVER THE CHILD COUNT IS));
	}*/

	return QString();
}

void RecipeListView::load(int limit, int offset)
{
	m_uncat_item = 0;

	if ( flat_list ) {
		ElementList recipeList;
		database->loadRecipeList( &recipeList );

		ElementList::const_iterator recipe_it;
		for ( recipe_it = recipeList.constBegin();recipe_it != recipeList.constEnd();++recipe_it ) {
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
			for ( recipe_it = recipeList.constBegin();recipe_it != recipeList.constEnd();++recipe_it ) {
				Recipe recipe;
				recipe.recipeID = ( *recipe_it ).id;
				recipe.title = ( *recipe_it ).name;
				createRecipe( recipe, -1 );
			}
		}
	}
}

void RecipeListView::populate( Q3ListViewItem *item )
{
	CategoryItemInfo *cat_item = dynamic_cast<CategoryItemInfo*>(item);
	if ( !cat_item || cat_item->isPopulated() ) return;

	delete item->firstChild(); //delete the "pseudo item"

	if ( m_progress_dlg ){
		m_progress_dlg->progressBar()->setValue(m_progress_dlg->progressBar()->value() + 1);
		kapp->processEvents();
	}

	StdCategoryListView::populate(item);

	if ( !flat_list ) {
		int id = cat_item->categoryId();

		// Now show the recipes
		ElementList recipeList;
		database->loadRecipeList( &recipeList, id );

		ElementList::const_iterator recipe_it;
		for ( recipe_it = recipeList.constBegin(); recipe_it != recipeList.constEnd(); ++recipe_it ) {
			Recipe recipe;
			recipe.recipeID = ( *recipe_it ).id;
			recipe.title = ( *recipe_it ).name;
			createRecipe( recipe, id );
		}
	}
}

void RecipeListView::populateAll( Q3ListViewItem *parent )
{
	bool first = false;
	if ( !parent ) {
		first = true;
		m_progress_dlg = new KProgressDialog(this,QString(),i18nc("@info:progress", "Loading recipes"));
		m_progress_dlg->setObjectName( "populate_all_prog_dlg" );
		m_progress_dlg->setModal( true );
		m_progress_dlg->setAllowCancel(false);
		m_progress_dlg->progressBar()->setRange(0,0);

		m_progress_dlg->grabKeyboard(); //don't let the user keep hitting keys

		parent = firstChild();
	}
	else {
		populate( parent );
		parent = parent->firstChild();
	}

	for ( Q3ListViewItem *item = parent; item; item = item->nextSibling() ) {
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
		}

		if ( m_uncat_item )
			createElement(new RecipeListItem( m_uncat_item, recipe ));
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

	if ( categories.isEmpty() ) {
		createRecipe( recipe, -1 );
	}
	else {
		for ( ElementList::const_iterator cat_it = categories.begin(); cat_it != categories.end(); ++cat_it ) {
			int cur_cat_id = ( *cat_it ).id;

			Q3ListViewItemIterator iterator( this );
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

void RecipeListView::createElement( Q3ListViewItem *item )
{
	CategoryItemInfo *cat_item = dynamic_cast<CategoryItemInfo*>(item);
	if ( cat_item && !cat_item->isPopulated() ) {
		new PseudoListItem( item );
	}

	//if ( cat_item && !cat_item->isPopulated() && item->rtti() == RECIPELISTITEM_RTTI )
	//	return;

	#if 0
	ElementList list;
	database->loadRecipeList( &list, cat_item->categoryId() );
	if ( list.count() > 0 )
	#endif

	CategoryListView::createElement(item);
}

void RecipeListView::modifyRecipe( const Element &recipe, const ElementList &categories )
{
	removeRecipe( recipe.id );
	createRecipe( recipe, categories );
}

void RecipeListView::removeRecipe( int id )
{
	Q3ListViewItemIterator iterator( this );
	while ( iterator.current() ) {
		if ( iterator.current() ->rtti() == 1000 ) {
			RecipeListItem * recipe_it = ( RecipeListItem* ) iterator.current();
			if ( recipe_it->recipeID() == id ) {
				removeElement(recipe_it);

				//delete the "Uncategorized" item if we removed the last recipe that was under it
				if ( m_uncat_item && m_uncat_item->childCount() == 0 ) {
					delete m_uncat_item;
					m_uncat_item = 0;
				}
			}
		}
		++iterator;
	}
}

void RecipeListView::removeRecipe( int recipe_id, int cat_id )
{
	Q3ListViewItem * item = items_map[ cat_id ];

	if (!item)
		return;

	//find out if this is the only category the recipe belongs to
	bool one_category;
	Recipe r;
	database->loadRecipe(&r, RecipeDB::Categories, recipe_id );
	one_category = r.categoryList.isEmpty();

	//do this to only iterate over children of 'item'
	Q3ListViewItem *pEndItem = NULL;
	Q3ListViewItem *pStartItem = item;
	do {
		if ( pStartItem->nextSibling() )
			pEndItem = pStartItem->nextSibling();
		else
			pStartItem = pStartItem->parent();
	}
	while ( pStartItem && !pEndItem );

	Q3ListViewItemIterator iterator( item );
	while ( iterator.current() != pEndItem ) {
		if ( iterator.current() ->rtti() == 1000 ) {
			RecipeListItem * recipe_it = ( RecipeListItem* ) iterator.current();

			if ( recipe_it->recipeID() == recipe_id ) {
				
				if ( one_category ) {
					//the item is now uncategorized
					kDebug() << "uncat";
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
	Q3ListViewItem * item = items_map[ id ];
	if ( !item )
		return ; //this may have been deleted already by its parent being deleted

	moveChildrenToRoot( item );

	StdCategoryListView::removeCategory( id );
}

void RecipeListView::moveChildrenToRoot( Q3ListViewItem *item )
{
	Q3ListViewItem * next_sibling;
	Recipe recipe;
	for ( Q3ListViewItem * it = item->firstChild(); it; it = next_sibling ) {
		next_sibling = it->nextSibling();
		if ( it->rtti() == 1000 ) {
			RecipeListItem *recipe_it = (RecipeListItem*) it;
			removeElement(it,false);

			//the item is now uncategorized if it's not part of other categories
			database->loadRecipe( &recipe,
				RecipeDB::Title|RecipeDB::Categories, recipe_it->recipeID() );

			if ( recipe.categoryList.isEmpty() ) {
				it->parent() ->takeItem( it );
				if ( !m_uncat_item && curr_offset == 0 )
					m_uncat_item = new UncategorizedItem(this);
				if ( m_uncat_item )
					new RecipeListItem(m_uncat_item,recipe);
			}
		}
		moveChildrenToRoot( it );
		delete it;
	}
}

#include "recipelistview.moc"
