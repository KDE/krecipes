/***************************************************************************
 *   Copyright (C) 2003 by Jason Kivlighn                                  *
 *   mizunoami44@users.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "pagesetupdialog.h"

#include "../widgets/dragarea.h"
#include "../widgets/sizehandle.h"

#include <kapplication.h>
#include <kconfig.h>
#include <klocale.h>
#include <kiconloader.h>

#include <qhbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtextedit.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qvaluelist.h>

#include <cmath>

PageSetupDialog::PageSetupDialog( QWidget *parent, const Recipe &sample ) : QDialog(parent,0,true)
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	dragArea = new DragArea(this);
	dragArea->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
	layout->addWidget(dragArea);

	QHBox *buttonsBox = new QHBox(this);
	KIconLoader *il = new KIconLoader;
	QPushButton *okButton = new QPushButton(il->loadIconSet("ok",KIcon::Small),i18n("&OK"),buttonsBox);
	QPushButton *cancelButton = new QPushButton(il->loadIconSet("cancel",KIcon::Small),i18n("&Cancel"),buttonsBox);
	layout->addWidget(buttonsBox);

	connect( okButton, SIGNAL(clicked()), SLOT(accept()) );
	connect( cancelButton, SIGNAL(clicked()), SLOT(reject()) );

	createWidgets( sample );

	adjustSize(); //this seems to need to be called to set a fixed size...
	setFixedSize(500,600);

	loadSetup();
}

//do not call until createWidgets() has been called!
void PageSetupDialog::loadSetup()
{
	//TODO: we have just one setup available for now. Later setup may be a separate file
	KConfig *config=kapp->config();
	config->setGroup("PageSetup");

	//=========================TITLE=======================//
	QValueList<int> title_dim_info = config->readIntListEntry( "TitleDimensions" );
	if ( title_dim_info.count() < 3 )
	{
		title_dim_info.append( 5 );   //left
		title_dim_info.append( 3 );   //top
		title_dim_info.append( 488 ); //width
	}
	title_box->setGeometry( title_dim_info[0], title_dim_info[1], title_dim_info[2], title_box->height() );

	//======================INSTRUCTIONS===================//
	QValueList<int> instr_dim_info = config->readIntListEntry( "InstructionsDimensions" );
	if ( instr_dim_info.count() < 3 )
	{
		instr_dim_info.append( 5 );   //left
		instr_dim_info.append( 287 ); //top
		instr_dim_info.append( 488 ); //width
	}
	instr_box->setGeometry( instr_dim_info[0], instr_dim_info[1], instr_dim_info[2], instr_box->height() );

	//=======================SERVINGS======================//
	QValueList<int> servings_dim_info = config->readIntListEntry( "ServingsDimensions" );
	if ( servings_dim_info.count() < 3 )
	{
		servings_dim_info.append( 400 );   //left
		servings_dim_info.append( 250 ); //top
		servings_dim_info.append( 85 ); //width
	}
	servings_box->setGeometry( servings_dim_info[0], servings_dim_info[1], servings_dim_info[2], servings_box->height() );

	//========================PHOTO========================//
	QValueList<int> photo_dim_info = config->readIntListEntry( "PhotosDimensions" );
	if ( photo_dim_info.count() < 4 )
	{
		photo_dim_info.append( 150 );  //left
		photo_dim_info.append( 45 );  //top
		photo_dim_info.append( 200 );  //width
		photo_dim_info.append( 200 );  //height
	}
	photo_box->setGeometry( photo_dim_info[0], photo_dim_info[1], photo_dim_info[2], photo_dim_info[3] );
}

void PageSetupDialog::createWidgets( const Recipe &sample )
{
	//=========================TITLE=======================//
	QString title;
	if ( sample.title.isNull() )
		title = i18n("Recipe Title");
	else
		title = sample.title;

	title_box = new QLabel("<center><h2>"+title+"</h2></center>",dragArea,"title_box");
	title_box->setFrameShape( QFrame::Box );
	title_box->setTextFormat( Qt::RichText ); //allows for wrapping
	QToolTip::add(title_box,i18n("Title"));

	//======================INSTRUCTIONS===================//
	QString instr;
	if ( sample.instructions.isNull() )
		instr = i18n("Instructions");
	else
		instr = sample.instructions;

	instr_box = new QLabel(instr,dragArea,"instr_box");
	instr_box->setFrameShape( QFrame::Box );
	instr_box->setAlignment( Qt::AlignTop );
	instr_box->setTextFormat( Qt::RichText ); //allows for wrapping
	QToolTip::add(instr_box,i18n("instructions"));

	//=======================SERVINGS======================//
	servings_box = new QLabel(QString(i18n("Servings: %1")).arg(sample.persons),dragArea,"servings_box");
	servings_box->setFrameShape( QFrame::Box );
	QToolTip::add(servings_box,i18n("Servings"));

	//========================PHOTO========================//
	photo_box = new QLabel(dragArea,"photo_box");
	photo_box->setFrameShape( QFrame::Box );
	photo_box->setScaledContents( true );
	photo_box->setPixmap(sample.photo);
	QToolTip::add(photo_box,i18n("Photo"));
}

void PageSetupDialog::accept()
{
	save();

	QDialog::accept();
}

void PageSetupDialog::save()
{
	//let's spit out some of the geometry info
	qDebug("%s: left: %d, top: %d, width: %d, height: %d",title_box->name(),title_box->geometry().left(),title_box->geometry().top(),title_box->width(),title_box->height());
	qDebug("%s: left: %d, top: %d, width: %d, height: %d",instr_box->name(),instr_box->geometry().left(),instr_box->geometry().top(),instr_box->width(),instr_box->height());
	qDebug("%s: left: %d, top: %d, width: %d, height: %d",servings_box->name(),servings_box->geometry().left(),servings_box->geometry().top(),servings_box->width(),servings_box->height());
	qDebug("%s: left: %d, top: %d, width: %d, height: %d",photo_box->name(),photo_box->geometry().left(),photo_box->geometry().top(),photo_box->width(),photo_box->height());


}
