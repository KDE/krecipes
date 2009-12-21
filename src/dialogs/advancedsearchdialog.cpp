/***************************************************************************
*   Copyright © 2003-2004 Jason Kivlighn <jkivlighn@gmail.com>	           *
*		                                                           *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or	   *
*   (at your option) any later version.	                                   *
***************************************************************************/

#include "advancedsearchdialog.h"

#include <KPushButton>
#include <qwidget.h>
#include <QCheckBox>
#include <qframe.h>
#include <KComboBox>
#include <q3header.h>
#include <q3listview.h>
#include <qspinbox.h>
#include <QDateTimeEdit>
#include <QScrollArea>
#include <QSplitter>
//Added by qt3to4:
#include <QHBoxLayout>
#include <QGridLayout>
#include <QVBoxLayout>
#include <kpushbutton.h>
#include <q3whatsthis.h>
#include <QLabel>
#include <KLineEdit>

#include <QGroupBox>
#include <QRadioButton>

#include <kapplication.h>
#include <kcursor.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <k3listview.h>
#include <klocale.h>
#include <kconfiggroup.h>
#include <kglobal.h>
#include <kmenu.h>
#include <kvbox.h>

#include "backends/recipedb.h"
#include "backends/searchparameters.h"
#include "actionshandlers/recipeactionshandler.h"
#include "widgets/recipelistview.h"
#include "widgets/kdateedit.h"
#include "widgets/fractioninput.h"
#include "widgets/criteriacombobox.h"

#include "profiling.h"

