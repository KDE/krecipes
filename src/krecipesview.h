/*
 * Copyright (C) 2003 Unai Garro <uga@ee.ed.ac.uk>
 */

#ifndef _KRECIPESVIEW_H_
#define _KRECIPESVIEW_H_

#include <qwidget.h>
#include <qvbox.h>
#include "recipedb.h"
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qsplitter.h>
#include <kparts/part.h>
#include <krecipesiface.h>
#include <qwidgetstack.h>
#include "recipeinputdialog.h"
#include "recipeviewdialog.h"

class QPainter;

/**
 * This is the main view class for Krecipes.  Most of the non-menu,
 * non-toolbar, and non-statusbar (e.g., non frame) GUI code should go
 * here.
 *
 * This krecipes uses an HTML component as an example.
 *
 * @short Main view
 * @author Unai Garro <uga@ee.ed.ac.uk>
 * @version 0.1
 */
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

private slots:
    void slotSetTitle(const QString& title);
    void slotSetPanel(int);

private:
	RecipeDB *database;
	QSplitter *splitter;
	QButtonGroup *leftPanel;
	QWidgetStack *rightPanel;
	QPushButton *boton1;
	QPushButton *boton2;
	QPushButton *boton3;
	QPushButton *boton4;
	QPushButton *boton5;

public:
	RecipeInputDialog *inputPanel;
	RecipeViewDialog *viewPanel;

signals:
	void enableSaveOption(bool en);
public slots:
	void save(void);
};

#endif // _KRECIPESVIEW_H_
