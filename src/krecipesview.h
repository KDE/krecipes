/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro <ugarro@users.sourceforge.net>                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef KRECIPESVIEW_H
#define KRECIPESVIEW_H

#include <qbitmap.h>
#include <qbuttongroup.h>
#include <qevent.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qhbox.h>
#include <qvaluelist.h>
#include <qvbox.h>
#include <qwidget.h>
#include <qwidgetstack.h>

#include <kiconloader.h>
#include <kpixmap.h>
#include <kpixmapeffect.h>
#include <kimageeffect.h>
#include <kparts/part.h>
#include <kstyle.h>
#include <ktextbrowser.h>

#include "importers/baseimporter.h"
#include "backends/recipedb.h"
#include "krecipesiface.h"

class AuthorsDialog;
class PrepMethodsDialog;
class CategoriesEditorDialog;
class DietWizardDialog;
class IngredientsDialog;
class Menu;
class KreMenu;
class KreMenuButton;
class IngredientMatcherDialog;
class PanelDeco;
class PropertiesDialog;
class QPainter;
class RecipeInputDialog;
class RecipeViewDialog;
class SelectRecipeDialog;
class ShoppingListDialog;
class UnitsDialog;
typedef QValueList <Menu>::Iterator MenuId;


/**
 * This is the main view class for Krecipes.  Most of the non-menu,
 * non-toolbar, and non-statusbar (e.g., non frame) GUI code should go
 * here.
 *
 * This krecipes uses an HTML component as an example.
 *
 * @short Main view
 * @author Unai Garro <ugarro@users.sourceforge.net>
 * @version 0.4
 */


// Declarations



// Some constants
typedef enum KrePanel {SelectP = 0, ShoppingP, DietP, MatcherP, IngredientsP, PropertiesP, UnitsP, PrepMethodsP, CategoriesP, AuthorsP, ContextHelp, RecipeEdit, RecipeView };


// Class KrecipesView
class KrecipesView : public QVBox, virtual public KrecipesIface
{
	Q_OBJECT
public:
	/**
	 * Default constructor
	 */
	KrecipesView( QWidget *parent );

	/**
	 * Destructor
	 */
	virtual ~KrecipesView();

	virtual DCOPRef currentDatabase() const;
	RecipeDB *database;

	/**
	 * Print this view to any medium -- paper or not
	 */
	void print();

	virtual void show ( void ); //Needed to make sure that the raise() is done after the construction of all the widgets, otherwise childEvent in the PanelDeco is called only _after_ the raise(), and can't be shown.

signals:
	/**
	 * Use this signal to change the content of the statusbar
	 */
	void signalChangeStatusbar( const QString& text );

	/**
	 * Use this signal to change the content of the caption
	 */
	void signalChangeCaption( const QString& text );

	void panelShown( KrePanel, bool );


public:

	// public widgets
	RecipeInputDialog *inputPanel;
	RecipeViewDialog *viewPanel;
	SelectRecipeDialog *selectPanel;
	IngredientsDialog *ingredientsPanel;
	PropertiesDialog *propertiesPanel;
	UnitsDialog* unitsPanel;
	ShoppingListDialog* shoppingListPanel;
	DietWizardDialog* dietPanel;
	CategoriesEditorDialog *categoriesPanel;
	AuthorsDialog *authorsPanel;
	PrepMethodsDialog *prepMethodsPanel;
	IngredientMatcherDialog *ingredientMatcherPanel;

	// public methods
	void createNewRecipe( void );
	void createNewElement( void );

	void exportRecipes( const QValueList<int> &ids );

private:

	// Internal methods
	QString checkCorrectDBType( KConfig *config );
	void initializeData( const QString &host, const QString &dbName, const QString &user, const QString &pass );
	void initDatabase( KConfig *config );
	bool questionRerunWizard( const QString &message, const QString &error = "" );
	void setupUserPermissions( const QString &host, const QString &client, const QString &dbName, const QString &newUser, const QString &newPass, const QString &adminUser = QString::null, const QString &adminPass = QString::null );
	void wizard( bool force = false );



	// Widgets
	QHBox *splitter;
	KreMenu *leftPanel;
	MenuId dataMenu;
	PanelDeco *rightPanel;
	QPtrList<KreMenuButton> *buttonsList;
	KreMenuButton *button0;
	KreMenuButton *button1;
	KreMenuButton *button2;
	KreMenuButton *button3;
	KreMenuButton *button4;
	KreMenuButton *button5;
	KreMenuButton *button6;
	KreMenuButton *button7;
	KreMenuButton *button8;
	KreMenuButton *button9;
	QWidget* contextHelp;
	QPushButton* contextButton;
	QPushButton* contextClose;
	QLabel* contextTitle;
	KTextBrowser* contextText;

	KreMenuButton *recipeButton;
	QWidget *recipeWidget;

	// Internal variables
	QString dbType;

	QMap<QWidget*, KrePanel> panelMap;

	// i18n
	void translate();


signals:
	void enableSaveOption( bool en );
	void recipeSelected( bool );

public slots:
	bool save( void );
	void exportRecipe();
	void exportToClipboard();
	void reloadDisplay();
	virtual void reload();

private slots:
	void actionRecipe( int recipeID, int action );
	void actionRecipes( const QValueList<int> &ids, int action );
	void addRecipeButton( QWidget *w, const QString &title );
	void closeRecipe( void );
	void showRecipe( int recipeID );
	void showRecipes( const QValueList<int> &recipeIDs );
	void slotSetTitle( const QString& title );
	void slotSetPanel( KrePanel );
	void switchToRecipe( void );
	void setContextHelp( KrePanel );
	void createShoppingListFromDiet( void );
	void moveTipButton( int, int );
	void resizeRightPane( int lpw, int lph );
	void panelRaised( QWidget *w, QWidget *old_w );
	void editRecipe();
};


#endif // KRECIPESVIEW_H
