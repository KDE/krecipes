/*
 * Copyright (C) 2003 Unai Garro <ugarro@users.sourceforge.net>
 */

#ifndef _KRECIPESVIEW_H_
#define _KRECIPESVIEW_H_

#include <qwidget.h>
#include <qvbox.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qsplitter.h>
#include <qevent.h>
#include <qtooltip.h>
#include <qbitmap.h>
#include <qwidgetstack.h>

#include <kiconloader.h>
#include <kpixmap.h>
#include <kpixmapeffect.h>
#include <kimageeffect.h>
#include <kparts/part.h>
#include <krecipesiface.h>
#include <kstyle.h>

#include "importers/baseimporter.h"
#include "DBBackend/recipedb.h"

class RecipeInputDialog;
class RecipeViewDialog;
class SelectRecipeDialog;
class IngredientsDialog;
class PropertiesDialog;
class ShoppingListDialog;
class DietWizardDialog;
class CategoriesEditorDialog;
class AuthorsDialog;
class UnitsDialog;

class QPainter;

/**
 * This is the main view class for Krecipes.  Most of the non-menu,
 * non-toolbar, and non-statusbar (e.g., non frame) GUI code should go
 * here.
 *
 * This krecipes uses an HTML component as an example.
 *
 * @short Main view
 * @author Unai Garro <ugarro@users.sourceforge.net>
 * @version 0.3
 */


 // Declarations

 class MenuButton;
 class DualProgressDialog;

 // Some constants
 enum panels{SelectP=0,ShoppingP=1, IngredientsP=2, PropertiesP=3, UnitsP=4, CategoriesP=5,AuthorsP=6, DietWizardP=10};


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
    void print(QPainter *, int height, int width);

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

	// public methods
	void createNewRecipe(void);
	void createNewElement(void);

	void import( BaseImporter &importer, DualProgressDialog *progress_dialog ){ importer.import(database, progress_dialog); }

private:

	// Internal methods
	void wizard(void);
	void initializeData(const QString &host,const QString &dbName, const QString &user,const QString &pass);
	void setupUserPermissions(const QString &host, const QString &client, const QString &dbName,const QString &newUser,const QString &newPass,const QString &adminUser=QString::null,const QString &adminPass=QString::null);

	// Widgets
	KIconLoader *il;
	QSplitter *splitter;
	QButtonGroup *leftPanel;
	QWidgetStack *rightPanel;
  QPtrList<MenuButton> *buttonsList;
	MenuButton *button0;
	MenuButton *button1;
	MenuButton *button2;
	MenuButton *button3;
	MenuButton *button4;
	MenuButton *button5;
	MenuButton *button6;
	MenuButton *button7;

	MenuButton *recipeButton;
	QWidget *recipeWidget;

	// Internal variables
	RecipeDB *database;

  // i18n
  void translate();

signals:
	void enableSaveOption(bool en);

public slots:
	void save(void);

private slots:
    void actionRecipe(int recipeID, int action);
    void addRecipeButton(QWidget *w, QString title);
    void closeRecipe(void);
    void resizeButtons();
    void showRecipe(int recipeID);
    void slotSetTitle(const QString& title);
    void slotSetPanel(int);
    void slotSetDietWizardPanel(void);
    void switchToRecipe(void);

};


// Class MenuButton

class MenuButton:public QPushButton
{

Q_OBJECT

public:
	MenuButton(QWidget *parent,const char *name=0);
	~MenuButton();
 QPixmap* icon; // icon

public slots:
	void setTitle(const QString &title);
  void drawButton( QPainter * );
  void setIconSet( QIconSet );
  void enterEvent( QEvent * );
  void leaveEvent( QEvent * );
  void focusInEvent( QFocusEvent * );
  void focusOutEvent( QFocusEvent * );
};

#endif // _KRECIPESVIEW_H_
