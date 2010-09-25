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

#include "ingredientinputwidget.h"

#include <QLabel>
#include <QStackedWidget>


#include <q3groupbox.h>
#include <q3buttongroup.h>
#include <QRadioButton>
#include <QCheckBox>
#include <QList>
#include <QPointer>

#include <kcombobox.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kvbox.h>

#include "backends/recipedb.h"
#include "datablocks/unit.h"
#include "widgets/fractioninput.h"
#include "widgets/ingredientcombobox.h"
#include "widgets/headercombobox.h"
#include "widgets/prepmethodcombobox.h"
#include "dialogs/createunitdialog.h"

#include "profiling.h"

IngredientInput::IngredientInput( RecipeDB *db, QWidget *parent, bool allowHeader ) : KHBox(parent), database(db)
{
	KVBox *ingredientVBox = new KVBox( this );
	KVBox *typeHBox = new KVBox( ingredientVBox );

	if ( allowHeader ) {
		typeButtonGrp = new Q3ButtonGroup();
		QRadioButton *ingredientRadioButton = new QRadioButton( i18n( "Ingredient:" ), typeHBox );
		typeButtonGrp->insert( ingredientRadioButton, 0 );

		//KDE4 i18n => i18nc
		QRadioButton *headerRadioButton = new QRadioButton( i18nc( "Ingredient grouping name", "Header:" ), typeHBox );
		typeButtonGrp->insert( headerRadioButton, 1 );

		typeButtonGrp->setButton( 0 );
		connect( typeButtonGrp, SIGNAL( clicked( int ) ), SLOT( typeButtonClicked( int ) ) );
	}
	else {
		(void) new QLabel( i18n( "Ingredient:" ), typeHBox );
		typeButtonGrp = 0;
	}

	header_ing_stack = new QStackedWidget(ingredientVBox);
	ingredientBox = new IngredientComboBox( true, header_ing_stack, database );
	ingredientBox->setAutoCompletion( true );
	ingredientBox->lineEdit() ->disconnect( ingredientBox ); //so hitting enter doesn't enter the item into the box
	ingredientBox->setSizePolicy( QSizePolicy( QSizePolicy::Ignored, QSizePolicy::Fixed ) );
	header_ing_stack->addWidget( ingredientBox );
	headerBox = new HeaderComboBox( true, header_ing_stack, database );
	headerBox->setAutoCompletion( true );
	headerBox->lineEdit() ->disconnect( ingredientBox ); //so hitting enter doesn't enter the item into the box
	headerBox->setSizePolicy( QSizePolicy( QSizePolicy::Ignored, QSizePolicy::Fixed ) );
	header_ing_stack->addWidget( headerBox );

	KVBox *amountVBox = new KVBox( this );
	amountLabel = new QLabel( i18n( "Amount:" ), amountVBox );
	amountLabel->setAlignment( Qt::AlignBottom );
	amountEdit = new FractionInput( amountVBox );
	amountEdit->setAllowRange(true);
	amountEdit->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed ) );

	KVBox *unitVBox = new KVBox( this );
	unitLabel = new QLabel( i18n( "Unit:" ), unitVBox );
	unitLabel->setAlignment( Qt::AlignBottom );
	unitBox = new KComboBox( true, unitVBox );
	unitBox->setAutoCompletion( true );
	unitBox->lineEdit() ->disconnect( unitBox ); //so hitting enter doesn't enter the item into the box
	unitBox->setSizePolicy( QSizePolicy( QSizePolicy::Ignored, QSizePolicy::Fixed ) );

	KVBox *prepMethodVBox = new KVBox( this );
	prepMethodLabel = new QLabel( i18n( "Preparation Method:" ), prepMethodVBox );
	prepMethodLabel->setAlignment( Qt::AlignBottom );
	prepMethodBox = new PrepMethodComboBox( true, prepMethodVBox, database );
	prepMethodBox->setAutoCompletion( true );
	prepMethodBox->lineEdit() ->disconnect( prepMethodBox ); //so hitting enter doesn't enter the item into the box
	prepMethodBox->setSizePolicy( QSizePolicy( QSizePolicy::Ignored, QSizePolicy::Fixed ) );

	orButton = new QCheckBox( i18n( "OR" ), this );

	setStretchFactor( ingredientVBox, 5 );
	setStretchFactor( amountVBox, 1 );
	setStretchFactor( unitVBox, 2 );
	setStretchFactor( prepMethodVBox, 3 );

	connect( ingredientBox, SIGNAL( activated( int ) ), this, SLOT( loadUnitListCombo() ) );
	connect( ingredientBox->lineEdit(), SIGNAL( lostFocus() ), this, SLOT( slotIngredientBoxLostFocus() ) );
	connect( unitBox->lineEdit(), SIGNAL( lostFocus() ), this, SLOT( slotUnitBoxLostFocus() ) );
	connect( prepMethodBox->lineEdit(), SIGNAL( lostFocus() ), this, SLOT( slotPrepMethodBoxLostFocus() ) );
	connect( orButton, SIGNAL( toggled(bool) ), this, SLOT( orToggled(bool) ) );

	connect( unitBox->lineEdit(), SIGNAL( returnPressed() ), this, SLOT( signalIngredient() ) );
	connect( ingredientBox->lineEdit(), SIGNAL( returnPressed() ), this, SLOT( signalIngredient() ) );
	connect( headerBox->lineEdit(), SIGNAL( returnPressed() ), this, SLOT( signalIngredient() ) );
	connect( prepMethodBox->lineEdit(), SIGNAL( returnPressed() ), this, SLOT( signalIngredient() ) );
	connect( amountEdit, SIGNAL( returnPressed( const QString & ) ), this, SLOT( signalIngredient() ) );

	unitComboList = new UnitList;

	setFocusProxy( ingredientBox );
}

