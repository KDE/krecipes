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

class RecipeDB;

class AdvancedSearchDialog : public QWidget
{
Q_OBJECT

public:
	AdvancedSearchDialog( QWidget *parent, RecipeDB * );
	~AdvancedSearchDialog();
	
	void reload();
	virtual void languageChange();

private:
	void loadAuthorListView();
	void loadCategoryListView();
	void loadIngredientListView();
	
	QWidgetStack* widgetStack;
	QWidget* searchPage;
	QPushButton* searchButton;
	QGroupBox* categoriesBox;
	QFrame* categoriesFrame;
	QComboBox* catTypeComboBox;
	QListView* catListView;
	QPushButton* catSelectAllButton;
	QPushButton* catUnselectAllButton;
	QGroupBox* authorsBox;
	QFrame* authorsFrame;
	QComboBox* authorTypeComboBox;
	QPushButton* authorUnselectAllButton;
	QPushButton* authorSelectAllButton;
	QListView* authorListView;
	QGroupBox* servingsBox;
	QFrame* servingsFrame;
	QComboBox* servingsComboBox;
	QSpinBox* servingsSpinBox;
	QGroupBox* ingredientsBox;
	QFrame* ingredientsFrame;
	QListView* ingListView;
	QComboBox* ingTypeComboBox;
	QPushButton* ingSelectAllButton;
	QPushButton* ingUnselectAllButton;
	QWidget* resultPage;
	QGroupBox* resultBox;
	QListView* resultsListView;
	QPushButton* backButton;
	QPushButton* openButton;

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
	QHBoxLayout* ingredientsBoxLayout;
	QGridLayout* ingredientsFrameLayout;
	QSpacerItem* ingSpacer;
	QVBoxLayout* resultPageLayout;
	QHBoxLayout* resultBoxLayout;
	QHBoxLayout* resultsButtonsLayout;
	QSpacerItem* resultsSpacer;
	
	QMap<QCheckListItem*,bool> authorPosMap;
	QMap<QCheckListItem*,bool> authorNegMap;
	QMap<QCheckListItem*,bool> categoryPosMap;
	QMap<QCheckListItem*,bool> categoryNegMap;
	QMap<QCheckListItem*,bool> ingredientPosMap;
	QMap<QCheckListItem*,bool> ingredientNegMap;
	
	int authorLast;
	int categoryLast;
	int ingredientLast;
	
	RecipeDB *database;

signals:
	void recipeSelected(int,int);
	
private slots:
	void search();
	void back();
	void open();
	
	void selectAllAuthors();
	void selectAllCategories();
	void selectAllIngredients();
	void unselectAllAuthors();
	void unselectAllCategories();
	void unselectAllIngredients();
	
	void authorSwitchType(int);
	void categorySwitchType(int);
	void ingredientSwitchType(int);
	
	void updateMaps( QMap<QCheckListItem*,bool>*, QMap<QCheckListItem*,bool>*, QListView* );
	void storeMap(QMap<QCheckListItem*,bool> *map_to_store, QListView *listview);

};

#endif //ADVANCEDSEARCHDIALOG_H
