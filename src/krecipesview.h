/*
 * Copyright (C) 2003 Unai Garro <ugarro@users.sourceforge.net>
 */

#ifndef _KRECIPESVIEW_H_
#define _KRECIPESVIEW_H_

#include <qwidget.h>
#include <qvbox.h>
#include "recipedb.h"
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qsplitter.h>

#include <kiconloader.h>
#include <kparts/part.h>
#include <krecipesiface.h>
#include <qwidgetstack.h>
#include "recipeinputdialog.h"
#include "recipeviewdialog.h"
#include "selectrecipedialog.h"
#include "ingredientsdialog.h"
#include "propertiesdialog.h"
#include "shoppinglistdialog.h"
#include "dietwizarddialog.h"


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
 * @version 0.2
 */


 // Some constants

 enum panels{SelectP=0,ShoppingP=1, IngredientsP=2, PropertiesP=3, UnitsP=4, DietWizardP=10};

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

	// public methods
	void createNewRecipe(void);
	void createNewElement(void);

private:

	// Internal methods
	void wizard(void);

	// Widgets
	KIconLoader *il;
	QSplitter *splitter;
	QButtonGroup *leftPanel;
	QWidgetStack *rightPanel;
	QPushButton *button1;
	QPushButton *button2;
	QPushButton *button3;
	QPushButton *button4;
	QPushButton *button5;
	QPushButton *button6;
	QPushButton *button7;
	// Internal variables
	RecipeDB *database;

signals:
	void enableSaveOption(bool en);
public slots:
	void save(void);

private slots:
    void slotSetTitle(const QString& title);
    void slotSetPanel(int);
    void slotSetDietWizardPanel(void);
    void actionRecipe(int recipeID, int action);


};

#endif // _KRECIPESVIEW_H_
