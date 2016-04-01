/***************************************************************************
*   Copyright © 2003-2005 Unai Garro <ugarro@gmail.com>                   *
*   Copyright © 2003-2005 Cyril Bosselut <bosselut@b1project.com>         *
*   Copyright © 2003-2005 Jason Kivlighn <jkivlighn@gmail.com>            *
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef RECIPEINPUTDIALOG_H
#define RECIPEINPUTDIALOG_H

#include <KVBox>

class RecipeGeneralInfoEditor;
class IngredientsEditor;
class KreTextEdit;
class RatingListEditor;
class RecipeDB;
class Recipe;

class KTabWidget;

class QFrame;
class QToolButton;

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

	RecipeGeneralInfoEditor * m_recipeGeneralInfoEditor;

	IngredientsEditor * ingredientsEditor;

	QFrame* instructionsTab;
	KreTextEdit* instructionsEdit;

	RatingListEditor * ratingListEditor;

	//Function buttons
	QFrame* functionsBox;
	QToolButton* saveButton;
	QToolButton* closeButton;
	QToolButton* showButton;
	QToolButton* resizeButton;

	// Internal functions
	void saveRecipe( void );

	// Signals & Slots

private slots:
	void recipeChanged( void );
	void recipeChanged( const QString &t );
	void enableChangedSignal( bool en = true );
	void enableSaveButton( bool enabled );
	void closeOptions( void );
	void recipeRemoved( int id );

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

#endif