IngredientInput::~IngredientInput()
{
	delete unitComboList;
	delete typeButtonGrp;
}

void IngredientInput::clear()
{
	unitComboList->clear();

	orButton->setChecked(false);
	typeButtonGrp->setButton( 0 ); //put back to ingredient input
	typeButtonClicked( 0 );

	amountEdit->clear();
	ingredientBox->lineEdit()->setText("");
	prepMethodBox->lineEdit()->setText("");
	headerBox->lineEdit()->setText("");
	unitBox->lineEdit()->setText("");
}

void IngredientInput::orToggled(bool b)
{
	emit orToggled(b,this);
}

void IngredientInput::reloadCombos()
{
	//these only needed to be loaded once
	if ( ingredientBox->count() == 0 ) {
		START_TIMER("Loading ingredient input auto-completion");
		ingredientBox->reload();
		END_TIMER();
	}
	if ( headerBox->count() == 0 ) {
		START_TIMER("Loading ingredient header input auto-completion");
		headerBox->reload();
		END_TIMER();
	}
	if ( prepMethodBox->count() == 0 ) {
		START_TIMER("Loading prep method input auto-completion");
		prepMethodBox->reload();
		END_TIMER();
	}

	loadUnitListCombo();
}

void IngredientInput::slotIngredientBoxLostFocus( void )
{
	if ( ingredientBox->contains( ingredientBox->currentText() ) ) {
		ingredientBox->setCurrentItem( ingredientBox->currentText() );
		loadUnitListCombo();
	}
	else {
		unitBox->clear();
		unitBox->completionObject() ->clear();
		unitComboList->clear();
	}
}

void IngredientInput::slotUnitBoxLostFocus()
{
	if ( unitBox->contains( unitBox->currentText() ) )
		unitBox->setCurrentItem( unitBox->currentText() );
}

void IngredientInput::slotPrepMethodBoxLostFocus()
{
	if ( prepMethodBox->contains( prepMethodBox->currentText() ) )
		prepMethodBox->setCurrentItem( prepMethodBox->currentText() );
}

void IngredientInput::typeButtonClicked( int button_id )
{
	if ( amountEdit->isEnabled() == !bool( button_id ) )  //it is already set (the same button was clicked more than once)
		return ;

	amountEdit->setEnabled( !bool( button_id ) );
	unitBox->setEnabled( !bool( button_id ) );
	prepMethodBox->setEnabled( !bool( button_id ) );

	if ( button_id == 1 ) { //Header
		header_ing_stack->setCurrentWidget( headerBox );
	}
	else {
		header_ing_stack->setCurrentWidget( ingredientBox );
	}
}