AdvancedSearchDialog::AdvancedSearchDialog( QWidget *parent, RecipeDB *db ) : QWidget( parent ),
		database( db )
{
	///
	///BEGIN OF AUTOMATICALLY GENERATED GUI CODE///
	///
	AdvancedSearchDialogLayout = new QHBoxLayout( this ); 
	AdvancedSearchDialogLayout ->setObjectName( "AdvancedSearchDialogLayout " );
	AdvancedSearchDialogLayout ->setMargin( 5 );
	AdvancedSearchDialogLayout ->setSpacing( 3 );
	
	layout7 = new QVBoxLayout; 
	layout7->setObjectName( "layout7" );
	layout7->setMargin( 0 );
	layout7->setSpacing( 3 );
	
	textLabel1_4 = new QLabel( this );
	textLabel1_4->setObjectName( "textLabel1_4" );
	textLabel1_4->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed );
	layout7->addWidget( textLabel1_4 );

	QSplitter *splitter = new QSplitter( this );
	QFrame *leftFrame = new QFrame;
	leftFrame->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
	leftFrame->resize( 1, 1 );
	
	scrollArea1 = new QScrollArea( this );
	scrollArea1->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
	
	parametersFrame = new QFrame( scrollArea1 );
	parametersFrame->setObjectName( "parametersFrame" ); 
	parametersFrame->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
	parametersFrame->resize( 1, 1 );
	parametersFrame->setFrameShape( QFrame::NoFrame );
	parametersFrame->setFrameShadow( QFrame::Plain );
	parametersFrame->setLineWidth( 0 );
	parametersFrameLayout = new QVBoxLayout( parametersFrame ); 
	parametersFrameLayout->setObjectName( "parametersFrameLayout" );
	parametersFrameLayout->setMargin( 0 );
	parametersFrameLayout->setSpacing( 0 );
	
	titleButton = new KPushButton( parametersFrame );
	titleButton->setObjectName( "titleButton" );
	titleButton->setCheckable( true );
	parametersFrameLayout->addWidget( titleButton );
	
	titleFrame = new QFrame( parametersFrame );
	titleFrame->setObjectName( "titleFrame" ); 
	titleFrame->setFrameShape( QFrame::StyledPanel );
	titleFrame->setFrameShadow( QFrame::Raised );
	titleFrameLayout = new QVBoxLayout( titleFrame ); 
	titleFrameLayout ->setObjectName( "titleFrameLayout" );
	titleFrameLayout ->setMargin( 5 );
	titleFrameLayout ->setSpacing( 3 );

	requireAllTitle = new QCheckBox( i18nc("@option:check", "Require All Words"), titleFrame );
	titleFrameLayout->addWidget( requireAllTitle );

	KHBox *titleHBox = new KHBox( titleFrame );
	/*QLabel *titleInfoLabel = */new QLabel(i18nc("@label:textbox", "Keywords:"),titleHBox);
	titleEdit = new KLineEdit( titleHBox );
	titleEdit->setObjectName( "titleEdit" );
	titleFrameLayout->addWidget( titleHBox );

	parametersFrameLayout->addWidget( titleFrame );
	titleFrameSpacer = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Preferred );
	parametersFrameLayout->addItem( titleFrameSpacer );


	ingredientButton = new KPushButton( parametersFrame );
	ingredientButton->setObjectName( "ingredientButton" );
	ingredientButton->setCheckable( true );
	parametersFrameLayout->addWidget( ingredientButton );
	
	ingredientFrame = new QFrame( parametersFrame );
	ingredientFrame->setObjectName( "ingredientFrame" ); 
	ingredientFrame->setFrameShape( QFrame::StyledPanel );
	ingredientFrame->setFrameShadow( QFrame::Raised );
	ingredientFrameLayout = new QGridLayout( ingredientFrame ); 
	ingredientFrameLayout->cellRect( 1, 1 );
	ingredientFrameLayout->setMargin( 3 );
	ingredientFrameLayout->setSpacing( 3 );
	ingredientFrameLayout->setObjectName( "ingredientFrameLayout" );

	QLabel *ingredientInfoLabel = new QLabel(i18nc("@info", "Enter ingredients: (e.g. chicken pasta \"white wine\")"),ingredientFrame);
	ingredientInfoLabel->setWordWrap( true );
	//ingredientInfoLabel->setTextFormat( Qt::RichText );
	ingredientFrameLayout->addWidget( ingredientInfoLabel, 0, 0, 1, 2, 0 );

	ingredientsAllEdit = new KLineEdit( ingredientFrame );
	ingredientsAllEdit->setObjectName( "ingredientsAllEdit" );
	
	ingredientFrameLayout->addWidget( ingredientsAllEdit, 1, 1 );
	
	ingredientsAnyEdit = new KLineEdit( ingredientFrame );
	ingredientsAnyEdit->setObjectName( "ingredientsAnyEdit" );
	
	ingredientFrameLayout->addWidget( ingredientsAnyEdit, 2, 1 );
	
	textLabel1_2 = new QLabel( ingredientFrame );
	textLabel1_2->setObjectName( "textLabel1_2" );
	
	ingredientFrameLayout->addWidget( textLabel1_2, 2, 0 );
	
	textLabel1 = new QLabel( ingredientFrame );
	textLabel1->setObjectName( "textLabel1" );
	
	ingredientFrameLayout->addWidget( textLabel1, 1, 0 );
	
	ingredientsWithoutEdit = new KLineEdit( ingredientFrame );
	ingredientsWithoutEdit->setObjectName( "ingredientsWithoutEdit" );
	
	ingredientFrameLayout->addWidget( ingredientsWithoutEdit, 3, 1 );
	
	textLabel1_3 = new QLabel( ingredientFrame );
	textLabel1_3->setObjectName( "textLabel1_3" );
	
	ingredientFrameLayout->addWidget( textLabel1_3, 3, 0 );
	parametersFrameLayout->addWidget( ingredientFrame );
	spacer3_2_3_2_2 = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Preferred );
	parametersFrameLayout->addItem( spacer3_2_3_2_2 );


	categoriesButton = new KPushButton( parametersFrame );
	categoriesButton->setObjectName( "categoriesButton" );
	categoriesButton->setCheckable( true );
	parametersFrameLayout->addWidget( categoriesButton );
	
	categoryFrame = new QFrame( parametersFrame );
	categoryFrame->setObjectName( "categoryFrame" ); 
	categoryFrame->setFrameShape( QFrame::StyledPanel );
	categoryFrame->setFrameShadow( QFrame::Raised );
	categoryFrameLayout = new QGridLayout( categoryFrame );
	categoryFrameLayout->cellRect( 1, 1 );
	categoryFrameLayout->setMargin( 3 );
	categoryFrameLayout->setSpacing( 3 );
	categoryFrameLayout->setObjectName( "ingredientFrameLayout" );
	

	QLabel *categoryInfoLabel = new QLabel(i18nc("@info", "Enter categories: (e.g. Desserts Pastas \"Main Dishes\")"),categoryFrame);
	categoryInfoLabel->setWordWrap( true );
	categoryInfoLabel->setTextFormat( Qt::RichText );
	categoryFrameLayout->addWidget( categoryInfoLabel, 0, 0, 1, 2, 0 );
	
	categoriesAllEdit = new KLineEdit( categoryFrame );
	categoriesAllEdit->setObjectName( "categoriesAllEdit" );
	
	categoryFrameLayout->addWidget( categoriesAllEdit, 1, 1 );
	
	textLabel1_5 = new QLabel( categoryFrame );
	textLabel1_5->setObjectName( "textLabel1_5" );
	
	categoryFrameLayout->addWidget( textLabel1_5, 1, 0 );
	
	textLabel1_3_3 = new QLabel( categoryFrame );
	textLabel1_3_3->setObjectName( "textLabel1_3_3" );
	
	categoryFrameLayout->addWidget( textLabel1_3_3, 3, 0 );
	
	categoriesAnyEdit = new KLineEdit( categoryFrame );
	categoriesAnyEdit->setObjectName( "categoriesAnyEdit" );
	
	categoryFrameLayout->addWidget( categoriesAnyEdit, 2, 1 );
	
	textLabel1_2_3 = new QLabel( categoryFrame );
	textLabel1_2_3->setObjectName( "textLabel1_2_3" );
	
	categoryFrameLayout->addWidget( textLabel1_2_3, 2, 0 );
	
	categoriesNotEdit = new KLineEdit( categoryFrame );
	categoriesNotEdit->setObjectName( "categoriesNotEdit" );
	
	categoryFrameLayout->addWidget( categoriesNotEdit, 3, 1 );
	parametersFrameLayout->addWidget( categoryFrame );
	spacer3_2_3_2 = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Preferred );
	parametersFrameLayout->addItem( spacer3_2_3_2 );


	authorsButton = new KPushButton( parametersFrame );
	authorsButton->setObjectName( "authorsButton" );
	authorsButton->setCheckable( true );
	parametersFrameLayout->addWidget( authorsButton );
	
	authorsFrame = new QFrame( parametersFrame );
	authorsFrame->setObjectName( "authorsFrame" ); 
	authorsFrame->setFrameShape( QFrame::StyledPanel );
	authorsFrame->setFrameShadow( QFrame::Raised );
	authorsFrameLayout = new QGridLayout( authorsFrame ); 
	authorsFrameLayout->cellRect( 1, 1 );
	authorsFrameLayout->setMargin( 3 );
	authorsFrameLayout->setSpacing( 3 );
	authorsFrameLayout->setObjectName( "ingredientFrameLayout" );

	QLabel *authorsInfoLabel = new QLabel(i18nc("@info", "Enter author name (e.g. Smith or \"Jane Doe\")"),authorsFrame);
	authorsInfoLabel->setWordWrap( true );
	authorsInfoLabel->setTextFormat( Qt::RichText );
	authorsFrameLayout->addWidget( authorsInfoLabel, 0, 0, 1, 2, 0 );

	textLabel1_2_4 = new QLabel( authorsFrame );
	textLabel1_2_4->setObjectName( "textLabel1_2_4" );
	
	authorsFrameLayout->addWidget( textLabel1_2_4, 1, 0 );
	
	textLabel1_6 = new QLabel( authorsFrame );
	textLabel1_6->setObjectName( "textLabel1_6" );
	
	authorsFrameLayout->addWidget( textLabel1_6, 2, 0 );
	
	textLabel1_3_4 = new QLabel( authorsFrame );
	textLabel1_3_4->setObjectName( "textLabel1_3_4" );
	
	authorsFrameLayout->addWidget( textLabel1_3_4, 3, 0 );
	
	authorsAnyEdit = new KLineEdit( authorsFrame );
	authorsAnyEdit->setObjectName( "authorsAnyEdit" );
	
	authorsFrameLayout->addWidget( authorsAnyEdit, 1, 1 );
	
	authorsAllEdit = new KLineEdit( authorsFrame );
	authorsAllEdit->setObjectName( "authorsAllEdit" );
	
	authorsFrameLayout->addWidget( authorsAllEdit, 2, 1 );
	
	authorsWithoutEdit = new KLineEdit( authorsFrame );
	authorsWithoutEdit->setObjectName( "authorsWithoutEdit" );
	
	authorsFrameLayout->addWidget( authorsWithoutEdit, 3, 1 );
	parametersFrameLayout->addWidget( authorsFrame );
	spacer3_2_3 = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Preferred );
	parametersFrameLayout->addItem( spacer3_2_3 );


	servingsButton = new KPushButton( parametersFrame );
	servingsButton->setObjectName( "servingsButton" );
	servingsButton->setCheckable( true );
	parametersFrameLayout->addWidget( servingsButton );
	
	servingsFrame = new QFrame( parametersFrame );
	servingsFrame->setObjectName( "servingsFrame" ); 
	servingsFrame->setFrameShape( QFrame::StyledPanel );
	servingsFrame->setFrameShadow( QFrame::Raised );
	servingsFrameLayout = new QVBoxLayout( servingsFrame ); 
	servingsFrameLayout->setObjectName( "servingsFrameLayout " );
	servingsFrameLayout->setMargin( 3 );
	servingsFrameLayout->setSpacing( 3 );
	
	enableServingsCheckBox = new QCheckBox( servingsFrame );
	enableServingsCheckBox->setObjectName( "enableServingsCheckBox" );
	servingsFrameLayout->addWidget( enableServingsCheckBox );
	
	layout5 = new QHBoxLayout; 
	layout5->setObjectName( "layout5" );
	layout5->setMargin( 0 );
	layout5->setSpacing( 3 );
	
	servingsComboBox = new KComboBox( servingsFrame );
	servingsComboBox->setObjectName( "servingsComboBox" );
	servingsComboBox->setEditable( false );
	servingsComboBox->setEnabled( false );
	layout5->addWidget( servingsComboBox );
	
	servingsSpinBox = new QSpinBox( servingsFrame );
	servingsSpinBox->setObjectName( "servingsSpinBox" );
	servingsSpinBox->setEnabled( false );
	servingsSpinBox->setMinimum( 1 );
	servingsSpinBox->setMaximum( 9999 );
	layout5->addWidget( servingsSpinBox );
	servingsFrameLayout->addLayout( layout5 );
	parametersFrameLayout->addWidget( servingsFrame );
	spacer3_2_2 = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Preferred );
	parametersFrameLayout->addItem( spacer3_2_2 );


	prepTimeButton = new KPushButton( parametersFrame );
	prepTimeButton->setObjectName( "prepTimeButton" );
	prepTimeButton->setCheckable( true );
	parametersFrameLayout->addWidget( prepTimeButton );
	
	prepTimeFrame = new QFrame( parametersFrame );
	prepTimeFrame->setObjectName( "prepTimeFrame" ); 
	prepTimeFrame->setFrameShape( QFrame::StyledPanel );
	prepTimeFrame->setFrameShadow( QFrame::Raised );
	prepTimeFrameLayout = new QVBoxLayout( prepTimeFrame ); 
	prepTimeFrameLayout->setObjectName( "prepTimeFrameLayout" );
	prepTimeFrameLayout->setMargin( 3 );
	prepTimeFrameLayout->setSpacing( 3 );
	
	enablePrepTimeCheckBox = new QCheckBox( prepTimeFrame );
	enablePrepTimeCheckBox->setObjectName( "enablePrepTimeCheckBox" );
	prepTimeFrameLayout->addWidget( enablePrepTimeCheckBox );
	
	layout6 = new QHBoxLayout; 
	layout6 ->setObjectName( "layout6" );
	layout6 ->setMargin( 0 );
	layout6 ->setSpacing( 3 );
	
	prepTimeComboBox = new KComboBox( prepTimeFrame );
	prepTimeComboBox->setEditable( false );
	prepTimeComboBox->setObjectName( "prepTimeComboBox" );
	prepTimeComboBox->setEnabled( false );
	layout6->addWidget( prepTimeComboBox );
	
	prepTimeEdit = new QDateTimeEdit( prepTimeFrame );
	prepTimeEdit->setObjectName( "prepTimeEdit" );
	prepTimeEdit->setEnabled( false );
	prepTimeEdit->setDisplayFormat( "hh:mm" );
	layout6->addWidget( prepTimeEdit );
	prepTimeFrameLayout->addLayout( layout6 );
	parametersFrameLayout->addWidget( prepTimeFrame );
	spacer15 = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Preferred );
	parametersFrameLayout->addItem( spacer15 );


	instructionsButton = new KPushButton( parametersFrame );
	instructionsButton->setObjectName( "instructionsButton" );
	instructionsButton->setCheckable( true );
	parametersFrameLayout->addWidget( instructionsButton );
	
	instructionsFrame = new QFrame( parametersFrame );
	instructionsFrame->setObjectName( "instructionsFrame" ); 
	instructionsFrame->setFrameShape( QFrame::StyledPanel );
	instructionsFrame->setFrameShadow( QFrame::Raised );
	instructionsFrameLayout = new QVBoxLayout( instructionsFrame ); 
	instructionsFrameLayout->setObjectName( "instructionsFrameLayout" );
	instructionsFrameLayout->setMargin( 5 );
	instructionsFrameLayout->setSpacing( 3 );

	requireAllInstructions = new QCheckBox( i18nc("@option:check", "Require All Words"), instructionsFrame );
	instructionsFrameLayout->addWidget( requireAllInstructions );

	KHBox *instructionsHBox = new KHBox(instructionsFrame);
	/*QLabel *instructionsInfoLabel = */new QLabel(i18nc("@label:textbox", "Keywords:"),instructionsHBox);

	instructionsEdit = new KLineEdit( instructionsHBox );
	instructionsEdit->setObjectName( "instructionsEdit" );
	instructionsFrameLayout->addWidget( instructionsHBox );

	parametersFrameLayout->addWidget( instructionsFrame );
	instructionsFrameSpacer = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Preferred );
	parametersFrameLayout->addItem( instructionsFrameSpacer );


	metaDataButton = new KPushButton( parametersFrame );
	metaDataButton->setObjectName( "metaDataButton" );
	metaDataButton->setCheckable( true );
	parametersFrameLayout->addWidget( metaDataButton );
	
	metaDataFrame = new QFrame( parametersFrame );
	metaDataFrame->setObjectName( "metaDataFrame" ); 
	metaDataFrame->setFrameShape( QFrame::StyledPanel );
	metaDataFrame->setFrameShadow( QFrame::Raised );
	metaDataFrameLayout = new QVBoxLayout( metaDataFrame );
	metaDataFrameLayout->setObjectName( "metaDataFrameLayout" );
	metaDataFrameLayout->setMargin( 5 );
	metaDataFrameLayout->setSpacing( 3 );

	QLabel *createdLabel = new QLabel( i18nc("@label Recipe created date", "Created:"), metaDataFrame );
	metaDataFrameLayout->addWidget( createdLabel );

	KHBox *createdHBox = new KHBox(metaDataFrame);
	createdStartDateEdit = new KDateEdit(createdHBox,"createdStartEdit");
	createdStartDateEdit->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
	(void)new QLabel(" - ",createdHBox);
	createdEndDateEdit = new KDateEdit(createdHBox,"createdEndEdit");
	createdEndDateEdit->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
	metaDataFrameLayout->addWidget( createdHBox );

	QLabel *modifiedLabel = new QLabel( i18nc("@label Recipe modified date", "Modified:"), metaDataFrame );
	metaDataFrameLayout->addWidget( modifiedLabel );

	KHBox *modifiedHBox = new KHBox(metaDataFrame);
	modifiedStartDateEdit = new KDateEdit(modifiedHBox,"modifiedStartEdit");
	modifiedStartDateEdit->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
	(void)new QLabel(" - ",modifiedHBox);
	modifiedEndDateEdit = new KDateEdit(modifiedHBox,"modifiedEndEdit");
	modifiedEndDateEdit->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
	metaDataFrameLayout->addWidget( modifiedHBox );

	QLabel *accessedLabel = new QLabel( i18nc("@label Recipe last accessed date", "Last Accessed:"), metaDataFrame );
	metaDataFrameLayout->addWidget( accessedLabel );

	KHBox *accessedHBox = new KHBox(metaDataFrame);
	accessedStartDateEdit = new KDateEdit(accessedHBox,"accessedStartEdit");
	accessedStartDateEdit->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
	(void)new QLabel(" - ",accessedHBox);
	accessedEndDateEdit = new KDateEdit(accessedHBox,"accessedEndEdit");
	accessedEndDateEdit->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
	metaDataFrameLayout->addWidget( accessedHBox );

	parametersFrameLayout->addWidget( metaDataFrame );
	metaDataFrameSpacer = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Preferred );
	parametersFrameLayout->addItem( metaDataFrameSpacer );

	//=============RATINGS FRAME===========//
	ratingsButton = new KPushButton( parametersFrame );
	ratingsButton->setObjectName( "ratingsButton" );
	ratingsButton->setCheckable( true );
	parametersFrameLayout->addWidget( ratingsButton );

	ratingButtonGroup = new QGroupBox( parametersFrame );
	ratingButtonGroupLayout = new QVBoxLayout( parametersFrame );
	ratingButtonGroupLayout->setAlignment( Qt::AlignTop );
	ratingButtonGroup->setLayout( ratingButtonGroupLayout );
	
	ratingAvgRadioButton = new QRadioButton( ratingButtonGroup );
	ratingAvgRadioButton->setObjectName( "ratingAvgRadioButton" );
	ratingAvgRadioButton->setChecked( true );
	ratingButtonGroupLayout->addWidget( ratingAvgRadioButton );
	
	ratingAvgFrame = new QFrame( ratingButtonGroup );
	ratingAvgFrame->setObjectName( "ratingAvgFrame" ); 
	ratingAvgFrame->setLineWidth( 0 );
	ratingAvgFrameLayout = new QHBoxLayout( ratingAvgFrame ); 
	ratingAvgFrameLayout ->setObjectName( "ratingAvgFrameLayout" );
	ratingAvgFrameLayout ->setMargin( 2 );
	ratingAvgFrameLayout ->setSpacing( 2 );
	
	avgStarsEdit = new FractionInput( ratingAvgFrame );
	avgStarsEdit->setAllowRange(true);
	ratingAvgFrameLayout->addWidget( avgStarsEdit );
	
	avgStarsLabel = new QLabel( ratingAvgFrame );
	avgStarsLabel->setObjectName( "avgStarsLabel" );
	ratingAvgFrameLayout->addWidget( avgStarsLabel );
	ratingButtonGroupLayout->addWidget( ratingAvgFrame );
	
	criterionRadioButton = new QRadioButton( ratingButtonGroup );
	criterionRadioButton->setObjectName( "criterionRadioButton" );
	ratingButtonGroupLayout->addWidget( criterionRadioButton );
	
	criterionFrame = new QFrame( ratingButtonGroup );
	criterionFrame->setObjectName( "criterionFrame" ); 
	criterionFrame->setEnabled( false );
	criterionFrame->setLineWidth( 0 );
	criterionFrameLayout = new QVBoxLayout( criterionFrame ); 
	criterionFrameLayout ->setObjectName( "criterionFrameLayout" );
	criterionFrameLayout ->setMargin( 2 );
	criterionFrameLayout ->setSpacing( 2 );
	
	layout12 = new QHBoxLayout; 
	layout12->setObjectName( "layout12" );
	layout12->setMargin( 0 );
	layout12->setSpacing( 3 );
	
	criteriaComboBox = new CriteriaComboBox( false, criterionFrame, database );
	criteriaComboBox->setEditable( false );
	criteriaComboBox->reload();
	layout12->addWidget( criteriaComboBox );
	
	starsWidget = new FractionInput( criterionFrame );
	starsWidget->setAllowRange(true);
	starsWidget->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Preferred );
	layout12->addWidget( starsWidget );
	

	addCriteriaButton = new KPushButton( criterionFrame );
	addCriteriaButton->setObjectName( "addCriteriaButton" );
	addCriteriaButton->setMaximumSize( QSize( 30, 30 ) );
	layout12->addWidget( addCriteriaButton );

	removeCriteriaButton = new KPushButton( criterionFrame );
	removeCriteriaButton->setObjectName( "removeCriteriaButton" );
	//removeCriteriaButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, removeCriteriaButton->sizePolicy().hasHeightForWidth() ) );
	removeCriteriaButton->setMaximumSize( QSize( 30, 30 ) );
	layout12->addWidget( removeCriteriaButton );

	criterionFrameLayout->addLayout( layout12 );
	
	criteriaListView = new K3ListView( criterionFrame );
	criteriaListView->setObjectName( "criteriaListView" );
	criteriaListView->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
	criteriaListView->addColumn( i18nc( "@title:column", "Criterion" ) );
	criteriaListView->addColumn( i18nc( "@title:column", "Stars" ) );
	criterionFrameLayout->addWidget( criteriaListView );
	ratingButtonGroupLayout->addWidget( criterionFrame );

	parametersFrameLayout->addWidget( ratingButtonGroup );
	ratingsFrameSpacer = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding );
	parametersFrameLayout->addItem( ratingsFrameSpacer );


	scrollArea1->setWidget( parametersFrame );
	scrollArea1->setWidgetResizable( true );
	layout7->addWidget( scrollArea1 );
	
	layout9 = new QHBoxLayout; 
	layout9->setObjectName( "layout9" );
	layout9->setMargin( 0 );
	layout9->setSpacing( 3 );
	
	clearButton = new KPushButton( this );
	clearButton->setObjectName( "clearButton" );
	clearButton->setIcon( KIcon( "edit-clear" ) );
	layout9->addWidget( clearButton );
	spacer3 = new QSpacerItem( 110, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
	layout9->addItem( spacer3 );
	
	findButton = new KPushButton( this );
	findButton->setObjectName( "findButton" );
	findButton->setIcon( KIcon( "edit-find" ) );
	layout9->addWidget( findButton );
	layout7->addLayout( layout9 );
	leftFrame->setLayout( layout7 );
	splitter->addWidget( leftFrame );
	
	resultsListView = new K3ListView( this); 
	resultsListView->setObjectName( "resultsListView" );
	resultsListView->setSelectionMode( Q3ListView::Extended );
	splitter->addWidget( resultsListView );
	splitter->setChildrenCollapsible( false );
	AdvancedSearchDialogLayout->addWidget( splitter );
	languageChange();
	//KDE4 port
	//QWidget::setWindowState( WState_Polished );
	///
	///END OF AUTOMATICALLY GENERATED GUI CODE///
	///

	resultsListView->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
	scrollArea1->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Ignored );

	splitter->setStretchFactor( 1, 2 );

	KConfigGroup config = KGlobal::config()->group( "Advanced" );
	bool show_id = config.readEntry( "ShowID", false );
	
	resultsListView->addColumn( i18nc("@title:column Recipe title", "Title" ) );
	resultsListView->addColumn( "Id" , show_id ? -1 : 0 );

	actionHandler = new RecipeActionsHandler( resultsListView, database );

	connect( titleEdit, SIGNAL( returnPressed() ), SLOT( search() ) );
	connect( ingredientsAllEdit, SIGNAL( returnPressed() ), SLOT( search() ) );
	connect( ingredientsAnyEdit, SIGNAL( returnPressed() ), SLOT( search() ) );
	connect( ingredientsWithoutEdit, SIGNAL( returnPressed() ), SLOT( search() ) );
	connect( authorsAnyEdit, SIGNAL( returnPressed() ), SLOT( search() ) );
	connect( authorsAllEdit, SIGNAL( returnPressed() ), SLOT( search() ) );
	connect( authorsWithoutEdit, SIGNAL( returnPressed() ), SLOT( search() ) );
	connect( categoriesNotEdit, SIGNAL( returnPressed() ), SLOT( search() ) );
	connect( categoriesAnyEdit, SIGNAL( returnPressed() ), SLOT( search() ) );
	connect( categoriesAllEdit, SIGNAL( returnPressed() ), SLOT( search() ) );
	connect( ingredientsWithoutEdit, SIGNAL( returnPressed() ), SLOT( search() ) );
	connect( instructionsEdit, SIGNAL( returnPressed() ), SLOT( search() ) );

	connect( findButton, SIGNAL( clicked() ), SLOT( search() ) );
	connect( clearButton, SIGNAL( clicked() ), SLOT( clear() ) );

	connect( enableServingsCheckBox, SIGNAL( toggled( bool ) ), servingsSpinBox, SLOT( setEnabled( bool ) ) );
	connect( enableServingsCheckBox, SIGNAL( toggled( bool ) ), servingsComboBox, SLOT( setEnabled( bool ) ) );
	connect( enablePrepTimeCheckBox, SIGNAL( toggled( bool ) ), prepTimeEdit, SLOT( setEnabled( bool ) ) );
	connect( enablePrepTimeCheckBox, SIGNAL( toggled( bool ) ), prepTimeComboBox, SLOT( setEnabled( bool ) ) );

	connect( titleButton, SIGNAL( toggled( bool ) ), titleFrame, SLOT( setShown( bool ) ) );
	connect( ingredientButton, SIGNAL( toggled( bool ) ), ingredientFrame, SLOT( setShown( bool ) ) );
	connect( authorsButton, SIGNAL( toggled( bool ) ), authorsFrame, SLOT( setShown( bool ) ) );
	connect( categoriesButton, SIGNAL( toggled( bool ) ), categoryFrame, SLOT( setShown( bool ) ) );
	connect( servingsButton, SIGNAL( toggled( bool ) ), servingsFrame, SLOT( setShown( bool ) ) );
	connect( prepTimeButton, SIGNAL( toggled( bool ) ), prepTimeFrame, SLOT( setShown( bool ) ) );
	connect( instructionsButton, SIGNAL( toggled( bool ) ), instructionsFrame, SLOT( setShown( bool ) ) );
	connect( metaDataButton, SIGNAL( toggled( bool ) ), metaDataFrame, SLOT( setShown( bool ) ) );
	connect( ratingsButton, SIGNAL( toggled( bool ) ), ratingButtonGroup, SLOT( setShown( bool ) ) );

	connect( titleButton, SIGNAL( toggled( bool ) ), SLOT( buttonSwitched() ) );
	connect( ingredientButton, SIGNAL( toggled( bool ) ), SLOT( buttonSwitched() ) );
	connect( authorsButton, SIGNAL( toggled( bool ) ), SLOT( buttonSwitched() ) );
	connect( categoriesButton, SIGNAL( toggled( bool ) ), SLOT( buttonSwitched() ) );
	connect( servingsButton, SIGNAL( toggled( bool ) ), SLOT( buttonSwitched() ) );
	connect( prepTimeButton, SIGNAL( toggled( bool ) ), SLOT( buttonSwitched() ) );
	connect( instructionsButton, SIGNAL( toggled( bool ) ), SLOT( buttonSwitched() ) );
	connect( metaDataButton, SIGNAL( toggled( bool ) ), SLOT( buttonSwitched() ) );
	connect( ratingsButton, SIGNAL( toggled( bool ) ), SLOT( buttonSwitched() ) );

	connect( ratingAvgRadioButton, SIGNAL( clicked() ), this, SLOT( activateRatingOptionAvg() ) );
	connect( criterionRadioButton, SIGNAL( clicked() ), this, SLOT( activateRatingOptionCriterion() ) );

	connect( addCriteriaButton, SIGNAL( clicked() ), this, SLOT( slotAddRatingCriteria() ) );
	connect( removeCriteriaButton, SIGNAL( clicked() ), this, SLOT( slotRemoveRatingCriteria() ) );

	titleFrame->setVisible(false);
	ingredientFrame->setVisible(false);
	authorsFrame->setVisible(false);
	categoryFrame->setVisible(false);
	servingsFrame->setVisible(false);
	prepTimeFrame->setVisible(false);
	instructionsFrame->setVisible(false);
	metaDataFrame->setVisible(false);
	ratingButtonGroup->setVisible(false);

	connect( actionHandler, SIGNAL( recipeSelected( int, int ) ), SIGNAL( recipeSelected( int, int ) ) );
	connect( actionHandler, SIGNAL( recipesSelected( const QList<int> &, int ) ), SIGNAL( recipesSelected( const QList<int> &, int ) ) );

	connect( database, SIGNAL( recipeRemoved( int ) ), SLOT( removeRecipe( int ) ) );

	clear();
}

