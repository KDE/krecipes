/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "advancedsearchdialog.h"

#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qwidgetstack.h>
#include <qwidget.h>
#include <qgroupbox.h>
#include <qcombobox.h>
#include <qheader.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qspinbox.h>
#include <qframe.h>
#include <qdatetimeedit.h>
#include <qvbox.h>

#include <kapplication.h>
#include <kcursor.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <klistview.h>
#include <klocale.h>
#include <kconfig.h>
#include <kglobal.h>

#include "DBBackend/recipedb.h"
#include "recipeactionshandler.h"
#include "widgets/recipelistview.h"

void storeMap(QMap<QCheckListItem*,bool> *map_to_store, QListView *listview)
{
	QCheckListItem* current_item;
	QListViewItemIterator it( listview );
	while ( it.current() ) {
		current_item = (QCheckListItem*)it.current();
		map_to_store->insert(current_item,current_item->isOn());

		++it;
	}
}

void updateMaps(QMap<QCheckListItem*,bool> *map_to_load, QMap<QCheckListItem*,bool> *map_to_store, QListView *listview)
{
	//store
	storeMap(map_to_store,listview);

	//restore
	for ( QMap<QCheckListItem*,bool>::Iterator it = map_to_load->begin(); it != map_to_load->end(); ++it )
	{
		it.key()->setOn(it.data());
		it.key()->setEnabled(true);
	}
	
	//be sensible and don't allow items to be simultaneously enabled and disabled
	for ( QMap<QCheckListItem*,bool>::Iterator it = map_to_store->begin(); it != map_to_store->end(); ++it )
		it.key()->setEnabled(!it.data());
}

DualAuthorListView::DualAuthorListView(QWidget *parent, RecipeDB *db) : AuthorListView(parent,db),
	last_state(0)
{
	addColumn(i18n("Name"));

	KConfig *config = KGlobal::config();
	config->setGroup( "Advanced" );
	bool show_id = config->readBoolEntry("ShowID",false);
	addColumn( i18n("Id"), show_id ? -1 : 0 );
}

void DualAuthorListView::reload()
{
	positiveMap.clear();
	negativeMap.clear();

	AuthorListView::reload();
}

void DualAuthorListView::change(int index)
{
	if ( index == last_state ) //don't do anything unless the selection has changed
		return;

	QMap<QCheckListItem*,bool> *map_to_store;
	QMap<QCheckListItem*,bool> *map_to_load;

	if ( index == 0 ) //store negative
	{
		map_to_load = &positiveMap;
		map_to_store = &negativeMap;
	}
	else //store positive
	{
		map_to_load = &negativeMap;
		map_to_store = &positiveMap;
	}
	
	updateMaps(map_to_load,map_to_store,this);
	
	last_state = index;
	clearSelection();
}
	
void DualAuthorListView::updateMap( int index )
{
	storeMap( (index == 0) ? &positiveMap : &negativeMap, this );
}

void DualAuthorListView::createAuthor(const Element &ing)
{
	QCheckListItem *item=new QCheckListItem(this,ing.name,QCheckListItem::CheckBox);
	item->setText(1,QString::number(ing.id));
	
	negativeMap.insert(item,false);
	positiveMap.insert(item,false);
}

void DualAuthorListView::removeAuthor(int id)
{
	QCheckListItem *item = (QCheckListItem*)findItem(QString::number(id),1);
	
	Q_ASSERT(item);

	negativeMap.remove(item);
	positiveMap.remove(item);
	delete item;
}


DualIngredientListView::DualIngredientListView(QWidget *parent, RecipeDB *db) : IngredientListView(parent,db),
	last_state(0)
{
	addColumn(i18n("Name"));

	KConfig *config = KGlobal::config();
	config->setGroup( "Advanced" );
	bool show_id = config->readBoolEntry("ShowID",false);
	addColumn( i18n("Id"), show_id ? -1 : 0 );
}

void DualIngredientListView::reload()
{
	positiveMap.clear();
	negativeMap.clear();

	IngredientListView::reload();
}

void DualIngredientListView::change(int index)
{
	if ( index == last_state ) //don't do anything unless the selection has changed
		return;

	QMap<QCheckListItem*,bool> *map_to_store;
	QMap<QCheckListItem*,bool> *map_to_load;

	if ( index == 0 ) //store negative
	{
		map_to_load = &positiveMap;
		map_to_store = &negativeMap;
	}
	else //store positive
	{
		map_to_load = &negativeMap;
		map_to_store = &positiveMap;
	}

	updateMaps(map_to_load,map_to_store,this);
	
	last_state = index;
	clearSelection();
}
	
