/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "usdadatadialog.h"

#include <kdebug.h>
#include <kstandarddirs.h>
#include <klineedit.h>
#include <klistview.h>
#include <klocale.h>
#include <kmessagebox.h>

#include <qfile.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qtextstream.h>
#include <qvbox.h>

#include "DBBackend/recipedb.h"

struct property_data
{
	int id;
	const char *name;
	const char *unit;
};

//NOTE: the following must be in this order
static property_data property_data_list[] = {
  {-1,I18N_NOOP("water"),"g"},
  {-1,I18N_NOOP("energy"),"kcal"},
  {-1,I18N_NOOP("protein"),"g"},
  {-1,I18N_NOOP("fat"),"g"},
  {-1,I18N_NOOP("ash"),"g"},
  {-1,I18N_NOOP("carbohydrates"),"g"},
  {-1,I18N_NOOP("dietary fiber"),"g"},
  {-1,I18N_NOOP("sugar"),"g"},
  {-1,I18N_NOOP("calcuim"),"mg"},
  {-1,I18N_NOOP("iron"),"mg"},
  {-1,I18N_NOOP("magnesium"),"mg"},
  {-1,I18N_NOOP("phosphorus"),"mg"},
  {-1,I18N_NOOP("potassium"),"mg"},
  {-1,I18N_NOOP("sodium"),"mg"},
  {-1,I18N_NOOP("zinc"),"mg"},
  {-1,I18N_NOOP("copper"),"mg"},
  {-1,I18N_NOOP("manganese"),"mg"},
  {-1,I18N_NOOP("selenium"),"g"},
  {-1,I18N_NOOP("vitamin C"),"mg"},
  {-1,I18N_NOOP("thiamin"),"mg"},
  {-1,I18N_NOOP("riboflavin"),"mg"},
  {-1,I18N_NOOP("niacin"),"mg"},
  {-1,I18N_NOOP("pantothenic acid"),"mg"},
  {-1,I18N_NOOP("vitamin B"),"mg"},
  {-1,I18N_NOOP("folate"),"g"},
  {-1,I18N_NOOP("folic acid"),"g"},
  {-1,I18N_NOOP("food folate"),"g"},
  {-1,I18N_NOOP("folate (DFE)"),"g"},
  {-1,I18N_NOOP("vitamin B12"),"g"},
  {-1,I18N_NOOP("vitamin A (IU)"),"g"},
  {-1,I18N_NOOP("vitamin A"),"mg"},
  {-1,I18N_NOOP("retinol"),"g"},
  {-1,I18N_NOOP("vitamin E"),"g"},
  {-1,I18N_NOOP("vitamin K"),"g"},
  {-1,I18N_NOOP("alpha-carotene"),"g"},
  {-1,I18N_NOOP("beta-carotene"),"g"},
  {-1,I18N_NOOP("beta-cryptoxanthin"),"g"},
  {-1,I18N_NOOP("lycopene"),"g"},
  {-1,I18N_NOOP("lutein+zeazanthin"),"g"},
  {-1,I18N_NOOP("saturated fat"),"g"},
  {-1,I18N_NOOP("monounsaturated fat"),"g"},
  {-1,I18N_NOOP("polyunsaturated fat"),"g"},
  {-1,I18N_NOOP("cholesterol"),"mg"},
  { 0, 0, 0 }
};

USDADataDialog::USDADataDialog( const Element &ing, RecipeDB *db, QWidget *parent ) : KDialog(parent,0,true),
  ingredient(ing),
  database(db)
{
	setCaption(QString(i18n("Load ingredient properties for: \"%1\"")).arg(ingredient.name));

	QHBoxLayout *hbox_layout = new QHBoxLayout( this, 11, 6 );

	QVBox *left_box = new QVBox(this);

	QHBox *search_hbox = new QHBox(left_box);
	QLabel *search_label = new QLabel( i18n("Search: "), search_hbox );
	search_edit = new KLineEdit(search_hbox);

	listView = new KListView(left_box);
	listView->addColumn(i18n("USDA Ingredient"));
	listView->addColumn(i18n("Id"));
	listView->setAllColumnsShowFocus(true);

	hbox_layout->addWidget(left_box);

	QVBoxLayout *right_layout = new QVBoxLayout( 0, 0, 6 );
	QPushButton *ok_button = new QPushButton(i18n("&Load"),this);
	right_layout->addWidget(ok_button);
	QPushButton *cancel_button = new QPushButton(i18n("&Cancel"),this);
	right_layout->addWidget(cancel_button);
	QSpacerItem* spacer = new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );
	right_layout->addItem( spacer );

	hbox_layout->addLayout(right_layout);

	loadDataFromFile();

	connect( search_edit, SIGNAL(textChanged(const QString &)), SLOT(filter(const QString &)) );
	connect( cancel_button, SIGNAL(clicked()), SLOT(reject()) );
	connect( ok_button, SIGNAL(clicked()), SLOT(importSelected()) );
	connect( listView, SIGNAL(doubleClicked( QListViewItem*,const QPoint &, int )), this, SLOT(importSelected()) );
}

USDADataDialog::~USDADataDialog()
{
}

void USDADataDialog::loadDataFromFile()
{
	QString abbrev_file = locate("appdata","abbrev.txt");
	if ( abbrev_file.isEmpty() )
	{
		KMessageBox::error(this,i18n("Unable to find abbrev.txt data file."));
		return;
	}

	QFile file( abbrev_file );
	if ( !file.open( IO_ReadOnly ) )
	{
		kdDebug()<<"Unable to open data file: "<<abbrev_file<<endl;
		return;
	}

	int index = 0;
	QTextStream stream( &file );
	while ( !stream.atEnd() )
	{
		QStringList fields = QStringList::split( "^", stream.readLine() );
		loaded_data << fields;

		QString ing_id = fields[0].mid(1,fields[1].length()-2);
		QString ing_name = fields[1].mid(1,fields[1].length()-2);
		(void)new QListViewItem(listView, ing_name, QString::number(index) ); //using an index instead of the actual id will help find the data later

		index++;
	}
}

void USDADataDialog::filter(const QString & substr)
{
	for ( QListViewItem *it = listView->firstChild(); it; it = it->nextSibling() )
	{
		if ( substr.isEmpty() ) // Don't filter if the filter text is empty
			it->setVisible(true);
		else
			it->setVisible( it->text(0).contains(substr,false) );
	}
}

void USDADataDialog::importSelected()
{
	QListViewItem *item = listView->selectedItem();
	if ( item )
	{
		int index = item->text(1).toInt();
		QStringList data = loaded_data[index];

		int grams_id = database->findExistingUnitByName("g"); //get this id because all data is given per gram
		if ( grams_id == -1 )
		{
			database->createNewUnit("g");
			grams_id = database->lastInsertID();
		}

		IngredientPropertyList property_list; database->loadProperties( &property_list );

		int i = 0;
		for ( QStringList::const_iterator it = data.at(2); property_data_list[i].name; it++, i++ )
		{
			int property_id = property_list.findByName(property_data_list[i].name);
			if ( property_id == -1 )
			{
				//TODO: check that the property doesn't already exist and update it if it does
				database->addProperty(property_data_list[i].name, property_data_list[i].unit);
				property_id = database->lastInsertID();
			}

			double amount = (*it).toDouble()/100.0; //data givin in 100g so divide by 100 to get the amount in 1 gram

			database->addPropertyToIngredient(ingredient.id,property_id,amount,grams_id);
		}

		accept();
	}
	else
		reject();
}

