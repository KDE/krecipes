/*
 * Copyright (C) 2003 Unai Garro <ugarro@users.sourceforge.net>
 */

#ifndef _KRECIPESVIEW_H_
#define _KRECIPESVIEW_H_

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
#include <krecipesiface.h>
#include <kstyle.h>
#include <ktextbrowser.h>

#include "importers/baseimporter.h"
#include "DBBackend/recipedb.h"


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
 enum mainPanels{SelectP=0,ShoppingP,DietP,MatcherP};
 enum dataPanels{IngredientsP=1, PropertiesP, UnitsP, PrepMethodsP, CategoriesP, AuthorsP, ContextHelp, RecipeEdit };


 // Class KrecipesView
class KrecipesView : public QVBox, public KrecipesIface
{
    Q_OBJECT
public:
	/**
	 * Default constructor
	 */
    KrecipesView(QWidget *parent);

	/**
	 * Destructor
	 */
    virtual ~KrecipesView();

    /**
     * Print this view to any medium -- paper or not
     */
    void print();
    virtual void show (void); //Needed to make sure that the raise() is done after the construction of all the widgets, otherwise childEvent in the PanelDeco is called only _after_ the raise(), and can't be shown.

signals:
    /**
     * Use this signal to change the content of the statusbar
     */
    void signalChangeStatusbar(const QString& text);

    /**
     * Use this signal to change the content of the caption
     */
    void signalChangeCaption(const QString& text);


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
	void createNewRecipe(void);
	void createNewElement(void);

	void import( BaseImporter &importer ){ importer.import(database); }

private:

	// Internal methods
	QString checkCorrectDBType(KConfig *config);
	void initializeData(const QString &host,const QString &dbName, const QString &user,const QString &pass);
	void initDatabase(KConfig *config);
	void questionRerunWizard(const QString &message, const QString &error="");
	void setupUserPermissions(const QString &host, const QString &client, const QString &dbName,const QString &newUser,const QString &newPass,const QString &adminUser=QString::null,const QString &adminPass=QString::null);
	void wizard(bool force=false);
	
	
	
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
	RecipeDB *database;
	QString dbType;

  // i18n
  void translate();
  

signals:
	void enableSaveOption(bool en);

public slots:
	void save(void);
  void exportRecipe();

private slots:
    void actionRecipe(int recipeID, int action);
    void actionRecipes(const QValueList<int> &ids, int action);
    void addRecipeButton(QWidget *w, QString title);
    void closeRecipe(void);
    void showRecipe(int recipeID);
    void showRecipes( const QValueList<int> &recipeIDs);
    void slotSetTitle(const QString& title);
    void slotSetPanel(int);
    void slotSetDietWizardPanel(void);
    void switchToRecipe(void);
    void setContextHelp(int);
    void createShoppingListFromDiet(void);
    void moveTipButton(int,int);
    void resizeRightPane(int lpw,int lph);
};


#endif // _KRECIPESVIEW_H_