void DualIngredientListView::updateMap( int index )
{
	storeMap( (index == 0) ? &positiveMap : &negativeMap, this );
}

void DualIngredientListView::createIngredient(const Element &ing)
{
	QCheckListItem *item=new QCheckListItem(this,ing.name,QCheckListItem::CheckBox);
	item->setText(1,QString::number(ing.id));
	
	negativeMap.insert(item,false);
	positiveMap.insert(item,false);
}

void DualIngredientListView::removeIngredient(int id)
{
	QCheckListItem *item = (QCheckListItem*)findItem(QString::number(id),1);
	
	Q_ASSERT(item);

	negativeMap.remove(item);
	positiveMap.remove(item);
	delete item;
}


DualCategoryListView::DualCategoryListView(QWidget *parent, RecipeDB *db) : CategoryListView(parent,db),
	last_state(0)
{
	addColumn(i18n("Name"));

	KConfig *config = KGlobal::config();
	config->setGroup( "Advanced" );
	bool show_id = config->readBoolEntry("ShowID",false);
	addColumn( i18n("Id"), show_id ? -1 : 0 );
}

void DualCategoryListView::reload()
{
	positiveMap.clear();
	negativeMap.clear();

	CategoryListView::reload();
}

void DualCategoryListView::change(int index)
{
	if ( index == last_state ) //don't do anything unless the selection has changed
		return;

	QMap<QCheckListItem*,bool> *map_to_store;
	QMap<QCheckListItem*,bool> *map_to_load;

	if ( index == 0 ) //store negative
	{
		map_to_load = &positiveMap;
		map_to_store = &negativeMap;
	}
	else //store positive
	{
		map_to_load = &negativeMap;
		map_to_store = &positiveMap;
	}
	
	updateMaps(map_to_load,map_to_store,this);
	
	last_state = index;
	clearSelection();
}

void DualCategoryListView::updateMap( int index )
{
	storeMap( (index == 0) ? &positiveMap : &negativeMap, this );
}

void DualCategoryListView::createCategory(const Element &category,int parent_id)
{
	CategoryCheckListItem *new_item;
	if ( parent_id == -1 )
		new_item = new CategoryCheckListItem(this,category,false);
	else
	{
		QListViewItem *parent = findItem(QString::number(parent_id),1);

		Q_ASSERT(parent);

		new_item = new CategoryCheckListItem(parent,category,false);
	}

	new_item->setOpen(true);
	
	negativeMap.insert(new_item,false);
	positiveMap.insert(new_item,false);
}

void DualCategoryListView::removeCategory(int id)
{
	QCheckListItem *item = (QCheckListItem*)findItem(QString::number(id),1);
	
	Q_ASSERT(item);

	negativeMap.remove(item);
	positiveMap.remove(item);
	delete item;
}

void DualCategoryListView::modifyCategory(const Element &category)
{
	QListViewItem *item = findItem(QString::number(category.id),1);

	Q_ASSERT(item);

	item->setText(0,category.name);
}

void DualCategoryListView::modifyCategory(int id, int parent_id)
{
	QListViewItem *item = findItem(QString::number(id),1);
	if ( !item->parent() )
		takeItem(item);
	else
		item->parent()->takeItem(item);
	
	Q_ASSERT(item);

	if ( parent_id == -1 )
		insertItem(item);
	else
		findItem(QString::number(parent_id),1)->insertItem(item);
}

void DualCategoryListView::mergeCategories(int id1, int id2)
{
	CategoryCheckListItem *to_item = (CategoryCheckListItem*)findItem(QString::number(id1),1);
	CategoryCheckListItem *from_item = (CategoryCheckListItem*)findItem(QString::number(id2),1);

	//note that this takes care of any recipes that may be children as well
	QListViewItem *next_sibling;
	for ( QListViewItem *it = from_item->firstChild(); it; it = next_sibling ) {
		next_sibling = it->nextSibling(); //get the sibling before we move the item

		from_item->takeItem(it);
		to_item->insertItem(it);
	}

	removeCategory(id2);
}



