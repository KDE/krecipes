/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*                                                                         *
*   Copyright (C) 2006 Jason Kivlighn (jkivlighn@gmail.com)               *
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

#include <qfontmetrics.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qhbox.h>
#include <qvbox.h>

#include <kstringhandler.h>
#include <klocale.h>

class KreListView;
class KIntSpinBox;
class RecipeDB;
class AmountUnitInput;
class MixedNumber;
class AmountUnitInput;

/**
@author Unai Garro
*/

class CustomRecipeListItem : public RecipeListItem
{
public:
	CustomRecipeListItem( QListView* qlv, const Recipe &r, const IngredientList &il ) : RecipeListItem( qlv, r )
	{
		ingredientListStored = new QStringList();
		IngredientList::ConstIterator ili;
		for ( ili = il.begin();ili != il.end();++ili ) {
			if ( (*ili).substitutes.count() > 0 ) {
				QStringList subs;
				subs << ( *ili ).name;
				for ( QValueList<IngredientData>::const_iterator it = (*ili).substitutes.begin(); it != (*ili).substitutes.end(); ++it ) {
 					subs << (*it).name;
				}
				ingredientListStored->append( subs.join(QString(" %1 ").arg(i18n("OR"))) );
			}
			else
				ingredientListStored->append( ( *ili ).name );
		}

		moveItem( qlv->lastItem() );
	}
	CustomRecipeListItem( QListView* qlv, const Recipe &r ) : RecipeListItem( qlv, r )
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

class SectionItem: public QListViewItem
{
public:
	SectionItem( QListView* qlv, QString sectionText ) : QListViewItem( qlv, qlv->lastItem() )
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
			return ( QString::null );
	}
};
class IngredientMatcherDialog: public QWidget
{

	Q_OBJECT

public:

	IngredientMatcherDialog( QWidget *parent, RecipeDB* db );
	~IngredientMatcherDialog();
	void reload( bool force = true );

signals:
	void recipeSelected( int, int );

private:
	//Private variables
	RecipeDB *database;

	//Widgets

	KreListView *allIngListView;
	KreListView *ingListView;

	KreListView *recipeListView;
	QHBox *missingBox;
	QLabel *missingNumberLabel;
	KIntSpinBox *missingNumberSpinBox;

	QPushButton *okButton;
	QPushButton *clearButton;
	QPushButton *addButton;
	QPushButton *removeButton;

	AmountUnitInput *amountEdit;

	IngredientList m_ingredientList;
	QMap<QListViewItem*, IngredientList::iterator> m_item_ing_map;

private slots:
	void findRecipes( void );
	void unselectIngredients();
	void addIngredient();
	void removeIngredient();
	void itemRenamed( QListViewItem*, const QPoint &, int col );
	void updateItemAmount( const MixedNumber &amount, const Unit &unit );
	void insertIntoListView( QListViewItem *it );
};

#endif