void IngredientInput::enableHeader( bool enable )
{
	if ( !enable ) {
		typeButtonGrp->setButton( 0 ); //put back to ingredient input
		typeButtonClicked( 0 );
	}
	typeButtonGrp->find(1)->setEnabled(enable);
}

void IngredientInput::signalIngredient()
{
	//validate input; if successful, emit signal
	if ( isHeader() ) {
		if ( header().isEmpty() )
			return;
	}
	else {
		if ( !isInputValid() )
			return;
	}

	emit addIngredient();
}

bool IngredientInput::isInputValid()
{
	if ( ingredientBox->currentText().trimmed().isEmpty() ) {
		KMessageBox::error( this, i18n( "Please enter an ingredient" ), QString() );
		ingredientBox->setFocus();
		return false;
	}
	return checkAmountEdit() && checkBounds();
}

bool IngredientInput::checkBounds()
{
	if ( ingredientBox->currentText().length() > int(database->maxIngredientNameLength()) ) {
		KMessageBox::error( this, i18np( "Ingredient name cannot be longer than 1 character.", "Ingredient name cannot be longer than %1 characters." , database->maxIngredientNameLength() ) );
		ingredientBox->lineEdit() ->setFocus();
		ingredientBox->lineEdit() ->selectAll();
		return false;
	}

	if ( unitBox->currentText().length() > int(database->maxUnitNameLength()) ) {
		KMessageBox::error( this, i18np( "Unit name cannot be longer than 1 character.", "Unit name cannot be longer than %1 characters.", database->maxUnitNameLength() ) );
		unitBox->lineEdit() ->setFocus();
		unitBox->lineEdit() ->selectAll();
		return false;
	}

	QStringList prepMethodList;
	if (prepMethodBox->currentText().isEmpty())
		prepMethodList = QStringList();
	else
		prepMethodList = prepMethodBox->currentText().split( ',', QString::SkipEmptyParts);
   
	for ( QStringList::const_iterator it = prepMethodList.constBegin(); it != prepMethodList.constEnd(); ++it ) {
		if ( (*it).trimmed().length() > int(database->maxPrepMethodNameLength()) )
		{
			KMessageBox::error( this, i18np( "Preparation method cannot be longer than 1 character.", "Preparation method cannot be longer than %1 characters." , database->maxPrepMethodNameLength() ) );
			prepMethodBox->lineEdit() ->setFocus();
			prepMethodBox->lineEdit() ->selectAll();
			return false;
		}
	}

	return true;
}

bool IngredientInput::checkAmountEdit()
{
	if ( amountEdit->isInputValid() )
		return true;
	else {
		KMessageBox::error( this, i18n( "Amount field contains invalid input." ),
			i18n( "Invalid input" ) );
		amountEdit->setFocus();
		amountEdit->selectAll();
		return false;
	}
}

void IngredientInput::loadUnitListCombo()
{
	QString store_unit = unitBox->currentText();
	unitBox->clear(); // Empty the combo first
	unitBox->completionObject() ->clear();

	int comboIndex = ingredientBox->currentIndex();
	int comboCount = ingredientBox->count();

	if ( comboCount > 0 ) { // If not, the list may be empty (no ingredient list defined) and crashes while reading
		int selectedIngredient = ingredientBox->id( comboIndex );
		database->loadPossibleUnits( selectedIngredient, unitComboList );

		//Populate this data into the ComboBox
		for ( UnitList::const_iterator unit_it = unitComboList->constBegin(); unit_it != unitComboList->constEnd(); ++unit_it ) {
			unitBox->insertItem( unitBox->count(), ( *unit_it ).name() );
			unitBox->completionObject() ->addItem( ( *unit_it ).name() );
			if ( ( *unit_it ).name() != (*unit_it ).plural() ) {
				unitBox->insertItem( unitBox->count(), ( *unit_it ).plural() );
				unitBox->completionObject() ->addItem( ( *unit_it ).plural() );
			}

			if ( !( *unit_it ).nameAbbrev().isEmpty() ) {
				unitBox->insertItem( unitBox->count(), ( *unit_it ).nameAbbrev() );
				unitBox->completionObject() ->addItem( ( *unit_it ).nameAbbrev() );
			}
			if ( !(*unit_it ).pluralAbbrev().isEmpty() &&
				( *unit_it ).nameAbbrev() != (*unit_it ).pluralAbbrev() ) {
				unitBox->insertItem( unitBox->count(), ( *unit_it ).pluralAbbrev() );
				unitBox->completionObject() ->addItem( ( *unit_it ).pluralAbbrev() );
			}

		}
	}
	unitBox->lineEdit() ->setText( store_unit );
}

