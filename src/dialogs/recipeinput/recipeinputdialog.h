/***************************************************************************
*   Copyright © 2003-2005 Unai Garro <ugarro@gmail.com>                   *
*   Copyright © 2003-2005 Cyril Bosselut <bosselut@b1project.com>         *
*   Copyright © 2003-2005 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef RECIPEINPUTDIALOG_H
#define RECIPEINPUTDIALOG_H

#include <kdialog.h>
#include <ktextedit.h>
#include <klineedit.h>
#include <kcombobox.h>
#include <k3listview.h>
#include <knuminput.h>
#include <kpushbutton.h>
#include <kled.h>

#include <QLabel>
#include <qmap.h>
#include <QToolButton>

#include <QDateTimeEdit>
#include <QDropEvent>
#include <QPixmap>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <kvbox.h>

#include "datablocks/elementlist.h"
#include "datablocks/rating.h"
class KTabWidget;
class QDateTimeEdit;
class QTextEdit;
class KTextBrowser;

class KreTextEdit;
class KWidgetListbox;
class KDialog;
class KPushButton;

class ImageDropLabel;
class Recipe;
class RecipeDB;
class FractionInput;
class Ingredient;
class Rating;
class IngredientInputWidget;
class ClickableLed;

#include "ui_ratingdisplaywidget.h"

class RatingDisplayWidget : public QWidget, public Ui::RatingDisplayWidget
{
public:
	RatingDisplayWidget( QWidget *parent ) : QWidget( parent ) {
		setupUi( this );
	}
RatingList::iterator rating_it;
};


/**
@author Unai Garro
*/
class RecipeInputDialog: public KVBox
{
	Q_OBJECT

public:

	RecipeInputDialog( QWidget* parent, RecipeDB* db );
	void loadRecipe( int recipeID );
	~RecipeInputDialog();
	void newRecipe( void );
	bool everythingSaved();
	void reload( void );
	int loadedRecipeID() const;
	void reloadCheckSpelling();

private:

	// Internal Data
	Recipe *loadedRecipe; //Loaded Recipe
	RecipeDB *database;
	bool changedSignalEnabled;
	bool unsavedChanges;

	// Widgets
	KTabWidget* tabWidget;
	QFrame* recipeTab;
	QFrame* instructionsTab;

	//Recipe Photo
	ImageDropLabel *photoLabel;
	QPixmap sourcePhoto;
	KPushButton *changePhotoButton;

	//Recipe Body
	KreTextEdit* instructionsEdit;
	QLabel* titleLabel;
	KLineEdit* titleEdit;

	//Additional recipe data
	QLabel* yieldLabel;
	FractionInput* yieldNumInput;
	KLineEdit* yieldTypeEdit;
	QDateTimeEdit *prepTimeEdit;
	QLabel* authorLabel;
	KLineEdit* authorShow;
	KPushButton* addAuthorButton;
	QLabel* categoryLabel;
	KLineEdit* categoryShow;
	KPushButton* addCategoryButton;

	//Ingredient inputs
	K3ListView* ingredientList;
	QFrame *ingredientsTab;
	IngredientInputWidget *ingInput;
	ClickableLed *propertyStatusLed;
	QLabel *propertyStatusLabel;
	KPushButton *propertyStatusButton;
	//QTextEdit *statusTextView;
	KTextBrowser *statusTextView;
	KDialog *propertyStatusDialog;

	// Buttons to move ingredients up & down...
	KPushButton* upButton;
	KPushButton* downButton;
	KPushButton* removeButton;
	KPushButton* addButton;
	KPushButton* ingParserButton;

	//Function buttons
	QFrame* functionsBox;
	QToolButton* saveButton;
	QToolButton* closeButton;
	QToolButton* showButton;
	QToolButton* resizeButton;


	KWidgetListbox *ratingListDisplayWidget;

	QMap<int,QString> propertyStatusMapRed;
	QMap<int,QString> propertyStatusMapYellow;

	// Internal functions
	int createNewYieldIfNecessary( const QString &yield );
	void saveRecipe( void );
	void showCategories( void );
	void showAuthors( void );
	int ingItemIndex( Q3ListView *listview, const Q3ListViewItem *item ) const;
	void addRating( const Rating &rating, RatingDisplayWidget *item );
	QString statusMessage() const;
	QString conversionPath( const QString &ingUnit, const QString &toUnit, const QString &fromUnit, const QString &propUnit ) const;

	// Signals & Slots

private slots:
	void changePhoto( void );
	void savePhotoAs( void );
	void clearPhoto( void );
	void moveIngredientUp( void );
	void moveIngredientDown( void );
	void removeIngredient( void );
	void syncListView( Q3ListViewItem* it, const QString &new_text, int col );
	void recipeChanged( void );
	void recipeChanged( const QString &t );
	void enableChangedSignal( bool en = true );
	void addAuthor( void );
	void enableSaveButton( bool enabled );
	void closeOptions( void );
	void prepTitleChanged( const QString &title );
	void recipeRemoved( int id );
	void slotIngredientParser();
	void slotAddRating();
	void slotEditRating();
	void slotRemoveRating();
	void addIngredient( const Ingredient &ing, bool noHeader = false );
	void addIngredientHeader( const Element &header );
	void updatePropertyStatus();
	void updatePropertyStatus( const Ingredient &ing, bool updateIndicator );
	void showStatusIndicator();
	void statusLinkClicked( const QUrl &url );

public slots:
	bool save ( void ); // Activated when krecipes.cpp sends signal save()
	void resizeRecipe( void );
	void addCategory( void );
	void showRecipe( void );

signals:
	void changed( void );
	void closeRecipe( void );
	void createButton( QWidget* w, const QString &title );
	void enableSaveOption( bool en = true );
	void showRecipe( int recipeID ); //Indicates krecipesview to show it
	void titleChanged( const QString &title );


};

class ClickableLed : public KLed
{
Q_OBJECT

public:
	ClickableLed( QWidget *parent );

protected:
	virtual void mouseReleaseEvent( QMouseEvent* );

signals:
	void clicked();
};

class ImageDropLabel : public QLabel
{
	Q_OBJECT

public:
	ImageDropLabel( QWidget *parent, QPixmap &_sourcePhoto );

signals:
	void changed();

protected:
	void dragEnterEvent( QDragEnterEvent* event );
	void dropEvent( QDropEvent* event );

private:
	QPixmap &sourcePhoto;
};

#endif
