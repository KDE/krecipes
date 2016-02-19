/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "dishinput.h"

#include "dishtitle.h"
#include "widgets/propertylistview.h"
#include "widgets/categorylistview.h"
#include "widgets/dblistviewbase.h"
#include "backends/recipedb.h"

#include <KLocale>
#include <KNumInput>
#include <KVBox>

#include <q3header.h>
#include <Q3ValueList>
#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QGroupBox>


DishInput::DishInput( QWidget* parent, RecipeDB *db, const QString &title ) : QWidget( parent ),
	database(db)
{
	// Initialize internal variables
	categoryFiltering = false;

	// Design the widget
	QVBoxLayout *layout = new QVBoxLayout( this );
	layout->setSpacing( 10 );

	//Horizontal Box to hold the K3ListView's
	listBox = new QGroupBox;
	QHBoxLayout *listBoxLayout = new QHBoxLayout;
	listBox->setTitle( i18nc( "@label:listbox", "Dish Characteristics" ) );
	listBox->setLayout( listBoxLayout );
	layout->addWidget( listBox );

	// Dish id
	dishTitle = new DishTitle( listBox, title );
	dishTitle->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding );
	listBoxLayout->addWidget( dishTitle );

	//Categories list
	categoriesBox = new KVBox;
	categoriesEnabledBox = new QCheckBox( categoriesBox );
	categoriesEnabledBox->setText( i18nc( "@option:check", "Enable Category Filtering" ) );

	categoriesView = new CategoryCheckListView( categoriesBox, database, false );
	categoriesView->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
	categoriesView->setEnabled( false ); // Disable it by default

	listBoxLayout->addWidget( categoriesBox );

	//Constraints list
	constraintsView = new PropertyConstraintListView( listBox, database );
	constraintsView->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
	constraintsView->reload();
	listBoxLayout->addWidget( constraintsView );

	// KDoubleInput based edit boxes
	constraintsEditBox1 = new KDoubleNumInput( constraintsView->viewport() );
	constraintsView->addChild( constraintsEditBox1 );
	constraintsEditBox1->hide();
	constraintsEditBox2 = new KDoubleNumInput( constraintsView->viewport() );
	constraintsView->addChild( constraintsEditBox2 );
	constraintsEditBox2->hide();


	// Connect Signals & Slots
	connect( constraintsView, SIGNAL( executed( Q3ListViewItem* ) ), this, SLOT( insertConstraintsEditBoxes( Q3ListViewItem* ) ) );
	connect( constraintsView, SIGNAL( selectionChanged() ), this, SLOT( hideConstraintInputs() ) );
	connect( constraintsEditBox1, SIGNAL( valueChanged( double ) ), this, SLOT( setMinValue( double ) ) );
	connect( constraintsEditBox2, SIGNAL( valueChanged( double ) ), this, SLOT( setMaxValue( double ) ) );
	connect( categoriesEnabledBox, SIGNAL( toggled( bool ) ), this, SLOT( enableCategories( bool ) ) );
}

DishInput::~DishInput()
{}

void DishInput::clear()
{
	Q3ListViewItemIterator it( categoriesView );
	while ( it.current() ) {
		if (it.current()->rtti() == CATEGORYCHECKLISTITEM_RTTI) {
			CategoryCheckListItem * item = ( CategoryCheckListItem* ) it.current();
			item->setOn( false );
		}
		++it;
	}

	constraintsView->reload();
	categoriesEnabledBox->setChecked( false );
}

void DishInput::enableCategories( bool enable )
{
	categoriesView->setEnabled( enable );
	categoryFiltering = enable;
}

bool DishInput::isCategoryFilteringEnabled( void ) const
{
	return categoryFiltering;
}

void DishInput::reload( ReloadFlags flag )
{
	constraintsView->reload();
	categoriesView->reload(flag);
}

void DishInput::insertConstraintsEditBoxes( Q3ListViewItem* it )
{
	QRect r;

	// Constraints Box1
	r = constraintsView->header() ->sectionRect( 2 ); //start at the section 2 header
	r.translate( 0, constraintsView->itemRect( it ).y() ); //Move down to the item, note that its height is same as header's right now.

	r.setHeight( it->height() ); // Set the item's height
	r.setWidth( constraintsView->header() ->sectionRect( 2 ).width() ); // and width
	constraintsEditBox1->setGeometry( r );


	//Constraints Box2
	r = constraintsView->header() ->sectionRect( 3 ); //start at the section 3 header
	r.translate( 0, constraintsView->itemRect( it ).y() ); //Move down to the item

	r.setHeight( it->height() ); // Set the item's height
	r.setWidth( constraintsView->header() ->sectionRect( 3 ).width() ); // and width
	constraintsEditBox2->setGeometry( r );

	// Set the values from the item
	constraintsEditBox1->setValue( ( ( ConstraintsListItem* ) it ) ->minVal() );
	constraintsEditBox2->setValue( ( ( ConstraintsListItem* ) it ) ->maxVal() );

	// Show Boxes
	constraintsEditBox1->show();
	constraintsEditBox2->show();
}

void DishInput::hideConstraintInputs()
{
	constraintsEditBox1->hide();
	constraintsEditBox2->hide();
}

void DishInput::loadConstraints( ConstraintList *constraints ) const
{
	constraints->clear();
	Constraint constraint;
	for ( ConstraintsListItem * it = ( ConstraintsListItem* ) ( constraintsView->firstChild() );it;it = ( ConstraintsListItem* ) ( it->nextSibling() ) ) {
		constraint.id = it->propertyId();
		constraint.min = it->minVal();
		constraint.max = it->maxVal();
		constraint.enabled = it->isOn();
		constraints->append( constraint );
	}
}

void DishInput::loadEnabledCategories( ElementList* categories )
{
	categories->clear();

	// Only load those that are checked, unless filtering is disabled
	if ( !categoriesView->isEnabled() ) {
		database->loadCategories(categories);
	}
	else {
		*categories = categoriesView->selections();
	}
}

void DishInput::setMinValue( double minValue )
{
	ConstraintsListItem *it = ( ConstraintsListItem* ) ( constraintsView->selectedItem() ); // Find selected property

	if ( it )
		it->setMin( minValue );
}

void DishInput::setMaxValue( double maxValue )
{
	ConstraintsListItem *it = ( ConstraintsListItem* ) ( constraintsView->selectedItem() ); // Find selected property

	if ( it )
		it->setMax( maxValue );
}



