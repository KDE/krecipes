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

#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QScrollView;
class QPushButton;
class QFrame;
class QLineEdit;
class QLabel;
class QCheckBox;
class QComboBox;
class QSpinBox;
class QTimeEdit;
class KPushButton;
class KListView;
class QListViewItem;

class RecipeDB;


class AdvancedSearchDialog : public QWidget
{
	Q_OBJECT

public:
	AdvancedSearchDialog( QWidget *parent, RecipeDB * );
	~AdvancedSearchDialog();

	virtual void languageChange();

	QLabel* textLabel1_4;
	QScrollView* scrollView1;
	QFrame* parametersFrame;
	QLineEdit* titleEdit;
	QPushButton* titleButton;
	QFrame* titleFrame;
	QPushButton* ingredientButton;
	QFrame* ingredientFrame;
	QLineEdit* ingredientsAllEdit;
	QLineEdit* ingredientsAnyEdit;
	QLabel* textLabel1_2;
	QLabel* textLabel1;
	QLineEdit* ingredientsWithoutEdit;
	QLabel* textLabel1_3;
	QPushButton* categoriesButton;
	QFrame* categoryFrame;
	QLineEdit* categoriesAllEdit;
	QLabel* textLabel1_5;
	QLabel* textLabel1_3_3;
	QLineEdit* categoriesAnyEdit;
	QLabel* textLabel1_2_3;
	QLineEdit* categoriesNotEdit;
	QPushButton* authorsButton;
	QFrame* authorsFrame;
	QLabel* textLabel1_2_4;
	QLabel* textLabel1_6;
	QLabel* textLabel1_3_4;
	QLineEdit* authorsAnyEdit;
	QLineEdit* authorsAllEdit;
	QLineEdit* authorsWithoutEdit;
	QPushButton* servingsButton;
	QFrame* servingsFrame;
	QCheckBox* enableServingsCheckBox;
	QComboBox* servingsComboBox;
	QSpinBox* servingsSpinBox;
	QPushButton* prepTimeButton;
	QFrame* prepTimeFrame;
	QCheckBox* enablePrepTimeCheckBox;
	QComboBox* prepTimeComboBox;
	QTimeEdit* prepTimeEdit;
	QLineEdit* instructionsEdit;
	QPushButton* instructionsButton;
	QFrame* instructionsFrame;
	KPushButton* clearButton;
	KPushButton* findButton;
	KListView* resultsListView;
	QCheckBox *requireAllTitle;
	QCheckBox *requireAllInstructions;
	
protected:
	QHBoxLayout* AdvancedSearchDialogLayout;
	QVBoxLayout* layout7;
	QHBoxLayout* scrollView1Layout;
	QVBoxLayout* parametersFrameLayout;
	QSpacerItem* spacer3_2_3_2_2;
	QSpacerItem* spacer3_2_3_2;
	QSpacerItem* spacer3_2_3;
	QSpacerItem* spacer3_2_2;
	QSpacerItem* titleFrameSpacer;
	QSpacerItem* instructionsFrameSpacer;
	QSpacerItem* spacer15;
	QVBoxLayout* titleFrameLayout;
	QGridLayout* ingredientFrameLayout;
	QGridLayout* categoryFrameLayout;
	QGridLayout* authorsFrameLayout;
	QVBoxLayout* servingsFrameLayout;
	QHBoxLayout* layout5;
	QVBoxLayout* prepTimeFrameLayout;
	QVBoxLayout* instructionsFrameLayout;
	QHBoxLayout* layout6;
	QHBoxLayout* layout9;
	QSpacerItem* spacer3;

	RecipeDB *database;

signals:
	void recipeSelected( int, int );

private slots:
	void search();
	void clear();
	void buttonSwitched();

private:
	QStringList split( const QString &text, bool sql_wildcards = false ) const;
};

#endif //ADVANCEDSEARCHDIALOG_H
