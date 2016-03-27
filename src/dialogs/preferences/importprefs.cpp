/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                         *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>               *
*   Copyright © 2003, 2004, 2006 Jason Kivlighn <jkivlighn@gmail.com>      *
*   Copyright © 2009-2016 José Manuel Santamaría Lema <panfaust@gmail.com> *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#include "importprefs.h"

#include <KConfigGroup>
#include <KGlobal>
#include <KLocale>
#include <KComboBox>

#include <QVBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QLabel>


ImportPrefs::ImportPrefs( QWidget *parent )
		: QWidget( parent )
{
	// Load Current Settings
	KConfigGroup config = KGlobal::config()->group( "Import" );

	bool overwrite = config.readEntry( "OverwriteExisting", false );
	bool direct = config.readEntry( "DirectImport", false );

	Form1Layout = new QVBoxLayout( this );
	Form1Layout->setMargin( 11 );
	Form1Layout->setSpacing( 6 );

	QGroupBox *importGroup = new QGroupBox;
	QVBoxLayout *importGroupLayout = new QVBoxLayout;
	importGroup->setLayout( importGroupLayout );
	importGroup->setTitle( i18n( "Import" ) );

	overwriteCheckbox = new QCheckBox( i18n( "Overwrite recipes with same title" ), importGroup );
	overwriteCheckbox->setChecked( overwrite );
	overwriteCheckbox->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
	importGroupLayout->addWidget( overwriteCheckbox );

	directImportCheckbox = new QCheckBox( i18n( "Ask which recipes to import" ), importGroup );
	directImportCheckbox->setChecked( !direct );
	directImportCheckbox->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
	importGroupLayout->addWidget( directImportCheckbox );

	Form1Layout->addWidget(importGroup);

	QGroupBox *exportGroup = new QGroupBox;
	QGridLayout *exportGroupLayout = new QGridLayout;
	exportGroup->setLayout( exportGroupLayout );
	exportGroup->setTitle( i18n( "Export" ) );

	QLabel *clipboardLabel = new QLabel(i18n("'Copy to Clipboard' format:") );
	exportGroupLayout->addWidget( clipboardLabel, 0, 0 );
	clipBoardFormatComboBox = new KComboBox;
	clipBoardFormatComboBox->insertItem( clipBoardFormatComboBox->count(), QString("%3 (*.txt)").arg(i18n("Plain Text")));
	clipBoardFormatComboBox->insertItem( clipBoardFormatComboBox->count(), "Krecipes (*.kreml)");
	clipBoardFormatComboBox->insertItem( clipBoardFormatComboBox->count(), "Meal-Master (*.mmf)");
	clipBoardFormatComboBox->insertItem( clipBoardFormatComboBox->count(), "Rezkonv (*.rk)");
	clipBoardFormatComboBox->insertItem( clipBoardFormatComboBox->count(), "RecipeML (*.xml)");
	clipBoardFormatComboBox->insertItem( clipBoardFormatComboBox->count(), "MasterCook (*.mx2)");
	//clipBoardFormatComboBox->insertItem("CookML (*.cml)");
	exportGroupLayout->addWidget( clipBoardFormatComboBox, 0, 1 ); 

	config = KGlobal::config()->group( "Export" );
	QString clipboardFormat = config.readEntry("ClipboardFormat");
	if ( clipboardFormat == "*.kreml" )
		clipBoardFormatComboBox->setCurrentIndex(1);
	else if ( clipboardFormat == "*.mmf" )
		clipBoardFormatComboBox->setCurrentIndex(2);
	else if ( clipboardFormat == "*.xml" )
		clipBoardFormatComboBox->setCurrentIndex(3);
	else if ( clipboardFormat == "*.mx2" )
		clipBoardFormatComboBox->setCurrentIndex(4);
	else
		clipBoardFormatComboBox->setCurrentIndex(0);

	Form1Layout->addWidget(exportGroup);

	Form1Layout->addItem( new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding ) );

	directImportCheckbox->setWhatsThis(
		i18n("When this is enabled, the importer will show every recipe in the file(s) and allow you to select which recipes you want imported.\n \
							\
		Disable this to always import every recipe, which allows for faster and less memory-intensive imports.")
	);

	adjustSize();
}

void ImportPrefs::saveOptions()
{
	KConfigGroup config = KGlobal::config()->group( "Import" );

	config.writeEntry( "OverwriteExisting", overwriteCheckbox->isChecked() );
	config.writeEntry( "DirectImport", !directImportCheckbox->isChecked() );

	config = KGlobal::config()->group( "Export" );
	QString ext = clipBoardFormatComboBox->currentText().mid(clipBoardFormatComboBox->currentText().indexOf("(")+1,clipBoardFormatComboBox->currentText().length()-clipBoardFormatComboBox->currentText().indexOf("(")-2);
	config.writeEntry( "ClipboardFormat", ext );
}

