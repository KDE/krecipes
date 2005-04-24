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
class QLabel;
class KPushButton;
class KListView;

class RecipeDB;


class AdvancedSearchDialog : public QWidget
{
	Q_OBJECT

public:
	AdvancedSearchDialog( QWidget *parent, RecipeDB * );
	~AdvancedSearchDialog();

	void reload();
	virtual void languageChange();

	QTabWidget* paramsTabWidget;
	QWidget* ingTab;
	QLineEdit* ingredientsAllEdit;
	QLineEdit* ingredientsAnyEdit;
	QLabel* textLabel1_3;
	QLabel* textLabel1_2;
	QLabel* textLabel1;
	QLineEdit* ingredientsWithoutEdit;
	QWidget* catTab;
	QLineEdit* categoriesAnyEdit;
	QLabel* textLabel1_3_3;
	QLabel* textLabel1_2_3;
	QLabel* textLabel1_5;
	QLineEdit* categoriesNotEdit;
	QLineEdit* categoriesAllEdit;
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
	QLineEdit* authorsAllEdit;
	QLabel* textLabel1_6;
	QLabel* textLabel1_3_4;
	QLineEdit* authorsWithoutEdit;
	QLabel* textLabel1_2_4;
	QLineEdit* authorsAnyEdit;
	KPushButton* findButton;
	KPushButton* clearButton;
	KListView* resultsListView;
	
	protected:
	QVBoxLayout* AdvancedSearchDialogLayout;
	QHBoxLayout* layout17;
	QGridLayout* ingTabLayout;
	QGridLayout* catTabLayout;
	QHBoxLayout* servPrepTabLayout;
	QVBoxLayout* servingsBoxLayout;
	QHBoxLayout* servingsFrameLayout;
	QVBoxLayout* prepTimeBoxLayout;
	QHBoxLayout* prepFrameLayout;
	QGridLayout* authorTabLayout;
	QVBoxLayout* layout9;
	QSpacerItem* spacer7;

	RecipeDB *database;

signals:
	void recipeSelected( int, int );

private slots:
	void search();
	void clear();
};

#endif //ADVANCEDSEARCHDIALOG_H