bool IngredientInput::isHeader() const
{
	return typeButtonGrp && (typeButtonGrp->id( typeButtonGrp->selected() ) == 1);
}

Ingredient IngredientInput::ingredient() const
{
	Ingredient ing;

	ing.prepMethodList = ElementList::split(",",prepMethodBox->currentText());
	ing.name = ingredientBox->currentText();
	amountEdit->value(ing.amount,ing.amount_offset);
	ing.units = Unit(unitBox->currentText().trimmed(),ing.amount+ing.amount_offset);
	ing.ingredientID = ingredientBox->id( ingredientBox->currentIndex() );

	return ing;
}

QString IngredientInput::header() const
{
	return headerBox->currentText().trimmed();
}

void IngredientInput::updateTabOrder()
{
	QWidget::setTabOrder( ingredientBox, amountEdit );
	QWidget::setTabOrder( amountEdit, unitBox );
	QWidget::setTabOrder( unitBox, prepMethodBox );
	QWidget::setTabOrder( prepMethodBox, orButton );
}


IngredientInputWidget::IngredientInputWidget( RecipeDB *db, QWidget *parent ) : KVBox(parent), database(db)
{
	setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );

	m_ingInputs.append(new IngredientInput(database,this));

	// Connect signals & Slots
	connect( m_ingInputs[0], SIGNAL(addIngredient()), this, SLOT(addIngredient()) );
	connect( m_ingInputs[0], SIGNAL(orToggled(bool,IngredientInput*)), this, SLOT(updateInputs(bool,IngredientInput*)) );

	reloadCombos();
}

IngredientInputWidget::~IngredientInputWidget()
{
}

void IngredientInputWidget::clear()
{
	//clearing the first input deletes all substitute inputs
	m_ingInputs[0]->clear();
}

void IngredientInputWidget::updateInputs(bool on, IngredientInput* input)
{
	QList<IngredientInput*>::iterator curr;
	if (m_ingInputs.indexOf(input) == -1)
		curr = m_ingInputs.end();
	else
		curr = m_ingInputs.begin()+m_ingInputs.indexOf(input);

	IngredientInput *prev_input = *curr;
	++curr;

	if ( on ) {
		IngredientInput *new_input = new IngredientInput(database,this,false);
		new_input->reloadCombos();

		QWidget::setTabOrder( prev_input, new_input );
		new_input->updateTabOrder();

		connect( new_input, SIGNAL(addIngredient()), this, SLOT(addIngredient()) );
		connect( new_input, SIGNAL(orToggled(bool,IngredientInput*)), this, SLOT(updateInputs(bool,IngredientInput*)) );

		new_input->show();
		m_ingInputs.insert(curr,new_input);

		m_ingInputs[0]->enableHeader(false);

	}
	else {
		while ( curr != m_ingInputs.end() ) {
			(*curr)->deleteLater();
			curr = m_ingInputs.erase(curr);
		}
		if ( m_ingInputs.count() == 1 )
			m_ingInputs[0]->enableHeader(true);
	}
}