AdvancedSearchDialog::AdvancedSearchDialog( QWidget *parent, RecipeDB *db ) : QWidget(parent),
  database(db)
{
	///AUTOMATICALLY GENERATED GUI CODE///
	advancedSearchLayout = new QHBoxLayout( this, 0, 0, "advancedSearchLayout"); 
	
	widgetStack = new QWidgetStack( this, "widgetStack" );
	
	searchPage = new QWidget( widgetStack, "searchPage" );
	searchPageLayout = new QGridLayout( searchPage, 1, 1, 3, 3, "searchPageLayout"); 
	
	searchButtonsLayout = new QHBoxLayout( 0, 0, 6, "searchButtonsLayout"); 
	searchSpacer = new QSpacerItem( 481, 21, QSizePolicy::Expanding, QSizePolicy::Minimum );
	searchButtonsLayout->addItem( searchSpacer );
	
	searchButton = new QPushButton( searchPage, "searchButton" );
	searchButtonsLayout->addWidget( searchButton );
	
	searchPageLayout->addMultiCellLayout( searchButtonsLayout, 2, 2, 0, 2 );
	
	categoriesBox = new QGroupBox( searchPage, "categoriesBox" );
	categoriesBox->setAlignment( int( QGroupBox::AlignTop ) );
	categoriesBox->setCheckable( TRUE );
	categoriesBox->setChecked( FALSE );
	categoriesBox->setColumnLayout(0, Qt::Vertical );
	categoriesBox->layout()->setSpacing( 6 );
	categoriesBox->layout()->setMargin( 11 );
	categoriesBoxLayout = new QHBoxLayout( categoriesBox->layout() );
	categoriesBoxLayout->setAlignment( Qt::AlignTop );
	
	categoriesFrame = new QFrame( categoriesBox, "categoriesFrame" );
	categoriesFrame->setFrameShape( QFrame::StyledPanel );
	categoriesFrame->setFrameShadow( QFrame::Raised );
	categoriesFrame->setLineWidth( 0 );
	categoriesFrameLayout = new QGridLayout( categoriesFrame, 1, 1, 0, 0, "categoriesFrameLayout"); 
	catSpacer = new QSpacerItem( 158, 21, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
	categoriesFrameLayout->addItem( catSpacer, 3, 2 );
	
	catTypeComboBox = new QComboBox( FALSE, categoriesFrame, "catTypeComboBox" );
	catTypeComboBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 1, 0, catTypeComboBox->sizePolicy().hasHeightForWidth() ) );
	
	categoriesFrameLayout->addMultiCellWidget( catTypeComboBox, 0, 0, 0, 2 );
	
	catListView = new DualCategoryListView( categoriesFrame, database );
	
	categoriesFrameLayout->addMultiCellWidget( catListView, 1, 1, 0, 2 );
	
	catSelectAllButton = new QPushButton( categoriesFrame, "catSelectAllButton" );
	
	categoriesFrameLayout->addMultiCellWidget( catSelectAllButton, 2, 3, 0, 0 );
	
	catUnselectAllButton = new QPushButton( categoriesFrame, "catUnselectAllButton" );
	
	categoriesFrameLayout->addMultiCellWidget( catUnselectAllButton, 2, 3, 1, 1 );
	categoriesBoxLayout->addWidget( categoriesFrame );
	
	searchPageLayout->addWidget( categoriesBox, 0, 0 );
	
	authorsBox = new QGroupBox( searchPage, "authorsBox" );
	authorsBox->setAlignment( int( QGroupBox::AlignTop ) );
	authorsBox->setCheckable( TRUE );
	authorsBox->setChecked( FALSE );
	authorsBox->setColumnLayout(0, Qt::Vertical );
	authorsBox->layout()->setSpacing( 6 );
	authorsBox->layout()->setMargin( 11 );
	authorsBoxLayout = new QHBoxLayout( authorsBox->layout() );
	authorsBoxLayout->setAlignment( Qt::AlignTop );
	
	authorsFrame = new QFrame( authorsBox, "authorsFrame" );
	authorsFrame->setFrameShape( QFrame::StyledPanel );
	authorsFrame->setFrameShadow( QFrame::Raised );
	authorsFrame->setLineWidth( 0 );
	authorsFrameLayout = new QGridLayout( authorsFrame, 1, 1, 0, 0, "authorsFrameLayout"); 
	
	authorTypeComboBox = new QComboBox( FALSE, authorsFrame, "authorTypeComboBox" );
	authorTypeComboBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 1, 0, authorTypeComboBox->sizePolicy().hasHeightForWidth() ) );
	
	authorsFrameLayout->addMultiCellWidget( authorTypeComboBox, 0, 0, 0, 2 );
	
	authorUnselectAllButton = new QPushButton( authorsFrame, "authorUnselectAllButton" );
	
	authorsFrameLayout->addWidget( authorUnselectAllButton, 2, 1 );
	authorSpacer = new QSpacerItem( 27, 21, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
	authorsFrameLayout->addItem( authorSpacer, 2, 2 );
	
	authorSelectAllButton = new QPushButton( authorsFrame, "authorSelectAllButton" );
	
	authorsFrameLayout->addWidget( authorSelectAllButton, 2, 0 );
	
	authorListView = new DualAuthorListView( authorsFrame, database );
	
	authorsFrameLayout->addMultiCellWidget( authorListView, 1, 1, 0, 2 );
	authorsBoxLayout->addWidget( authorsFrame );
	
	searchPageLayout->addMultiCellWidget( authorsBox, 0, 0, 1, 2 );
	
	QVBox *serv_prep_box = new QVBox(searchPage);

	servingsBox = new QGroupBox( serv_prep_box, "servingsBox" );
	servingsBox->setAlignment( int( QGroupBox::AlignVCenter ) );
	servingsBox->setCheckable( TRUE );
	servingsBox->setChecked( FALSE );
	servingsBox->setColumnLayout(0, Qt::Vertical );
	servingsBox->layout()->setSpacing( 6 );
	servingsBox->layout()->setMargin( 11 );
	servingsBoxLayout = new QHBoxLayout( servingsBox->layout() );
	servingsBoxLayout->setAlignment( Qt::AlignTop );
	
	servingsFrame = new QFrame( servingsBox, "servingsFrame" );
	servingsFrame->setFrameShape( QFrame::StyledPanel );
	servingsFrame->setFrameShadow( QFrame::Raised );
	servingsFrame->setLineWidth( 0 );
	servingsFrameLayout = new QHBoxLayout( servingsFrame, 0, 0, "servingsFrameLayout"); 
	
	servingsComboBox = new QComboBox( FALSE, servingsFrame, "servingsComboBox" );
	servingsComboBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 1, 0, servingsComboBox->sizePolicy().hasHeightForWidth() ) );
	servingsFrameLayout->addWidget( servingsComboBox );
	
	servingsSpinBox = new QSpinBox( servingsFrame, "servingsSpinBox" );
	servingsSpinBox->setMinValue( 1 );
	servingsFrameLayout->addWidget( servingsSpinBox );
	servingsBoxLayout->addWidget( servingsFrame );
	
	//searchPageLayout->addWidget( servingsBox, 1, 2 );

	prepTimeBox = new QGroupBox( serv_prep_box, "prepTimeBox" );
	prepTimeBox->setAlignment( int( QGroupBox::AlignVCenter ) );
	prepTimeBox->setCheckable( TRUE );
	prepTimeBox->setChecked( FALSE );
	prepTimeBox->setColumnLayout(0, Qt::Vertical );
	prepTimeBox->layout()->setSpacing( 6 );
	prepTimeBox->layout()->setMargin( 11 );
	prepTimeBoxLayout = new QHBoxLayout( prepTimeBox->layout() );
	prepTimeBoxLayout->setAlignment( Qt::AlignTop );
	
	prepTimeFrame = new QFrame( prepTimeBox, "prepTimeFrame" );
	prepTimeFrame->setFrameShape( QFrame::StyledPanel );
	prepTimeFrame->setFrameShadow( QFrame::Raised );
	prepTimeFrame->setLineWidth( 0 );
	prepTimeFrameLayout = new QHBoxLayout( prepTimeFrame, 0, 0, "prepTimeFrameLayout"); 
	
	prepTimeComboBox = new QComboBox( FALSE, prepTimeFrame, "prepTimeComboBox" );
	prepTimeComboBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 1, 0, prepTimeComboBox->sizePolicy().hasHeightForWidth() ) );
	prepTimeFrameLayout->addWidget( prepTimeComboBox );
	
	prepTimeEdit = new QTimeEdit( prepTimeFrame, "prepTimeEdit" );
	prepTimeEdit->setMinValue( QTime(0,0) );
	prepTimeEdit->setDisplay( QTimeEdit::Hours | QTimeEdit::Minutes );
	prepTimeFrameLayout->addWidget( prepTimeEdit );
	prepTimeBoxLayout->addWidget( prepTimeFrame );
	
	searchPageLayout->addWidget( serv_prep_box, 1, 2 );
	
	ingredientsBox = new QGroupBox( searchPage, "ingredientsBox" );
	ingredientsBox->setAlignment( int( QGroupBox::AlignTop ) );
	ingredientsBox->setCheckable( TRUE );
	ingredientsBox->setChecked( FALSE );
	ingredientsBox->setColumnLayout(0, Qt::Vertical );
	ingredientsBox->layout()->setSpacing( 6 );
	ingredientsBox->layout()->setMargin( 11 );
	ingredientsBoxLayout = new QHBoxLayout( ingredientsBox->layout() );
	ingredientsBoxLayout->setAlignment( Qt::AlignTop );
	
	ingredientsFrame = new QFrame( ingredientsBox, "ingredientsFrame" );
	ingredientsFrame->setFrameShape( QFrame::StyledPanel );
	ingredientsFrame->setFrameShadow( QFrame::Raised );
	ingredientsFrame->setLineWidth( 0 );
	ingredientsFrameLayout = new QGridLayout( ingredientsFrame, 1, 1, 0, 0, "ingredientsFrameLayout"); 
	
	ingListView = new DualIngredientListView( ingredientsFrame, database );

	ingredientsFrameLayout->addMultiCellWidget( ingListView, 1, 1, 0, 3 );
	
	ingTypeComboBox = new QComboBox( FALSE, ingredientsFrame, "ingTypeComboBox" );
	ingTypeComboBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 1, 0, ingTypeComboBox->sizePolicy().hasHeightForWidth() ) );
	
	ingredientsFrameLayout->addMultiCellWidget( ingTypeComboBox, 0, 0, 0, 2 );
	ingSpacer = new QSpacerItem( 184, 21, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
	ingredientsFrameLayout->addItem( ingSpacer, 2, 2 );
	
	ingSelectAllButton = new QPushButton( ingredientsFrame, "ingSelectAllButton" );
	
	ingredientsFrameLayout->addWidget( ingSelectAllButton, 2, 0 );
	
	ingUnselectAllButton = new QPushButton( ingredientsFrame, "ingUnselectAllButton" );
	
	ingredientsFrameLayout->addWidget( ingUnselectAllButton, 2, 1 );
	ingredientsBoxLayout->addWidget( ingredientsFrame );
	
	searchPageLayout->addMultiCellWidget( ingredientsBox, 1, 1, 0, 1 );
	widgetStack->addWidget( searchPage, 0 );
	
	resultPage = new QWidget( widgetStack, "resultPage" );
	resultPageLayout = new QVBoxLayout( resultPage, 11, 6, "resultPageLayout"); 
	
	resultBox = new QGroupBox( resultPage, "resultBox" );
	resultBox->setColumnLayout(0, Qt::Vertical );
	resultBox->layout()->setSpacing( 6 );
	resultBox->layout()->setMargin( 11 );
	resultBoxLayout = new QHBoxLayout( resultBox->layout() );
	resultBoxLayout->setAlignment( Qt::AlignTop );
	
	resultsListView = new KListView( resultBox, "resultsListView" );
	resultsListView->addColumn( i18n( "Recipe" ) );

	KConfig *config = KGlobal::config();
	config->setGroup( "Advanced" );
	bool show_id = config->readBoolEntry("ShowID",false);
	resultsListView->addColumn( i18n("Id"), show_id ? -1 : 0 );

	resultsListView->setAllColumnsShowFocus( TRUE );
	resultBoxLayout->addWidget( resultsListView );
	resultPageLayout->addWidget( resultBox );
	
	resultsButtonsLayout = new QHBoxLayout( 0, 0, 6, "resultsButtonsLayout"); 
	resultsSpacer = new QSpacerItem( 521, 31, QSizePolicy::Expanding, QSizePolicy::Minimum );
	resultsButtonsLayout->addItem( resultsSpacer );
	
	backButton = new QPushButton( resultPage, "backButton" );
	resultsButtonsLayout->addWidget( backButton );
	
	openButton = new QPushButton( resultPage, "openButton" );
	resultsButtonsLayout->addWidget( openButton );
	resultPageLayout->addLayout( resultsButtonsLayout );
	widgetStack->addWidget( resultPage, 1 );
	advancedSearchLayout->addWidget( widgetStack );
	languageChange();
	clearWState( WState_Polished );
	///END OF AUTOMATICALLY GENERATED GUI CODE///
	
	// ### At present, this is the only way I can get this dialog's size down
	//     Should I put each element in its own tab to avoid squeezing them
	//     into this one page???
	authorListView->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
	catListView->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
	ingListView->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
	
	KIconLoader *il=new KIconLoader;
	openButton->setIconSet(il->loadIcon("ok", KIcon::NoGroup,16));
	backButton->setIconSet(il->loadIcon("back", KIcon::NoGroup,16));
	searchButton->setIconSet(il->loadIcon("find", KIcon::NoGroup,16));
	

	authorsFrame->setEnabled(false);
	categoriesFrame->setEnabled(false);
	ingredientsFrame->setEnabled(false);
	servingsFrame->setEnabled(false);
	
	RecipeActionsHandler *actionHandler = new RecipeActionsHandler(resultsListView,database,RecipeActionsHandler::Open|RecipeActionsHandler::Edit|RecipeActionsHandler::SaveAs);

	connect( searchButton, SIGNAL(clicked()), SLOT(search()) );
	connect( backButton, SIGNAL(clicked()), SLOT(back()) );
	connect( authorsBox, SIGNAL(toggled(bool)), authorsFrame, SLOT(setEnabled(bool)) );
	connect( categoriesBox, SIGNAL(toggled(bool)), categoriesFrame, SLOT(setEnabled(bool)) );
	connect( ingredientsBox, SIGNAL(toggled(bool)), ingredientsFrame, SLOT(setEnabled(bool)) );
	connect( servingsBox, SIGNAL(toggled(bool)), servingsFrame, SLOT(setEnabled(bool)) );
	connect( authorTypeComboBox, SIGNAL(activated(int)), authorListView, SLOT(change(int)) );
	connect( ingTypeComboBox, SIGNAL(activated(int)), ingListView, SLOT(change(int)) );
	connect( catTypeComboBox, SIGNAL(activated(int)), catListView, SLOT(change(int)) );
	connect( authorSelectAllButton, SIGNAL(clicked()), SLOT(selectAllAuthors()) );
	connect( catSelectAllButton, SIGNAL(clicked()), SLOT(selectAllCategories()) );
	connect( ingSelectAllButton, SIGNAL(clicked()), SLOT(selectAllIngredients()) );
	connect( authorUnselectAllButton, SIGNAL(clicked()), SLOT(unselectAllAuthors()) );
	connect( catUnselectAllButton, SIGNAL(clicked()), SLOT(unselectAllCategories()) );
	connect( ingUnselectAllButton, SIGNAL(clicked()), SLOT(unselectAllIngredients()) );
	connect( actionHandler, SIGNAL(recipeSelected(int,int)),SIGNAL(recipeSelected(int,int)) );
	
	connect( openButton, SIGNAL(clicked()), actionHandler, SLOT(open()) );

	reload();
}

