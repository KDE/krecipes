/***************************************************************************
*   Copyright (C) 2003-2004 by                                            *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef ADVANCEDSEARCHDIALOG_H
#define ADVANCEDSEARCHDIALOG_H

#include <qmap.h>
#include <qwidget.h>

#include "widgets/ingredientlistview.h"
#include "widgets/categorylistview.h"
#include "widgets/authorlistview.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QTabWidget;
class QWidget;
class QCheckBox;
class QFrame;
class QComboBox;
class QListViewItem;
class QPushButton;
class QGroupBox;
class QSpinBox;
class QTimeEdit;
class KPushButton;
class KListView;

class RecipeDB;


class DualAuthorListView : public AuthorCheckListView
{
	Q_OBJECT

public:
	DualAuthorListView( QWidget *parent, RecipeDB *db );

	void reload();
	void stateChange( AuthorCheckListItem *, bool );

	QValueList<Element> positiveSelections;
	QValueList<Element> negativeSelections;

public slots:
	void change( int index );

protected:
	virtual void createAuthor( const Element &ing );
	virtual void removeAuthor( int id );

private:
	int last_state;
	bool lock_updates;
};

class DualCategoryListView : public CategoryCheckListView
{
	Q_OBJECT

public:
	DualCategoryListView( QWidget *parent, RecipeDB *db );

	void reload();
	void stateChange( CategoryCheckListItem *, bool );

	QValueList<Element> positiveSelections;
	QValueList<Element> negativeSelections;

public slots:
	void change( int index );

protected:
	virtual void createCategory( const Element &, int );
	virtual void removeCategory( int id );
	virtual void modifyCategory( const Element &category );
	virtual void modifyCategory( int id, int parent_id );
	virtual void mergeCategories( int id1, int id2 );

private:
	int last_state;
	bool lock_updates;
};

class DualIngredientListView : public IngredientCheckListView
{
	Q_OBJECT

public:
	DualIngredientListView( QWidget *parent, RecipeDB *db );

	void reload();
	void stateChange( IngredientCheckListItem *, bool );

	QValueList<Element> positiveSelections;
	QValueList<Element> negativeSelections;

public slots:
	void change( int index );

protected:
	virtual void createIngredient( const Element &ing );
	virtual void removeIngredient( int id );

private:
	int last_state;
	bool lock_updates;
};


class AdvancedSearchDialog : public QWidget
{
	Q_OBJECT

public:
	AdvancedSearchDialog( QWidget *parent, RecipeDB * );
	~AdvancedSearchDialog();

	void reload();
	virtual void languageChange();

private:
	QTabWidget* paramsTabWidget;
	QWidget* ingTab;
	QCheckBox* enableIngCheckBox;
	QFrame* ingredientsFrame;
	QComboBox* ingTypeComboBox;
	DualIngredientListView* ingListView;
	QPushButton* ingSelectAllButton;
	QPushButton* ingUnselectAllButton;
	QWidget* catTab;
	QCheckBox* enableCatCheckBox;
	QFrame* categoriesFrame;
	QComboBox* catTypeComboBox;
	DualCategoryListView* catListView;
	QPushButton* catSelectAllButton;
	QPushButton* catUnselectAllButton;
	QWidget* servPrepTab;
	QGroupBox* servingsBox;
	QCheckBox* enableServingsCheckBox;
	QFrame* servingsFrame;
	QComboBox* servingsComboBox;
	QSpinBox* servingsSpinBox;
	QGroupBox* prepTimeBox;
	QCheckBox* enablePrepTimeCheckBox;
	QFrame* prepFrame;
	QComboBox* prepTimeComboBox;
	QTimeEdit* prepTimeEdit;
	QWidget* authorTab;
	QCheckBox* enableAuthorCheckBox;
	QFrame* authorsFrame;
	QComboBox* authorTypeComboBox;
	DualAuthorListView* authorListView;
	QPushButton* authorSelectAllButton;
	QPushButton* authorUnselectAllButton;
	KPushButton* findButton;
	KPushButton* clearButton;
	KListView* resultsListView;

protected:
	QVBoxLayout* AdvancedSearchDialogLayout;
	QHBoxLayout* layout17;
	QHBoxLayout* ingTabLayout;
	QVBoxLayout* layout18;
	QSpacerItem* spacer6;
	QVBoxLayout* ingredientsFrameLayout;
	QHBoxLayout* catTabLayout;
	QVBoxLayout* layout16;
	QSpacerItem* spacer5;
	QVBoxLayout* categoriesFrameLayout;
	QHBoxLayout* servPrepTabLayout;
	QVBoxLayout* servingsBoxLayout;
	QHBoxLayout* servingsFrameLayout;
	QVBoxLayout* prepTimeBoxLayout;
	QHBoxLayout* prepFrameLayout;
	QHBoxLayout* authorTabLayout;
	QVBoxLayout* layout15;
	QSpacerItem* spacer4;
	QVBoxLayout* authorsFrameLayout;
	QVBoxLayout* layout9;
	QSpacerItem* spacer7;

	RecipeDB *database;

signals:
	void recipeSelected( int, int );

private slots:
	void search();

	void selectAllAuthors();
	void selectAllCategories();
	void selectAllIngredients();
	void unselectAllAuthors();
	void unselectAllCategories();
	void unselectAllIngredients();
};

#endif //ADVANCEDSEARCHDIALOG_H
