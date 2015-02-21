/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2006 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef INGREDIENTMATCHERDIALOG_H
#define INGREDIENTMATCHERDIALOG_H

#include "datablocks/element.h"
#include "datablocks/ingredientlist.h"
#include "datablocks/recipe.h"
#include "widgets/recipelistview.h"
#include "widgets/dblistviewbase.h"

#include <QSplitter>
#include <QLabel>
#include <q3listview.h>
#include <KPushButton>

#include <kstringhandler.h>
#include <klocale.h>
#include <kvbox.h>

class KreListView;
class KIntSpinBox;
class RecipeDB;
class RecipeActionsHandler;
class KAction;

/**
@author Unai Garro
*/

class CustomRecipeListItem : public RecipeListItem
{
public:
	CustomRecipeListItem( Q3ListView* qlv, const Recipe &r, const IngredientList &il ) : RecipeListItem( qlv, r )
	{
		ingredientListStored = new QStringList();
		IngredientList::ConstIterator ili;
		for ( ili = il.begin();ili != il.end();++ili ) {
			if ( (*ili).substitutes.count() > 0 ) {
				QStringList subs;
				subs << ( *ili ).name;
				for ( Ingredient::SubstitutesList::const_iterator it = (*ili).substitutes.begin(); it != (*ili).substitutes.end(); ++it ) {
					subs << (*it).name;
				}
				ingredientListStored->append( subs.join(QString(" %1 ").arg(i18n("OR"))) );
			}
			else
				ingredientListStored->append( ( *ili ).name );
		}

		moveItem( qlv->lastItem() );
	}
	CustomRecipeListItem( Q3ListView* qlv, const Recipe &r ) : RecipeListItem( qlv, r )
	{
		ingredientListStored = 0;

		moveItem( qlv->lastItem() );
	}

	~CustomRecipeListItem( void )
	{
		delete ingredientListStored;
	}

private:
	QStringList *ingredientListStored;

public:
	virtual QString text( int column ) const
	{
		if ( column == 2 && ingredientListStored )
			return ingredientListStored->join ( "," );
		else
			return ( RecipeListItem::text( column ) );
	}
};

class SectionItem: public Q3ListViewItem
{
public:
	SectionItem( Q3ListView* qlv, QString sectionText ) : Q3ListViewItem( qlv, qlv->lastItem() )
	{
		mText = sectionText;
	}

	~SectionItem( void )
	{}
	virtual void paintCell ( QPainter * p, const QColorGroup & cg, int column, int width, int align );

private:
	QString mText;

public:
	virtual QString text( int column ) const
	{
		if ( column == 0 )
			return ( mText );
		else
			return ( QString() );
	}
};
class IngredientMatcherDialog: public QSplitter
{

	Q_OBJECT

public:

	IngredientMatcherDialog( QWidget *parent, RecipeDB* db );
	~IngredientMatcherDialog();
	void reload( ReloadFlags flag = Load );
	RecipeActionsHandler* getActionsHandler() const;
	void addAction( KAction * action );

signals:
	void recipeSelected( int, int );
	void recipeSelected( bool );

private:
	//Private variables
	RecipeDB *database;
	RecipeActionsHandler * actionHandler;

	//Widgets

	KreListView *allIngListView;
	KreListView *ingListView;

	KreListView *recipeListView;
	KHBox *missingBox;
	QLabel *missingNumberLabel;
	KIntSpinBox *missingNumberSpinBox;

	KPushButton *okButton;
	KPushButton *clearButton;
	KPushButton *addButton;
	KPushButton *removeButton;

	IngredientList m_ingredientList;
	QMap<Q3ListViewItem*, IngredientList::iterator> m_item_ing_map;

private slots:
	void findRecipes( void );
	void unselectIngredients();
	void addIngredient();
	void removeIngredient();
	void itemRenamed( Q3ListViewItem*, const QPoint &, int col );

public slots:
	void haveSelectedItems();
};

#endif