AdvancedSearchDialog::~AdvancedSearchDialog()
{}

void AdvancedSearchDialog::languageChange()
{
	titleButton->setText( QString("%1 >>").arg(i18nc("@action:button Recipe title", "Title")) );
	textLabel1_4->setText( i18nc( "@label", "Search using the following criteria:" ) );
	ingredientButton->setText( QString("%1 >>").arg(i18nc("@action:button", "Ingredients")) );
	textLabel1_2->setText( i18nc( "@label:textbox", "Uses any of:" ) );
	textLabel1->setText( i18nc( "@label:textbox", "Uses all:" ) );
	textLabel1_3->setText( i18nc( "@label:textbox", "Without:" ) );
	categoriesButton->setText( i18nc( "@action:button", "Categories >>" ) );
	textLabel1_5->setText( i18nc(  "@label:textbox","In all:" ) );
	textLabel1_3_3->setText( i18nc(  "@label:textbox","Not in:" ) );
	textLabel1_2_3->setText( i18nc(  "@label:textbox","In any of:" ) );
	authorsButton->setText( QString("%1 >>").arg(i18nc("@action:button", "Authors")) );
	textLabel1_2_4->setText( i18nc( "@label:textbox", "By any of:" ) );
	textLabel1_6->setText( i18nc( "@label:textbox", "By all:" ) );
	textLabel1_3_4->setText( i18nc( "@label:textbox", "Not by:" ) );
	servingsButton->setText( QString("%1 >>").arg(i18nc("@action:button", "Yield")) );
	enableServingsCheckBox->setText( i18nc( "@option:check", "Enabled" ) );
	servingsComboBox->clear();
	servingsComboBox->insertItem( servingsComboBox->count(), i18nc( "@item:inlistbox", "Yields at least:" ) );
	servingsComboBox->insertItem( servingsComboBox->count(), i18nc( "@item:inlistbox", "Yields at most:" ) );
	servingsComboBox->insertItem( servingsComboBox->count(), i18nc( "@item:inlistbox", "Yields about:" ) );
	prepTimeButton->setText( QString("%1 >>").arg(i18nc( "@action:button", "Preparation Time")) );
	enablePrepTimeCheckBox->setText( i18nc( "@option:check", "Enabled" ) );
	prepTimeComboBox->clear();
	prepTimeComboBox->insertItem( prepTimeComboBox->count(), i18nc( "@item:inlistbox", "Ready in at most:" ) );
	prepTimeComboBox->insertItem( prepTimeComboBox->count(), i18nc( "@item:inlistbox", "Ready in about:" ) );
	instructionsButton->setText( QString("%1 >>").arg(i18nc( "@action:button", "Instructions")) );
	metaDataButton->setText( QString("%1 >>").arg(i18nc( "@label", "Meta Data")) );
	clearButton->setText( i18nc( "@action:button Clear search criteria", "C&lear" ) );
	clearButton->setShortcut( QKeySequence( i18n( "Alt+L" ) ) );
	findButton->setText( i18nc( "@action:button Search recipes", "&Search" ) );
	ratingAvgRadioButton->setText( i18nc("@option:radio",  "By average:" ) );
	avgStarsLabel->setText( i18nc( "@label", "stars" ) );
	criterionRadioButton->setText( i18nc( "@option:radio", "By criteria:" ) );
	addCriteriaButton->setIcon( KIcon( "list-add" ) );
	removeCriteriaButton->setIcon( KIcon( "list-remove" ) );
	criteriaListView->header()->setLabel( 0, i18nc( "@title:column", "Criteria" ) );
	criteriaListView->header()->setLabel( 1, i18nc( "@title:column", "Stars" ) );
	ratingsButton->setText( QString("%1 >>").arg(i18nc("@action:button", "Ratings")) );
}

