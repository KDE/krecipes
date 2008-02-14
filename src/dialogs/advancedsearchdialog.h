/***************************************************************************
*   Copyright (C) 2003-2004 by                                            *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef ADVANCEDSEARCHDIALOG_H
#define ADVANCEDSEARCHDIALOG_H

#include <qwidget.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3GridLayout>
#include <Q3Frame>
#include <Q3ValueList>
#include <QLabel>
#include <Q3VBoxLayout>

class Q3VBoxLayout;
class Q3HBoxLayout;
class Q3GridLayout;
class QSpacerItem;
class Q3ScrollView;
class QPushButton;
class Q3Frame;
class QLineEdit;
class QLabel;
class QCheckBox;
class QComboBox;
class QSpinBox;
class Q3TimeEdit;
class KPushButton;
class K3ListView;
class QRadioButton;
class Q3ButtonGroup;

class KDateEdit;
class RecipeDB;
class RecipeActionsHandler;
class FractionInput;
class CriteriaComboBox;


class AdvancedSearchDialog : public QWidget
{
	Q_OBJECT

public:
	AdvancedSearchDialog( QWidget *parent, RecipeDB * );
	~AdvancedSearchDialog();

	virtual void languageChange();

	RecipeActionsHandler *actionHandler;

protected:
	QLabel* textLabel1_4;
	Q3ScrollView* scrollView1;
	Q3Frame* parametersFrame;
	QLineEdit* titleEdit;
	QPushButton* titleButton;
	Q3Frame* titleFrame;
	QPushButton* ingredientButton;
	Q3Frame* ingredientFrame;
	QLineEdit* ingredientsAllEdit;
	QLineEdit* ingredientsAnyEdit;
	QLabel* textLabel1_2;
	QLabel* textLabel1;
	QLineEdit* ingredientsWithoutEdit;
	QLabel* textLabel1_3;
	QPushButton* categoriesButton;
	Q3Frame* categoryFrame;
	QLineEdit* categoriesAllEdit;
	QLabel* textLabel1_5;
	QLabel* textLabel1_3_3;
	QLineEdit* categoriesAnyEdit;
	QLabel* textLabel1_2_3;
	QLineEdit* categoriesNotEdit;
	QPushButton* authorsButton;
	Q3Frame* authorsFrame;
	QLabel* textLabel1_2_4;
	QLabel* textLabel1_6;
	QLabel* textLabel1_3_4;
	QLineEdit* authorsAnyEdit;
	QLineEdit* authorsAllEdit;
	QLineEdit* authorsWithoutEdit;
	QPushButton* servingsButton;
	Q3Frame* servingsFrame;
	QCheckBox* enableServingsCheckBox;
	QComboBox* servingsComboBox;
	QSpinBox* servingsSpinBox;
	QPushButton* prepTimeButton;
	Q3Frame* prepTimeFrame;
	QCheckBox* enablePrepTimeCheckBox;
	QComboBox* prepTimeComboBox;
	Q3TimeEdit* prepTimeEdit;
	QLineEdit* instructionsEdit;
	QPushButton* instructionsButton;
	Q3Frame* instructionsFrame;
	KPushButton* clearButton;
	KPushButton* findButton;
	K3ListView* resultsListView;
	QCheckBox *requireAllTitle;
	QCheckBox *requireAllInstructions;
	KDateEdit *createdStartDateEdit;
	KDateEdit *createdEndDateEdit;
	KDateEdit *modifiedStartDateEdit;
	KDateEdit *modifiedEndDateEdit;
	KDateEdit *accessedStartDateEdit;
	KDateEdit *accessedEndDateEdit;
	QPushButton* metaDataButton;
	Q3Frame* metaDataFrame;
	QRadioButton* ratingAvgRadioButton;
	FractionInput* avgStarsEdit;
	QLabel* avgStarsLabel;
	QRadioButton* criterionRadioButton;
	CriteriaComboBox* criteriaComboBox;
	FractionInput* starsWidget;
	QPushButton* addCriteriaButton;
	QPushButton* removeCriteriaButton;
	K3ListView* criteriaListView;
	QPushButton* ratingsButton;
	Q3ButtonGroup *ratingButtonGroup;
	Q3Frame *ratingAvgFrame;
	Q3Frame *criterionFrame;
	Q3VBoxLayout *criterionFrameLayout;
	Q3HBoxLayout *ratingAvgFrameLayout;
	Q3VBoxLayout *ratingButtonGroupLayout;

	Q3HBoxLayout* AdvancedSearchDialogLayout;
	Q3VBoxLayout* layout7;
	Q3HBoxLayout* scrollView1Layout;
	Q3VBoxLayout* parametersFrameLayout;
	QSpacerItem* spacer3_2_3_2_2;
	QSpacerItem* spacer3_2_3_2;
	QSpacerItem* spacer3_2_3;
	QSpacerItem* spacer3_2_2;
	QSpacerItem* titleFrameSpacer;
	QSpacerItem* instructionsFrameSpacer;
	QSpacerItem* metaDataFrameSpacer;
	QSpacerItem* spacer15;
	Q3VBoxLayout* titleFrameLayout;
	Q3GridLayout* ingredientFrameLayout;
	Q3GridLayout* categoryFrameLayout;
	Q3GridLayout* authorsFrameLayout;
	Q3VBoxLayout* servingsFrameLayout;
	Q3HBoxLayout* layout5;
	Q3VBoxLayout* prepTimeFrameLayout;
	Q3VBoxLayout* instructionsFrameLayout;
	Q3VBoxLayout* metaDataFrameLayout;
	Q3HBoxLayout* layout6;
	Q3HBoxLayout* layout9;
	QSpacerItem* spacer3;
	Q3VBoxLayout* ratingsFrameLayout;
	Q3HBoxLayout* layout11;
	Q3HBoxLayout* layout12;
	QSpacerItem* ratingsFrameSpacer;

	RecipeDB *database;

signals:
	void recipeSelected( int, int );
	void recipesSelected( const Q3ValueList<int> &, int );

private slots:
	void search();
	void clear();
	void buttonSwitched();
	void activateRatingOption( int button_id );
	void slotAddRatingCriteria();
	void slotRemoveRatingCriteria();

	//called by a signal from the database when a recipe is removed
	void removeRecipe( int id );

private:
	QStringList split( const QString &text, bool sql_wildcards = false ) const;
};

#endif //ADVANCEDSEARCHDIALOG_H