AdvancedSearchDialog::~AdvancedSearchDialog()
{
}

void AdvancedSearchDialog::languageChange()
{
	searchButton->setText( i18n( "Search" ) );
	categoriesBox->setTitle( i18n( "Categories" ) );
	catTypeComboBox->clear();
	catTypeComboBox->insertItem( i18n( "Belong to:" ) );
	catTypeComboBox->insertItem( i18n( "Do not belong to:" ) );
	QWhatsThis::add( catTypeComboBox, i18n( "Here you can search for recipes based on whether or not a recipe belongs to certain categories.  Note that you can use both the inclusive and exclusive searches simultaneously." ) );
	catSelectAllButton->setText( i18n( "Select All" ) );
	catUnselectAllButton->setText( i18n( "Unselect All" ) );
	authorsBox->setTitle( i18n( "Authors" ) );
	authorTypeComboBox->clear();
	authorTypeComboBox->insertItem( i18n( "By:" ) );
	authorTypeComboBox->insertItem( i18n( "Not by:" ) );
	QWhatsThis::add( authorTypeComboBox, i18n( "Here you can search for recipes based on whether or not a recipe is by certain authors.  Note that you can use both the inclusive and exclusive searches simultaneously." ) );
	authorUnselectAllButton->setText( i18n( "Unselect All" ) );
	authorSelectAllButton->setText( i18n( "Select All" ) );
	servingsBox->setTitle( i18n( "Servings" ) );
	servingsComboBox->clear();
	servingsComboBox->insertItem( i18n( "Serves at least:" ) );
	servingsComboBox->insertItem( i18n( "Serves at most:" ) );
	servingsComboBox->insertItem( i18n( "Serves exactly:" ) );
	servingsComboBox->insertItem( i18n( "Serves about:" ) );
	prepTimeBox->setTitle( i18n( "Preparation Time" ) );
	prepTimeComboBox->clear();
	prepTimeComboBox->insertItem( i18n( "Ready in at least:" ) );
	prepTimeComboBox->insertItem( i18n( "Ready in at most:" ) );
	prepTimeComboBox->insertItem( i18n( "Ready in exactly:" ) );
	prepTimeComboBox->insertItem( i18n( "Ready in about:" ) );
	ingredientsBox->setTitle( i18n( "Ingredients" ) );
	ingTypeComboBox->clear();
	ingTypeComboBox->insertItem( i18n( "Use:" ) );
	ingTypeComboBox->insertItem( i18n( "Do not use:" ) );
	QWhatsThis::add( ingTypeComboBox, i18n( "Here you can search for recipes based on whether or not a recipe uses or does not use certain ingredients.  Note that you can use both the inclusive and exclusive searches simultaneously." ) );
	ingSelectAllButton->setText( i18n( "Select All" ) );
	ingUnselectAllButton->setText( i18n( "Unselect All" ) );
	resultBox->setTitle( i18n( "Search Results" ) );
	resultsListView->header()->setLabel( 0, i18n( "Recipe" ) );
	resultsListView->header()->setLabel( 1, i18n( "Id" ) );
	backButton->setText( i18n( "Back" ) );
	openButton->setText( i18n( "Open" ) );
}