void AdvancedSearchDialog::addAction( KAction * action )
{
	actionHandler->addRecipeAction( action );
}

void AdvancedSearchDialog::showEvent( QShowEvent * event )
{
	actionHandler->selectionChangedSlot();
}

void AdvancedSearchDialog::removeRecipe( int id )
{
	Q3ListViewItemIterator iterator( resultsListView );
	while ( iterator.current() ) {
		if ( iterator.current()->rtti() == 1000 ) {
			RecipeListItem * recipe_it = ( RecipeListItem* ) iterator.current();
			if ( recipe_it->recipeID() == id )
				delete recipe_it;
		}
		++iterator;
	}
}

void AdvancedSearchDialog::clear()
{
	resultsListView->clear();
	authorsAllEdit->clear();
	authorsWithoutEdit->clear();
	authorsAnyEdit->clear();
	categoriesAllEdit->clear();
	categoriesNotEdit->clear();
	categoriesAnyEdit->clear();
	ingredientsAllEdit->clear();
	ingredientsWithoutEdit->clear();
	ingredientsAnyEdit->clear();
	titleEdit->clear();
	instructionsEdit->clear();

	createdStartDateEdit->setDate( QDate() );
	createdEndDateEdit->setDate( QDate() );
	modifiedStartDateEdit->setDate( QDate() );
	modifiedEndDateEdit->setDate( QDate() );
	accessedStartDateEdit->setDate( QDate() );
	accessedEndDateEdit->setDate( QDate() );

	servingsSpinBox->setValue( 1 );
	prepTimeEdit->setTime( QTime(0,0) );

	enablePrepTimeCheckBox->setChecked(false);
	enableServingsCheckBox->setChecked(false);

	requireAllTitle->setChecked(false);
	requireAllInstructions->setChecked(false);

	ratingAvgRadioButton->setChecked(true);
	activateRatingOptionAvg();
	avgStarsEdit->clear();
	criteriaListView->clear();
	starsWidget->clear();

	actionHandler->selectionChangedSlot();
}

