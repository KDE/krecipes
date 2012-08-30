/****************************************************************************
** Form implementation generated from reading ui file 'createunitconversiondialog.ui'
**
** Created: Thu Sep 13 19:56:07 2007
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.8   edited Jan 11 14:47 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "createunitconversiondialog.h"

#include <q3buttongroup.h>
#include <QLabel>
#include <kvbox.h>
#include <QHBoxLayout>
#include <QComboBox>

#include <klocale.h>

CreateUnitConversionDialog::CreateUnitConversionDialog( const Element &fromUnit, const ElementList &toUnits, QWidget* parent )
	: KDialog( parent ), m_toUnits(toUnits)
{
	setButtons(KDialog::Ok | KDialog::Cancel );
	setDefaultButton(KDialog::Ok);
	setModal( true );
	KVBox *page = new KVBox( this );
	setMainWidget( page );

	buttonGroup1 = new Q3ButtonGroup( page, "buttonGroup1" );
	buttonGroup1->setColumnLayout(0, Qt::Vertical );
	buttonGroup1->layout()->setSpacing( 6 );
	buttonGroup1->layout()->setMargin( 11 );
	buttonGroup1Layout = new QHBoxLayout( buttonGroup1->layout() );
	buttonGroup1Layout->setAlignment( Qt::AlignTop );

	fromUnitEdit = new FractionInput( buttonGroup1 );
	buttonGroup1Layout->addWidget( fromUnitEdit );

	fromUnitLabel = new QLabel( buttonGroup1 );
	fromUnitLabel->setObjectName( "fromUnitLabel" );
	buttonGroup1Layout->addWidget( fromUnitLabel );

	textLabel4 = new QLabel( buttonGroup1 );
	textLabel4->setObjectName( "textLabel4" );
	buttonGroup1Layout->addWidget( textLabel4 );

	toUnitEdit = new FractionInput( buttonGroup1 );
	buttonGroup1Layout->addWidget( toUnitEdit );

	toUnitComboBox = new QComboBox( buttonGroup1 );
	toUnitComboBox->setEditable( false );
	toUnitComboBox->setObjectName( "toUnitComboBox" );
	buttonGroup1Layout->addWidget( toUnitComboBox );

	languageChange();

	//clearWState( WState_Polished );

	for ( ElementList::const_iterator it = m_toUnits.constBegin(); it != m_toUnits.constEnd(); ++it ) {
		toUnitComboBox->insertItem( toUnitComboBox->count(), (*it).name );
	}

	fromUnitLabel->setText(fromUnit.name);
	fromUnitEdit->setValue(1);
}

CreateUnitConversionDialog::~CreateUnitConversionDialog()
{
	// no need to delete child widgets, Qt does it all for us
}

void CreateUnitConversionDialog::languageChange()
{
	setCaption( i18nc( "@title:window", "Unit Conversion" ) );
	buttonGroup1->setTitle( i18nc( "@title:group", "New Unit Conversion" ) );
	fromUnitLabel->setText( QString() );
	textLabel4->setText( "=" );
}

int CreateUnitConversionDialog::toUnitID() const
{
	return m_toUnits.findByName(toUnitComboBox->currentText()).id;
}

double CreateUnitConversionDialog::ratio() const
{
	if (toUnitEdit->isInputValid() && fromUnitEdit->isInputValid() &&
		fromUnitEdit->value() > 0 ) {
		return toUnitEdit->value().toDouble() / fromUnitEdit->value().toDouble();
	}
	return -1;
}