void AdvancedSearchDialog::selectAllAuthors()
{
	for ( QCheckListItem *qlv_it = static_cast<QCheckListItem*>(authorListView->firstChild()); qlv_it ; qlv_it = static_cast<QCheckListItem*>(qlv_it->nextSibling()) )
	{
		if ( qlv_it->isEnabled() )
			qlv_it->setOn(true);
	}
}

void AdvancedSearchDialog::selectAllCategories()
{
	QCheckListItem* current_item;
	QListViewItemIterator it( catListView );
	while ( it.current() ) {
		current_item = (QCheckListItem*)it.current();

		if ( current_item->isEnabled() )
			current_item->setOn(true);

		++it;
	}
}

void AdvancedSearchDialog::selectAllIngredients()
{
	for ( QCheckListItem *qlv_it = static_cast<QCheckListItem*>(ingListView->firstChild()); qlv_it ; qlv_it = static_cast<QCheckListItem*>(qlv_it->nextSibling()) )
	{
		if ( qlv_it->isEnabled() )
			qlv_it->setOn(true);
	}
}

void AdvancedSearchDialog::unselectAllAuthors()
{
	for ( QCheckListItem *qlv_it = static_cast<QCheckListItem*>(authorListView->firstChild()); qlv_it ; qlv_it = static_cast<QCheckListItem*>(qlv_it->nextSibling()) )
	{
		if ( qlv_it->isEnabled() )
			qlv_it->setOn(false);
	}
}

