/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
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
class QWidgetStack;
class QWidget;
class QGroupBox;
class QComboBox;
class QListView;
class QListViewItem;
class QPushButton;
class QCheckBox;
class QCheckListItem;
class QSpinBox;
class QFrame;
class QTimeEdit;

class KListView;

class RecipeDB;


class DualAuthorListView : public AuthorListView
{
Q_OBJECT

public:
	DualAuthorListView(QWidget *parent, RecipeDB *db);

	void reload();

	QMap<QCheckListItem*,bool> positiveMap;
	QMap<QCheckListItem*,bool> negativeMap;

public slots:
	void change(int index);
	void updateMap( int index );
	
protected:
	virtual void createAuthor(const Element &ing);
	virtual void removeAuthor(int id);
	
private:
	int last_state;
};

class DualCategoryListView : public CategoryListView
{
Q_OBJECT

public:
	DualCategoryListView(QWidget *parent, RecipeDB *db);

	void reload();

	QMap<QCheckListItem*,bool> positiveMap;
	QMap<QCheckListItem*,bool> negativeMap;

public slots:
	void change(int index);
	void updateMap( int index );
	
protected:
	virtual void createCategory(const Element &,int);
	virtual void removeCategory(int id);
	virtual void modifyCategory(const Element &category);
	virtual void modifyCategory(int id, int parent_id);
	virtual void mergeCategories(int id1, int id2);
	
private:
	int last_state;
};

class DualIngredientListView : public IngredientListView
{
Q_OBJECT

public:
	DualIngredientListView(QWidget *parent, RecipeDB *db);

	void reload();

	QMap<QCheckListItem*,bool> positiveMap;
	QMap<QCheckListItem*,bool> negativeMap;

public slots:
	void change(int index);
	void updateMap( int index );
	
protected:
	virtual void createIngredient(const Element &ing);
	virtual void removeIngredient(int id);
	
private:
	int last_state;
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
	QWidgetStack* widgetStack;
	QWidget* searchPage;
	QPushButton* searchButton;
	QGroupBox* categoriesBox;
	QFrame* categoriesFrame;
	QComboBox* catTypeComboBox;
	DualCategoryListView* catListView;
	QPushButton* catSelectAllButton;
	QPushButton* catUnselectAllButton;
	QGroupBox* authorsBox;
	QFrame* authorsFrame;
	QComboBox* authorTypeComboBox;
	QPushButton* authorUnselectAllButton;
	QPushButton* authorSelectAllButton;
	DualAuthorListView* authorListView;
	QGroupBox* servingsBox;
	QFrame* servingsFrame;
	QComboBox* servingsComboBox;
	QSpinBox* servingsSpinBox;
	QGroupBox* ingredientsBox;
	QFrame* ingredientsFrame;
	DualIngredientListView* ingListView;
	QComboBox* ingTypeComboBox;
	QPushButton* ingSelectAllButton;
	QPushButton* ingUnselectAllButton;
	QWidget* resultPage;
	QGroupBox* resultBox;
	KListView* resultsListView;
	QPushButton* backButton;
	QPushButton* openButton;
	QTimeEdit* prepTimeEdit;
	QFrame* prepTimeFrame;
	QGroupBox* prepTimeBox;
	QComboBox* prepTimeComboBox;

	QHBoxLayout* advancedSearchLayout;
	QGridLayout* searchPageLayout;
	QHBoxLayout* searchButtonsLayout;
	QSpacerItem* searchSpacer;
	QHBoxLayout* categoriesBoxLayout;
	QGridLayout* categoriesFrameLayout;
	QSpacerItem* catSpacer;
	QHBoxLayout* authorsBoxLayout;
	QGridLayout* authorsFrameLayout;
	QSpacerItem* authorSpacer;
	QHBoxLayout* servingsBoxLayout;
	QHBoxLayout* servingsFrameLayout;
	QHBoxLayout* prepTimeFrameLayout;
	QHBoxLayout* prepTimeBoxLayout;
	QHBoxLayout* ingredientsBoxLayout;
	QGridLayout* ingredientsFrameLayout;
	QSpacerItem* ingSpacer;
	QVBoxLayout* resultPageLayout;
	QHBoxLayout* resultBoxLayout;
	QHBoxLayout* resultsButtonsLayout;
	QSpacerItem* resultsSpacer;

	RecipeDB *database;

signals:
	void recipeSelected(int,int);
	
private slots:
	void search();
	void back();

	void selectAllAuthors();
	void selectAllCategories();
	void selectAllIngredients();
	void unselectAllAuthors();
	void unselectAllCategories();
	void unselectAllIngredients();
};

#endif //ADVANCEDSEARCHDIALOG_H
