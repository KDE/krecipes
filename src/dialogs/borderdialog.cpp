/***************************************************************************
*   Copyright (C) 2004 by                                                 *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "borderdialog.h"

#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

#include <kdebug.h>
#include <khtml_part.h>
#include <khtmlview.h>
#include <klistbox.h>
#include <klocale.h>

#include "datablocks/kreborder.h"

BorderDialog::BorderDialog( const KreBorder &border, QWidget* parent, const char* name )
		: QDialog( parent, name, true )
{
	BorderDialogLayout = new QVBoxLayout( this, 11, 6, "BorderDialogLayout" );

	borderGroupBox = new QGroupBox( this, "borderGroupBox" );
	borderGroupBox->setColumnLayout( 0, Qt::Vertical );
	borderGroupBox->layout() ->setSpacing( 6 );
	borderGroupBox->layout() ->setMargin( 11 );
	borderGroupBoxLayout = new QVBoxLayout( borderGroupBox->layout() );
	borderGroupBoxLayout->setAlignment( Qt::AlignTop );

	layout4 = new QHBoxLayout( 0, 0, 6, "layout4" );

	layout3 = new QVBoxLayout( 0, 0, 6, "layout3" );

	styleLabel = new QLabel( borderGroupBox, "styleLabel" );
	layout3->addWidget( styleLabel );

	styleListBox = new KListBox( borderGroupBox, "styleListBox" );
	layout3->addWidget( styleListBox );
	layout4->addLayout( layout3 );

	layout2 = new QVBoxLayout( 0, 0, 6, "layout2" );

	colorLabel = new QLabel( borderGroupBox, "colorLabel" );
	layout2->addWidget( colorLabel );

	QHBox *color_hbox = new QHBox( borderGroupBox );
	color_hbox->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
	hsSelector = new KHSSelector( color_hbox );
	hsSelector->setMinimumSize( 140, 70 );
	connect( hsSelector, SIGNAL( valueChanged( int, int ) ), SLOT( slotHSChanged( int, int ) ) );

	valuePal = new KValueSelector( color_hbox );
	valuePal->setMinimumSize( 26, 70 );
	connect( valuePal, SIGNAL( valueChanged( int ) ), SLOT( slotVChanged( int ) ) );

	layout2->addWidget( color_hbox );
	layout4->addLayout( layout2 );

	layout1 = new QVBoxLayout( 0, 0, 6, "layout1" );

	widthLabel = new QLabel( borderGroupBox, "widthLabel" );
	layout1->addWidget( widthLabel );

	widthSpinBox = new QSpinBox( borderGroupBox, "widthSpinBox" );
	widthSpinBox->setMinValue( 1 );
	layout1->addWidget( widthSpinBox );

	widthListBox = new KListBox( borderGroupBox, "widthListBox" );
	layout1->addWidget( widthListBox );
	layout4->addLayout( layout1 );
	borderGroupBoxLayout->addLayout( layout4 );

	borderPreview = new KHTMLPart( borderGroupBox );
	borderPreview->view() ->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );
	borderGroupBoxLayout->addWidget( borderPreview->view() );
	BorderDialogLayout->addWidget( borderGroupBox );

	QHBoxLayout *bottom_layout = new QHBoxLayout( 0, 0, 6 );
	QSpacerItem* spacer = new QSpacerItem( 20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum );
	bottom_layout->addItem( spacer );
	QPushButton *ok_button = new QPushButton( i18n( "&OK" ), this );
	bottom_layout->addWidget( ok_button );
	QPushButton *cancel_button = new QPushButton( i18n( "&Cancel" ), this );
	bottom_layout->addWidget( cancel_button );

	BorderDialogLayout->addLayout( bottom_layout );

	languageChange();

	connect( widthSpinBox, SIGNAL( valueChanged( int ) ), SLOT( updatePreview() ) );
	connect( widthListBox, SIGNAL( highlighted( int ) ), SLOT( updateSpinBox( int ) ) );
	connect( styleListBox, SIGNAL( highlighted( int ) ), SLOT( updatePreview() ) );

	connect( ok_button, SIGNAL( clicked() ), SLOT( accept() ) );
	connect( cancel_button, SIGNAL( clicked() ), SLOT( reject() ) );

	initListBoxs();
	loadBorder( border );

	clearWState( WState_Polished );
}

BorderDialog::~BorderDialog()
{}

void BorderDialog::languageChange()
{
	borderGroupBox->setTitle( i18n( "Requested Border" ) );
	styleLabel->setText( i18n( "Style:" ) );
	colorLabel->setText( i18n( "Color:" ) );
	widthLabel->setText( i18n( "Width:" ) );
}

KreBorder BorderDialog::border() const
{
	int width = widthSpinBox->value();

	QString style;
	switch ( styleListBox->currentItem() ) {
	case 0:
		style = "none";
		break;
	case 1:
		style = "dotted";
		break;
	case 2:
		style = "dashed";
		break;
	case 3:
		style = "solid";
		break;
	case 4:
		style = "double";
		break;
	case 5:
		style = "groove";
		break;
	case 6:
		style = "ridge";
		break;
	case 7:
		style = "inset";
		break;
	case 8:
		style = "outset";
		break;
	}

	return KreBorder( width, style, selColor );
}

void BorderDialog::loadBorder( const KreBorder &border )
{
	widthSpinBox->setValue( border.width );
	widthListBox->setCurrentItem( border.width - 1 );

	if ( border.style == "none" )
		styleListBox->setCurrentItem( 0 );
	else if ( border.style == "dotted" )
		styleListBox->setCurrentItem( 1 );
	else if ( border.style == "dashed" )
		styleListBox->setCurrentItem( 2 );
	else if ( border.style == "solid" )
		styleListBox->setCurrentItem( 3 );
	else if ( border.style == "double" )
		styleListBox->setCurrentItem( 4 );
	else if ( border.style == "groove" )
		styleListBox->setCurrentItem( 5 );
	else if ( border.style == "ridge" )
		styleListBox->setCurrentItem( 6 );
	else if ( border.style == "inset" )
		styleListBox->setCurrentItem( 7 );
	else if ( border.style == "outset" )
		styleListBox->setCurrentItem( 8 );

	setColor( border.color );

	updatePreview();
}

void BorderDialog::initListBoxs()
{
	styleListBox->insertItem( i18n( "None" ) );
	styleListBox->insertItem( i18n( "See http://krecipes.sourceforge.net/bordertypes.png for an example", "Dotted" ) );
	styleListBox->insertItem( i18n( "See http://krecipes.sourceforge.net/bordertypes.png for an example", "Dashed" ) );
	styleListBox->insertItem( i18n( "See http://krecipes.sourceforge.net/bordertypes.png for an example", "Solid" ) );
	styleListBox->insertItem( i18n( "See http://krecipes.sourceforge.net/bordertypes.png for an example", "Double" ) );
	styleListBox->insertItem( i18n( "See http://krecipes.sourceforge.net/bordertypes.png for an example", "Groove" ) );
	styleListBox->insertItem( i18n( "See http://krecipes.sourceforge.net/bordertypes.png for an example", "Ridge" ) );
	styleListBox->insertItem( i18n( "See http://krecipes.sourceforge.net/bordertypes.png for an example", "Inset" ) );
	styleListBox->insertItem( i18n( "See http://krecipes.sourceforge.net/bordertypes.png for an example", "Outset" ) );

	widthListBox->insertItem( "1" );
	widthListBox->insertItem( "2" );
	widthListBox->insertItem( "3" );
	widthListBox->insertItem( "4" );
	widthListBox->insertItem( "5" );
	widthListBox->insertItem( "6" );
	widthListBox->insertItem( "7" );
}

void BorderDialog::updatePreview()
{
	KreBorder b( border() );

	QString html_str = QString( "<html><body><div style=\"vertical-align: middle; border: %1px %2 %3;\"><center><h1>%4</h1></center></div></body></html>" ).arg( b.width ).arg( b.style ).arg( b.color.name() ).arg( i18n( "Border Preview" ) );

	borderPreview->begin();
	borderPreview->write( html_str );
	borderPreview->end();
	borderPreview->show();
}

void BorderDialog::updateSpinBox( int index )
{
	widthSpinBox->setValue( index + 1 );
}

void BorderDialog::slotHSChanged( int h, int s )
{
	int _h, _s, v;
	selColor.hsv( &_h, &_s, &v );
	if ( v < 1 )
		v = 1;

	KColor col;
	col.setHsv( h, s, v );

	setColor( col );
	updatePreview();
}

void BorderDialog::slotVChanged( int v )
{
	int h, s, _v;
	selColor.hsv( &h, &s, &_v );

	KColor col;
	col.setHsv( h, s, v );

	setColor( col );
	updatePreview();
}

void BorderDialog::setColor( const KColor &color )
{
	if ( color == selColor )
		return ;

	selColor = color;

	int h, s, v;
	color.hsv( &h, &s, &v );
	hsSelector->setValues( h, s );
	valuePal->setHue( h );
	valuePal->setSaturation( s );
	valuePal->setValue( v );
	valuePal->updateContents();
	valuePal->repaint( FALSE );
}

#include "borderdialog.moc"
