/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef KRECIPESVIEW_H
#define KRECIPESVIEW_H

#include <QPushButton>

#include <q3valuelist.h>

#include <qwidget.h>
#include <q3widgetstack.h>
//Added by qt3to4:
#include <QList>

#include <kiconloader.h>

#include <kparts/part.h>
#include <kstyle.h>
#include <ktextbrowser.h>
#include <kvbox.h>

#include "importers/baseimporter.h"
#include "backends/recipedb.h"
#include "actionshandlers/recipeactionshandler.h"

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
class RecipeInputDialog;
class RecipeViewDialog;
class SelectRecipeDialog;
class ShoppingListDialog;
class UnitsDialog;
class QFrame;
class KAction;
typedef Q3ValueList <Menu>::Iterator MenuId;


/**
 * This is the main view class for Krecipes.  Most of the non-menu,
 * non-toolbar, and non-statusbar (e.g., non frame) GUI code should go
 * here.
 *
 * This krecipes uses an HTML component as an example.
 *
 * @short Main view
 * @author Unai Garro <ugarro@gmail.com>            
 * @version 0.4
 */


// Declarations



// Some constants
enum KrePanel {SelectP = 0, ShoppingP, DietP, MatcherP, IngredientsP, PropertiesP, UnitsP, PrepMethodsP, CategoriesP, AuthorsP, RecipeEdit, RecipeView };


// Class KrecipesView
class KrecipesView : public QWidget
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

	virtual QString currentDatabase() const;
	RecipeDB *database;

	/**
	 * Print this view to any medium -- paper or not
	 */
	void printRequested();

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
	void exportRecipes( const QList<int> &ids );

	// actions
	void addSelectRecipeAction( KAction * action );
	void addFindRecipeAction( KAction * action );
	void addCategoryAction( KAction * action );
	void addIngredientMatcherAction( KAction * action );
	void setCategorizeAction( KAction * action );
	void setRemoveFromCategoryAction( KAction * action );
	void addElementAction( KAction * action ); /* Element is anything except a recipe */
	void addCategoriesPanelAction( KAction * action );
	void setCategoryPasteAction( KAction * action );
	void setPasteAsSubcategoryAction( KAction * action );

private:

	// Internal methods
	QString checkCorrectDBType( KConfigGroup &config );
	void initializeData( const QString &host, const QString &dbName, const QString &user, const QString &pass, int port );
	void initDatabase(  );
	bool questionRerunWizard( const QString &message, const QString &errormsg = "", RecipeDB::Error error = RecipeDB::NoError );
	void setupUserPermissions( const QString &host, const QString &client, const QString &dbName, const QString &newUser, const QString &newPass, const QString &adminUser = QString(), const QString &adminPass = QString(), int port = 0 );
	void wizard( bool force = false );



	// Widgets
	KHBox *splitter;
	QFrame *leftPanelFrame;
	KreMenu *leftPanel;
	MenuId dataMenu;
	PanelDeco *rightPanel;
	QList<KreMenuButton*> buttonsList;
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
	QPushButton* contextButton;

	KreMenuButton *recipeButton;
	QWidget *recipeWidget;

	// Actions
	KAction * showRecipeAction;

	// Internal variables
	QString dbType;
	KrePanel m_activePanel;
	KrePanel m_previousActivePanel;

	QMap<QWidget*, KrePanel> panelMap;

	// We need this dummy actions handler instance to call
        // RecipeActionsHandler::print(...)
        // which can't be static for technical reasons.
	RecipeActionsHandler m_actionshandler;

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
	void activateContextHelp();
	void cut();
	void paste();
	void pasteAsSubcategory();
	void enableSaveOptionSlot( bool enabled );

private slots:
	void actionRecipe( int recipeID, int action );
	void actionRecipes( const QList<int> &ids, int action );
	void addRecipeButton( QWidget *w, const QString &title );
	void closeRecipe( void );
	void showRecipe( int recipeID );
	void showRecipes( const QList<int> &recipeIDs );
	void slotSetTitle( const QString& title );
	void slotSetPanel( KrePanel, bool highlightLeftButton=false );
	void switchToRecipe( void );
	void createShoppingListFromDiet( void );
	void moveTipButton( int, int );
	void resizeRightPane( int lpw, int lph );
	void panelRaised( QWidget *w, QWidget *old_w );
	void showCurrentRecipes();
	void editRecipe();
	void addToShoppingList();
	void categorizeCurrentRecipe();
	void removeFromCategory();
	void createNewElement();
	void renameCurrentElement();
	void deleteCurrentElements();
	void expandAll();
	void collapseAll();
};


#endif // KRECIPESVIEW_H
