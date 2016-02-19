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

#ifndef DISHINPUT_H
#define DISHINPUT_H


#include "widgets/dblistviewbase.h"

#include "datablocks/constraintlist.h"
#include "datablocks/elementlist.h"

class DishTitle;
class RecipeDB;
class CategoryCheckListView;
class PropertyConstraintListView;

class KDoubleNumInput;
class KVBox;

class QGroupBox;
class QCheckBox;


class DishInput: public QWidget
{
	Q_OBJECT

public:
	DishInput( QWidget *parent, RecipeDB *database, const QString &title );
	~DishInput();
	// Methods
	bool isCategoryFilteringEnabled( void ) const;
	void loadConstraints( ConstraintList *constraints ) const;
	void loadEnabledCategories( ElementList* categories );
	void reload( ReloadFlags flag = Load );
	void setDishTitle( const QString & text );
	void clear();


private:
	// Variables
	bool categoryFiltering;
	// Widgets
	QGroupBox *listBox;
	DishTitle *dishTitle;
	KVBox *categoriesBox;
	QCheckBox *categoriesEnabledBox;
	CategoryCheckListView *categoriesView;
	PropertyConstraintListView *constraintsView;
	KDoubleNumInput *constraintsEditBox1;
	KDoubleNumInput *constraintsEditBox2;
	RecipeDB *database;

private slots:
	void enableCategories( bool enable );
	void insertConstraintsEditBoxes( Q3ListViewItem* it );
	void hideConstraintInputs();
	void setMinValue( double minValue );
	void setMaxValue( double maxValue );
};

#endif