void AdvancedSearchDialog::unselectAllCategories()
{
	QCheckListItem* current_item;
	QListViewItemIterator it( catListView );
	while ( it.current() ) {
		current_item = (QCheckListItem*)it.current();

		if ( current_item->isEnabled() )
			current_item->setOn(false);

		++it;
	}
}

void AdvancedSearchDialog::unselectAllIngredients()
{
	for ( QCheckListItem *qlv_it = static_cast<QCheckListItem*>(ingListView->firstChild()); qlv_it ; qlv_it = static_cast<QCheckListItem*>(qlv_it->nextSibling()) )
	{
		if ( qlv_it->isEnabled() )
			qlv_it->setOn(false);
	}
}

void AdvancedSearchDialog::reload()
{
	authorListView->reload();
	catListView->reload();
	ingListView->reload();
}

void AdvancedSearchDialog::search()
{
	KApplication::setOverrideCursor( KCursor::waitCursor() );
	
	//we'll load all the recipes and whittle down the list based on constraints
	RecipeList allRecipes; database->loadRecipeDetails( &allRecipes, true, true, false, true );
	
	//narrow down by servings
	if ( servingsBox->isChecked() )
	{
		int servings = servingsSpinBox->value();
		for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it )
		{
			switch ( servingsComboBox->currentItem() )
			{
			//at least
			case 0: if ( (*it).persons < servings ){ it = allRecipes.remove( it ); it--; }
				break;
			//at most
			case 1: if ( (*it).persons > servings ){ it = allRecipes.remove( it ); it--; }
				break;
			//exactly
			case 2: if ( (*it).persons != servings ){ it = allRecipes.remove( it ); it--; }
				break;
			//about
			case 3: if ( QABS((*it).persons-servings) > 5 ){ it = allRecipes.remove( it ); it--; }
				break;
			}
		}
	}

	//narrow down by prep time
	if ( prepTimeBox->isChecked() )
	{
		QTime time = prepTimeEdit->time();
		for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it )
		{
			switch ( prepTimeComboBox->currentItem() )
			{
			//at least
			case 0: if ( (*it).prepTime > time ){ it = allRecipes.remove( it ); it--; }
				break;
			//at most
			case 1: if ( (*it).prepTime < time ){ it = allRecipes.remove( it ); it--; }
				break;
			//exactly
			case 2: if ( (*it).prepTime != time ){ it = allRecipes.remove( it ); it--; }
				break;
			//about
			case 3:
			{
				int rec_minutes = (*it).prepTime.minute() + (*it).prepTime.hour()*60;
				int test_minutes = time.minute() + time.hour()*60;
				
				//TODO: have a configurable 'about'.  It tests within 15 minutes for now.
				if ( QABS(test_minutes-rec_minutes) > 15 ){ it = allRecipes.remove( it ); it--; }
				break;
			}
			}
		}
	}
	
	//narrow down by authors
	if ( authorsBox->isChecked() )
	{
		authorListView->updateMap( authorTypeComboBox->currentItem() ); //the other won't have been updated yet

		//positive search
		for ( QMap<QCheckListItem*,bool>::Iterator map_it = authorListView->positiveMap.begin(); map_it != authorListView->positiveMap.end(); ++map_it )
		{
			if ( map_it.data() == true )
			{
				for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it )
				{
					Element i; i.id = map_it.key()->text(1).toInt();
					if ( (*it).authorList.find( i ) == (*it).authorList.end() )
					{
						it = allRecipes.remove( it ); it--;
					}
				}
			}
		}

		//negative search
		for ( QMap<QCheckListItem*,bool>::Iterator map_it = authorListView->negativeMap.begin(); map_it != authorListView->negativeMap.end(); ++map_it )
		{
			if ( map_it.data() == true )
			{
				for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it )
				{
					Element i; i.id = map_it.key()->text(1).toInt();
					if ( (*it).authorList.find( i ) != (*it).authorList.end() )
					{
						it = allRecipes.remove( it ); it--;
					}
				}
			}
		}
	}
	
	//narrow down by categories
	if ( categoriesBox->isChecked() )
	{
		catListView->updateMap( catTypeComboBox->currentItem() ); //the other won't have been updated yet

		//positive search
		for ( QMap<QCheckListItem*,bool>::Iterator map_it = catListView->positiveMap.begin(); map_it != catListView->positiveMap.end(); ++map_it )
		{
			if ( map_it.data() == true )
			{
				for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it )
				{
					Element i; i.id = map_it.key()->text(1).toInt();
					if ( (*it).categoryList.find( i ) == (*it).categoryList.end() )
					{
						it = allRecipes.remove( it ); it--;
					}
				}
			}
		}

		//negative search
		for ( QMap<QCheckListItem*,bool>::Iterator map_it = catListView->negativeMap.begin(); map_it != catListView->negativeMap.end(); ++map_it )
		{
			if ( map_it.data() == true )
			{
				for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it )
				{
					Element i; i.id = map_it.key()->text(1).toInt();
					if ( (*it).categoryList.find( i ) != (*it).categoryList.end() )
					{
						it = allRecipes.remove( it ); it--;
					}
				}
			}
		}
	}

	//narrow down by ingredients
	if ( ingredientsBox->isChecked() )
	{
		ingListView->updateMap( ingTypeComboBox->currentItem() ); //the other won't have been updated yet

		//positive search
		for ( QMap<QCheckListItem*,bool>::Iterator map_it = ingListView->positiveMap.begin(); map_it != ingListView->positiveMap.end(); ++map_it )
		{
			if ( map_it.data() == true )
			{
				for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it )
 				{
					int id = map_it.key()->text(1).toInt();
					if ( (*it).ingList.find( id ) == -1 )
					{
						it = allRecipes.remove( it ); it--;
					}
				}
			}
		}

		//negative search
		for ( QMap<QCheckListItem*,bool>::Iterator map_it = ingListView->negativeMap.begin(); map_it != ingListView->negativeMap.end(); ++map_it )
		{
			if ( map_it.data() == true )
			{
				for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it )
				{
					int id = map_it.key()->text(1).toInt();
					if ( (*it).ingList.find( id ) != -1 )
					{
						it = allRecipes.remove( it ); it--;
					}
				}
			}
		}
	}
	
	//now display the recipes left
	resultsListView->clear();
	for ( RecipeList::const_iterator it = allRecipes.begin(); it != allRecipes.end(); ++it )
	{
		(void)new RecipeListItem(resultsListView,*it);
	}

	widgetStack->raiseWidget( resultPage );
	KApplication::restoreOverrideCursor();
}

void AdvancedSearchDialog::back()
{
	widgetStack->raiseWidget( searchPage );
}

#include "advancedsearchdialog.moc"
