/***************************************************************************
*   Copyright (C) 2003-2005 by                                            *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   Copyright (C) 2006 Jason Kivlighn (jkivlighn@gmail.com)               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef INGREDIENTINPUTWIDGET_H
#define INGREDIENTINPUTWIDGET_H

#include <q3vbox.h>
//Added by qt3to4:
#include <QLabel>
#include <Q3ValueList>

#include "datablocks/unit.h"

class Q3VBox;
class Q3HBox;
class QLabel;
class Q3ButtonGroup;
class Q3WidgetStack;
class Q3GroupBox;
class QRadioButton;
class QCheckBox;

class KComboBox;

class IngredientComboBox;
class HeaderComboBox;
class PrepMethodComboBox;
class RecipeDB;
class FractionInput;
class Ingredient;
class Element;
class ElementList;
class IngredientInput;

class IngredientInputWidget : public Q3VBox
{
Q_OBJECT

public:
	IngredientInputWidget( RecipeDB *db, QWidget *parent );
	~IngredientInputWidget();

	void clear();

	static int createNewIngredientIfNecessary( const QString &ing, RecipeDB *db );
	static int createNewUnitIfNecessary( const QString &unit, bool plural, int ingredient_id, Unit &new_unit, RecipeDB *db );
	static Q3ValueList<int> createNewPrepIfNecessary( const ElementList &prepMethods, RecipeDB *db );
	static int createNewGroupIfNecessary( const QString &group, RecipeDB *db );

signals:
	void ingredientEntered( const Ingredient &ing );

	void headerEntered( const Element &header );

public slots:
	void addIngredient();

private slots:
	void updateInputs(bool,IngredientInput*);

private:
	/** Reloads lists of units, ingredients, and preparation methods */
	void reloadCombos();

	void checkIfNewUnits();

	RecipeDB *database;

	Q3ValueList<IngredientInput*> m_ingInputs;
};

class IngredientInput : public Q3HBox
{
Q_OBJECT

public:
	IngredientInput( RecipeDB *db, QWidget *parent, bool allowHeader = true );
	~IngredientInput();

	void clear();
	bool isInputValid();

	bool isHeader() const;
	Ingredient ingredient() const;
	QString header() const;

	void reloadCombos();
	void enableHeader( bool );
	void updateTabOrder();

signals:
	void addIngredient();
	void orToggled(bool,IngredientInput*);

private slots:
	void loadUnitListCombo();
	void signalIngredient();
	void typeButtonClicked( int );
	void slotIngredientBoxLostFocus();
	void slotUnitBoxLostFocus();
	void slotPrepMethodBoxLostFocus();
	void orToggled(bool);

private:
	bool checkBounds();
	bool checkAmountEdit();

	RecipeDB *database;
	UnitList *unitComboList;

	QCheckBox *orButton;
	Q3GroupBox *ingredientGBox;
	QLabel *amountLabel;
	FractionInput* amountEdit;
	QLabel *unitLabel;
	KComboBox* unitBox;
	QLabel *prepMethodLabel;
	PrepMethodComboBox* prepMethodBox;
	QLabel *ingredientLabel;
	IngredientComboBox* ingredientBox;
	HeaderComboBox* headerBox;
	Q3WidgetStack *header_ing_stack;
	Q3ButtonGroup *typeButtonGrp;
};

#endif //INGREDIENTINPUTWIDGET_H
