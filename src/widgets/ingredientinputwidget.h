/***************************************************************************
*   Copyright © 2003-2005 Unai Garro <ugarro@gmail.com>                   *
*   Copyright © 2003-2005 Cyril Bosselut <bosselut@b1project.com>         *
*   Copyright © 2003-2006 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef INGREDIENTINPUTWIDGET_H
#define INGREDIENTINPUTWIDGET_H


//Added by qt3to4:
#include <QLabel>
#include <QList>
#include <kvbox.h>

#include "datablocks/unit.h"

class KVBox;
class KHBox;
class QLabel;
class Q3ButtonGroup;
class QStackedWidget;
class Q3GroupBox;
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

class IngredientInputWidget : public KVBox
{
Q_OBJECT

public:
	IngredientInputWidget( RecipeDB *db, QWidget *parent );
	~IngredientInputWidget();

	void clear();

	static int createNewIngredientIfNecessary( const QString &ing, RecipeDB *db );
	static int createNewUnitIfNecessary( const QString &unit, bool plural, int ingredient_id, Unit &new_unit, RecipeDB *db );
	static QList<int> createNewPrepIfNecessary( const ElementList &prepMethods, RecipeDB *db );
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

	QList<IngredientInput*> m_ingInputs;
};

class IngredientInput : public KHBox
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
	QStackedWidget *header_ing_stack;
	Q3ButtonGroup *typeButtonGrp;
};

#endif //INGREDIENTINPUTWIDGET_H