void AdvancedSearchDialog::activateRatingOptionAvg()
{
	criterionFrame->setEnabled( false );
	ratingAvgFrame->setEnabled( true );
}

void AdvancedSearchDialog::activateRatingOptionCriterion()
{
	criterionFrame->setEnabled( true );
	ratingAvgFrame->setEnabled( false );
}

void AdvancedSearchDialog::buttonSwitched()
{
	const QObject *sent = sender();

	if ( sent->inherits("KPushButton") ) {
		KPushButton *pushed = (KPushButton*) sent;

		//KDE4 port
		QString suffix = ( pushed->isChecked() ) ? " <<" : " >>";
		pushed->setText( pushed->text().left( pushed->text().length() - 3 ) + suffix );
	}
}

void AdvancedSearchDialog::search()
{
	KApplication::setOverrideCursor( Qt::WaitCursor );

	//we need to load more than just the title because we'll be doing further refining of the search
	int load_items = RecipeDB::Title | RecipeDB::NamesOnly | RecipeDB::Noatime;
	if ( !authorsAllEdit->text().isEmpty() || !authorsWithoutEdit->text().isEmpty() )
		load_items |= RecipeDB::Authors;
	if ( !ingredientsAllEdit->text().isEmpty() || !ingredientsWithoutEdit->text().isEmpty() )
		load_items |= RecipeDB::Ingredients;
	if ( !categoriesAllEdit->text().isEmpty() || !categoriesNotEdit->text().isEmpty() )
		load_items |= RecipeDB::Categories;
	if ( (ratingAvgRadioButton->isChecked() && !avgStarsEdit->isEmpty()) || (criterionRadioButton->isChecked() && criteriaListView->firstChild()) )
		load_items |= RecipeDB::Ratings;

	RecipeSearchParameters parameters;

	parameters.titleKeywords = split(titleEdit->text(),true);
	parameters.requireAllTitleWords = requireAllTitle->isChecked();

	parameters.instructionsKeywords = split(instructionsEdit->text(),true);
	parameters.requireAllInstructionsWords = requireAllInstructions->isChecked();

	parameters.ingsOr = split(ingredientsAnyEdit->text(),true);
	parameters.catsOr = split(categoriesAnyEdit->text(),true);
	parameters.authorsOr = split(authorsAnyEdit->text(),true);

	if ( enablePrepTimeCheckBox->isChecked() )
		parameters.prep_time = prepTimeEdit->time();
	parameters.prep_param = prepTimeComboBox->currentIndex();

	if ( enableServingsCheckBox->isChecked() )
		parameters.servings = servingsSpinBox->value();
	parameters.servings_param = servingsComboBox->currentIndex();

	parameters.createdDateBegin = QDateTime(createdStartDateEdit->date());
	parameters.createdDateEnd = QDateTime(createdEndDateEdit->date());
	if ( parameters.createdDateEnd.date().isValid() )
		parameters.createdDateEnd = parameters.createdDateEnd.addDays(1); //we want to include the given day in the search

	parameters.modifiedDateBegin = QDateTime(modifiedStartDateEdit->date());
	parameters.modifiedDateEnd = QDateTime(modifiedEndDateEdit->date());
	if ( parameters.modifiedDateEnd.date().isValid() )
		parameters.modifiedDateEnd = parameters.modifiedDateEnd.addDays(1); //we want to include the given day in the search

	parameters.accessedDateBegin = QDateTime(accessedStartDateEdit->date());
	parameters.accessedDateEnd = QDateTime(accessedEndDateEdit->date());
	if ( parameters.accessedDateEnd.date().isValid() )
		parameters.accessedDateEnd = parameters.accessedDateEnd.addDays(1); //we want to include the given day in the search

	START_TIMER("Doing database SQL search");
	RecipeList allRecipes;
	database->search( &allRecipes, load_items, parameters );
	END_TIMER();

	/*
	 * Ideally, would be done by the above SQL query, but I have no idea how to accomplish this.
	 */

	START_TIMER("Further narrowing the search (no SQL)");
	QStringList items = split(authorsAllEdit->text());
	for ( QStringList::const_iterator author_it = items.constBegin(); author_it != items.constEnd(); ++author_it ) {
		for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
			if ( ( *it ).authorList.findByName( QRegExp(*author_it, Qt::CaseInsensitive, QRegExp::Wildcard) ).id == -1 ) {
				it = allRecipes.remove( it );
				it--;
			}
		}
	}
	items = split(authorsWithoutEdit->text());
	for ( QStringList::const_iterator author_it = items.constBegin(); author_it != items.constEnd(); ++author_it ) {
		for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
			if ( ( *it ).authorList.findByName( QRegExp(*author_it, Qt::CaseInsensitive, QRegExp::Wildcard) ).id != -1 ) {
				it = allRecipes.remove( it );
				it--;
			}
		}
	}

	//narrow down by categories
	items = split(categoriesAllEdit->text());
	for ( QStringList::const_iterator cat_it = items.constBegin(); cat_it != items.constEnd(); ++cat_it ) {
		for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
			if ( ( *it ).categoryList.findByName( QRegExp(*cat_it, Qt::CaseInsensitive, QRegExp::Wildcard) ).id == -1 ) {
				it = allRecipes.remove( it );
				it--;
			}
		}
	}
	items = split(categoriesNotEdit->text());
	for ( QStringList::const_iterator cat_it = items.constBegin(); cat_it != items.constEnd(); ++cat_it ) {
		for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
			if ( ( *it ).categoryList.findByName( QRegExp(*cat_it, Qt::CaseInsensitive, QRegExp::Wildcard) ).id != -1 ) {
				it = allRecipes.remove( it );
				it--;
			}
		}
	}

	//narrow down by ingredients
	items = split(ingredientsAllEdit->text());
	for ( QStringList::const_iterator ing_it = items.constBegin(); ing_it != items.constEnd(); ++ing_it ) {
		for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
			if ( ( *it ).ingList.findByName( QRegExp(*ing_it, Qt::CaseInsensitive, QRegExp::Wildcard) ).ingredientID == -1 ) {
				it = allRecipes.remove( it );
				it--;
			}
		}
	}
	items = split(ingredientsWithoutEdit->text());
	for ( QStringList::const_iterator ing_it = items.constBegin(); ing_it != items.constEnd(); ++ing_it ) {
		for ( RecipeList::iterator it = allRecipes.begin(); it != allRecipes.end(); ++it ) {
			if ( ( *it ).ingList.findByName( QRegExp(*ing_it, Qt::CaseInsensitive, QRegExp::Wildcard) ).ingredientID != -1 ) {
				it = allRecipes.remove( it );
				it--;
			}
		}
	}

	if ( ratingAvgRadioButton->isChecked() && !avgStarsEdit->isEmpty() ) {
		for ( RecipeList::iterator recipe_it = allRecipes.begin(); recipe_it != allRecipes.end(); ++recipe_it ) {
			double sum = 0;
			int count = 0;
	
			for ( RatingList::iterator rating_it = (*recipe_it).ratingList.begin(); rating_it != (*recipe_it).ratingList.end(); ++rating_it ) {
				sum += (*rating_it).average();
				++count;
			}
	
			if ( count != 0 ) {
				double average = sum/count;

				double stars;
				double stars_offset;
				avgStarsEdit->value(stars,stars_offset);
		                         if ( stars_offset < 1e-10 ) { //if an exact amount is given, search for an amount within 0.5 of what is given
		                                 //we can get negatives here, but it really doesn't matter
		                                 stars = stars-0.5;
		                                 stars_offset = 1.0;
		                         }


				kDebug()<<"average for "<<(*recipe_it).title<<" "<<average;
				if ( average < stars || average > stars + stars_offset ) {
					recipe_it = allRecipes.remove( recipe_it );
					recipe_it--;
				}
			}
			else {
				recipe_it = allRecipes.remove( recipe_it );
				recipe_it--;
			}
		}
	}

	//TODO: Clean this up and/or do it more efficiently
	if ( criterionRadioButton->isChecked() && criteriaListView->firstChild() ) {
		for ( RecipeList::iterator recipe_it = allRecipes.begin(); recipe_it != allRecipes.end(); ++recipe_it ) {
			QMap< int, double > idSumMap;
			QMap< int, int > idCountMap;

			for ( RatingList::const_iterator rating_it = (*recipe_it).ratingList.constBegin(); rating_it != (*recipe_it).ratingList.constEnd(); ++rating_it ) {
				for ( RatingCriteriaList::const_iterator rc_it = (*rating_it).ratingCriteriaList.constBegin(); rc_it != (*rating_it).ratingCriteriaList.constEnd(); ++rc_it ) {
					QMap< int, double >::iterator sum_it = idSumMap.find((*rc_it).id);
					if ( sum_it == idSumMap.end() )
						sum_it = idSumMap.insert((*rc_it).id,0);
					(*sum_it) += (*rc_it).stars;

					QMap< int, int >::iterator count_it = idCountMap.find((*rc_it).id);
					if ( count_it == idCountMap.end() )
						count_it = idCountMap.insert((*rc_it).id,0);
					(*count_it)++;
				}
			}

			for ( Q3ListViewItem *item = criteriaListView->firstChild(); item; item = item->nextSibling() ) {
				Ingredient i; i.setAmount( item->text(1) );
				double stars = i.amount;
				double stars_offset = i.amount_offset;

				if ( stars_offset < 1e-10 ) { //if an exact amount is given, search for an amount within 0.5 of what is given
					//we can get negatives here, but it really doesn't matter
					stars = stars-0.5;
					stars_offset = 1.0;
				}

				int id = item->text(2).toInt();

				QMap< int, double >::iterator sum_it = idSumMap.find(id);
				if ( sum_it != idSumMap.end() ) {
					QMap< int, int >::iterator count_it = idCountMap.find(id);
					double average = (*sum_it)/(*count_it);

					if ( average < stars || average > stars + stars_offset ) {
						recipe_it = allRecipes.remove( recipe_it );
						recipe_it--;
						break;
					}
				}
				else {
					recipe_it = allRecipes.remove( recipe_it );
					recipe_it--;
					break;
				}
			}
		}
	}
	END_TIMER();


	//now display the recipes left
	resultsListView->clear();
	actionHandler->selectionChangedSlot();
	for ( RecipeList::const_iterator it = allRecipes.constBegin(); it != allRecipes.constEnd(); ++it ) {
		( void ) new RecipeListItem( resultsListView, *it );
	}

	if ( !resultsListView->firstChild() ) {
		( void ) new Q3ListViewItem( resultsListView, "--- "+i18nc("@item:inlistbox", "No matching recipes found")+" ---");
	}

	KApplication::restoreOverrideCursor();
}

