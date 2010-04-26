/**************************************************************************
*   Copyright © 2003-2004 Jason Kivlighn <jkivlighn@gmail.com>	          *
*	                                                                  *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or	  *
*   (at your option) any later version.	                                  *
***************************************************************************/

#ifndef ADVANCEDSEARCHDIALOG_H
#define ADVANCEDSEARCHDIALOG_H

#include <qwidget.h>
//Added by qt3to4:
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QScrollArea;
class KPushButton;
class QFrame;
class KLineEdit;
class QLabel;
class QCheckBox;
class KComboBox;
class QSpinBox;
class QDateTimeEdit;
class KPushButton;
class K3ListView;
class QRadioButton;
class QGroupBox;
class KAction;

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

	RecipeActionsHandler *actionHandler;
	void addAction( KAction * action );

protected:
	virtual void showEvent( QShowEvent * event );

	QLabel* searchLabel;
	QScrollArea* scrollArea1;
	QFrame* parametersFrame;
	KLineEdit* titleEdit;
	KPushButton* titleButton;
	QFrame* titleFrame;
	KPushButton* ingredientButton;
	QFrame* ingredientFrame;
	KLineEdit* ingredientsAllEdit;
	KLineEdit* ingredientsAnyEdit;
	KLineEdit* ingredientsWithoutEdit;
	KPushButton* categoriesButton;
	QFrame* categoryFrame;
	KLineEdit* categoriesAllEdit;
	KLineEdit* categoriesAnyEdit;
	KLineEdit* categoriesNotEdit;
	KPushButton* authorsButton;
	QFrame* authorsFrame;
	KLineEdit* authorsAnyEdit;
	KLineEdit* authorsAllEdit;
	KLineEdit* authorsWithoutEdit;
	KPushButton* servingsButton;
	QFrame* servingsFrame;
	QCheckBox* enableServingsCheckBox;
	KComboBox* servingsComboBox;
	QSpinBox* servingsSpinBox;
	KPushButton* prepTimeButton;
	QFrame* prepTimeFrame;
	QCheckBox* enablePrepTimeCheckBox;
	KComboBox* prepTimeComboBox;
	QDateTimeEdit* prepTimeEdit;
	KLineEdit* instructionsEdit;
	KPushButton* instructionsButton;
	QFrame* instructionsFrame;
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
	KPushButton* metaDataButton;
	QFrame* metaDataFrame;
	QRadioButton* ratingAvgRadioButton;
	FractionInput* avgStarsEdit;
	QLabel* avgStarsLabel;
	QRadioButton* criterionRadioButton;
	CriteriaComboBox* criteriaComboBox;
	FractionInput* starsWidget;
	KPushButton* addCriteriaButton;
	KPushButton* removeCriteriaButton;
	K3ListView* criteriaListView;
	KPushButton* ratingsButton;
	QGroupBox *ratingButtonGroup;
	QFrame *ratingAvgFrame;
	QFrame *criterionFrame;
	QVBoxLayout *criterionFrameLayout;
	QHBoxLayout *ratingAvgFrameLayout;
	QVBoxLayout *ratingButtonGroupLayout;

	QHBoxLayout* AdvancedSearchDialogLayout;
	QVBoxLayout* layout7;
	QHBoxLayout* scrollView1Layout;
	QVBoxLayout* parametersFrameLayout;
	QSpacerItem* ingredientspacer;
	QSpacerItem* categoryspacer;
	QSpacerItem* authorspacer;
	QSpacerItem* servingsspacer;
	QSpacerItem* titleFrameSpacer;
	QSpacerItem* instructionsFrameSpacer;
	QSpacerItem* metaDataFrameSpacer;
	QSpacerItem* spacer15;
	QVBoxLayout* titleFrameLayout;
	QFormLayout* ingredientFrameLayout;
	QFormLayout* categoryFrameLayout;
	QFormLayout* authorsFrameLayout;
	QVBoxLayout* servingsFrameLayout;
	QHBoxLayout* layout5;
	QVBoxLayout* prepTimeFrameLayout;
	QVBoxLayout* instructionsFrameLayout;
	QVBoxLayout* metaDataFrameLayout;
	QHBoxLayout* layout6;
	QHBoxLayout* layout9;
	QSpacerItem* spacer3;
	QVBoxLayout* ratingsFrameLayout;
	QHBoxLayout* layout11;
	QHBoxLayout* layout12;
	QSpacerItem* ratingsFrameSpacer;

	RecipeDB *database;

signals:
	void recipeSelected( bool );
	void recipeSelected( int, int );
	void recipesSelected( const QList<int> &, int );

private slots:
	void search();
	void clear();
	void buttonSwitched();
	void activateRatingOptionAvg();
	void activateRatingOptionCriterion();
	void slotAddRatingCriteria();
	void slotRemoveRatingCriteria();
	void haveSelectedItems();

	//called by a signal from the database when a recipe is removed
	void removeRecipe( int id );

private:
	QStringList split( const QString &text, bool sql_wildcards = false ) const;
};

#endif //ADVANCEDSEARCHDIALOG_H