void IngredientInputWidget::addIngredient()
{
	if ( m_ingInputs[0]->isHeader() ) {
		QString header = m_ingInputs[0]->header();
		if ( header.isEmpty() )
			return;

		int group_id = createNewGroupIfNecessary( header,database );
		emit headerEntered( Element(header,group_id) );
	}
	else {
		for ( QList<IngredientInput*>::iterator it = m_ingInputs.begin(); it != m_ingInputs.end(); ++it ) {
			if ( !(*it)->isInputValid() )
				return;
		}

		QList<IngredientData> list;
		for ( QList<IngredientInput*>::const_iterator it = m_ingInputs.constBegin(); it != m_ingInputs.constEnd(); ++it ) {
			Ingredient ing = (*it)->ingredient();
			ing.ingredientID = createNewIngredientIfNecessary(ing.name,database);

			bool plural = ing.amount+ing.amount_offset > 1;
			ing.units.setId(createNewUnitIfNecessary( ing.units.determineName(ing.amount+ing.amount_offset, false), plural, ing.ingredientID, ing.units,database ));
			if ( ing.units.id() == -1 )  // this will happen if the dialog to create a unit was cancelled
				return ;

			QList<int> prepIDs = createNewPrepIfNecessary( ing.prepMethodList,database );
			QList<int>::const_iterator id_it = prepIDs.constBegin();
			for ( ElementList::iterator it = ing.prepMethodList.begin(); it != ing.prepMethodList.end(); ++it, ++id_it ) {
				(*it).id = *id_it;
			}

			list.append(ing);
		}

		Ingredient ing = list.first();
		list.pop_front();
		ing.substitutes = list;
		emit ingredientEntered( ing );
	}
	clear();

	m_ingInputs[0]->setFocus(); //put cursor back to the ingredient name so user can begin next ingredient
}

int IngredientInputWidget::createNewIngredientIfNecessary( const QString &ing, RecipeDB *database )
{
	int id = -1;
	if ( ing.isEmpty() )
		return -1;

	id = database->findExistingIngredientByName( ing );
	if (  id == -1 ) {
		database->createNewIngredient( ing );
		id = database->lastInsertID();
	}
	return id;
}

int IngredientInputWidget::createNewUnitIfNecessary( const QString &unit, bool plural, int ingredientID, Unit &new_unit, RecipeDB *database )
{
	int id = database->findExistingUnitByName( unit );
	if ( -1 == id )
	{
		QPointer<CreateUnitDialog> getUnit =
			new CreateUnitDialog( 0, ( plural ) ? QString() : unit, ( !plural ) ? QString() : unit );
		if ( getUnit->exec() == QDialog::Accepted ) {
			new_unit = getUnit->newUnit();
			database->createNewUnit( new_unit );

			id = database->lastInsertID();
		}
		delete getUnit;
	}

	if ( !database->ingredientContainsUnit(
			ingredientID,
			id ) )
	{
		database->addUnitToIngredient(
			ingredientID,
			id );
	}

	new_unit = database->unitName( id );

	//loadUnitListCombo();
	return id;
}

QList<int> IngredientInputWidget::createNewPrepIfNecessary( const ElementList &prepMethods, RecipeDB *database )
{
	QList<int> ids;

	if ( prepMethods.isEmpty() )  //no prep methods
		return ids;
	else
	{
		for ( ElementList::const_iterator it = prepMethods.begin(); it != prepMethods.end(); ++it ) {
			int id = database->findExistingPrepByName( (*it).name.trimmed() );
			if ( id == -1 )
			{
				database->createNewPrepMethod( (*it).name.trimmed() );
				id = database->lastInsertID();
			}
			ids << id;
		}

		return ids;
	}
}

int IngredientInputWidget::createNewGroupIfNecessary( const QString &group, RecipeDB *database )
{
	if ( group.trimmed().isEmpty() )  //no group
		return -1;
	else
	{
		int id = database->findExistingIngredientGroupByName( group );
		if ( id == -1 ) //creating new
		{
			database->createNewIngGroup( group );
			id = database->lastInsertID();
		}

		return id;
	}
}

void IngredientInputWidget::reloadCombos()
{
	for ( QList<IngredientInput*>::iterator it = m_ingInputs.begin(); it != m_ingInputs.end(); ++it )
		(*it)->reloadCombos();
}

#include "ingredientinputwidget.moc"