QStringList AdvancedSearchDialog::split( const QString &text, bool sql_wildcards ) const
{
	QStringList result;

	// To keep quoted words together, first split on quotes,
	// and then split again on the even numbered items
	
	QStringList temp;
	if ( text.isEmpty() )
		temp = QStringList();
	else
		temp = text.split( '"', QString::KeepEmptyParts );

	for ( int i = 0; i < temp.count(); ++i ) {
		if ( i & 1 ) //odd
			result += temp[i].trimmed();
		else		  //even
			result += temp[i].split(' ',QString::SkipEmptyParts);
	}

	if ( sql_wildcards ) {
		for ( QStringList::iterator it = result.begin(); it != result.end(); ++it ) {
			(*it).replace("%","\\%");
			(*it).replace("_","\\_");

			(*it).replace("*","%");
			(*it).replace("?","_");
		}
	}

	return result;
}

void AdvancedSearchDialog::slotAddRatingCriteria()
{
	Q3ListViewItem * it = new Q3ListViewItem(criteriaListView,criteriaComboBox->currentText());

	MixedNumber stars;
	double stars_offset;
	starsWidget->value(stars,stars_offset);
	QString stars_str = stars.toString();
	if ( stars_offset > 0 )
		stars_str += "-"+MixedNumber( stars + stars_offset ).toString();
	else if ( stars.toDouble() <= 1e-10 )
		stars_str = "";

	it->setText(1,stars_str);
	it->setText(2,QString::number(criteriaComboBox->criteriaID(criteriaComboBox->currentIndex())));
}

void AdvancedSearchDialog::slotRemoveRatingCriteria()
{
	delete criteriaListView->selectedItem();
}

#include "advancedsearchdialog.moc"
