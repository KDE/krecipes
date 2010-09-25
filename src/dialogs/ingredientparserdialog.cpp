/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "ingredientparserdialog.h"

#include <QPushButton>
#include <KTextEdit>
#include <QLabel>

#include <q3header.h>
#include <QApplication>
#include <QClipboard>

#include <QHBoxLayout>
#include <QList>

#include <klocale.h>
#include <kdebug.h>
#include <k3listview.h>
#include <kpushbutton.h>
#include <kmessagebox.h>
#include <kaction.h>
#include <KVBox>
#include <kmenu.h>
#include <kvbox.h>

#include "datablocks/mixednumber.h"
#include "widgets/inglistviewitem.h"

IngredientParserDialog::IngredientParserDialog( const UnitList &units, QWidget* parent, const char* name )
		: KDialog( parent ),
		m_unitList(units)
{
	//setButtonBoxOrientation( Qt::Vertical );
	setObjectName( name );
	setCaption(i18nc( "@title:window", "Ingredient Parser" ));
	setButtons(KDialog::Ok | KDialog::Cancel);
	setDefaultButton(KDialog::Ok);
	setModal( true );

	KVBox *page = new KVBox( this );
	setMainWidget( page );

	textLabel1 = new QLabel( page );
	textLabel1->setObjectName( "textLabel1" );
	textLabel1->setTextFormat( Qt::RichText );

	ingredientTextEdit = new KTextEdit( page );
	ingredientTextEdit->setObjectName( "ingredientTextEdit" );
	ingredientTextEdit->setAcceptRichText( false );

	parseButton = new KPushButton( page );

	previewLabel = new QLabel( page );
	previewLabel->setObjectName( "previewLabel" );
	previewLabel->setTextFormat( Qt::RichText );

	previewIngView = new K3ListView( page );
	previewIngView->setSorting(-1);
	previewIngView->addColumn( i18nc( "@title:column", "Ingredient" ) );
	previewIngView->addColumn( i18nc( "@title:column", "Amount" ) );
	previewIngView->addColumn( i18nc( "@title:column", "Unit" ) );
	previewIngView->addColumn( i18nc( "@title:column", "Preparation Method" ) );

	languageChange();
	setInitialSize( QSize(577, 371).expandedTo(minimumSizeHint()) );

	previewIngView->setItemsRenameable( true );
	previewIngView->setRenameable( 0, true );
	previewIngView->setRenameable( 1, true );
	previewIngView->setRenameable( 2, true );
	previewIngView->setRenameable( 3, true );

	previewIngView->setSelectionMode( Q3ListView::Extended );

	ingredientTextEdit->setText( QApplication::clipboard()->text() );
	ingredientTextEdit->selectAll();

	QWidget *buttonWidget = new QWidget( page );
	QHBoxLayout *buttonBox = new QHBoxLayout(buttonWidget);
	QSpacerItem *horizontalSpacing = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	buttonGroup = new QPushButton( i18nc("@action:button", "Set &Header"), buttonWidget );
	buttonGroup->setWhatsThis( i18nc( "@info:whatsthis", "If an ingredient header is detected as an ingredient, select it and click this button so that Krecipes will recognize it as a header.  All the ingredients below the header will be included within that group.\n\nAlternatively, if you select multiple ingredients and click this button, those ingredients will be grouped together.") );
	buttonBox->addWidget( buttonGroup );
	buttonBox->addItem( horizontalSpacing );

	KMenu *kpop = new KMenu( previewIngView );
	kpop->addAction( i18nc( "@item:inmenu", "&Delete" ), this, SLOT( removeIngredient() ), Qt::Key_Delete );
	kpop->addAction( i18nc( "@item:inmenu", "Set &Header") , this, SLOT( convertToHeader() ) );

	connect( parseButton, SIGNAL(clicked()), this, SLOT(parseText()) );
	connect( buttonGroup, SIGNAL(clicked()), this, SLOT(convertToHeader()) );
}

IngredientParserDialog::~IngredientParserDialog()
{
	// no need to delete child widgets, Qt does it all for us
}

void IngredientParserDialog::languageChange()
{
	textLabel1->setText( i18nc( "@info", "<para>To use: Paste a list of ingredient below, "
		"click <interface>Parse Text</interface>, and then you may correct any incorrectly "
		"parsed ingredients.</para><para><warning>Fields will be truncated if longer than "
		"the database allows</warning></para>" ) );
	previewLabel->setText( i18nc("@title", "Ingredients as understood by Krecipes:") );
		 textLabel1->setWordWrap(true);
	parseButton->setText( i18nc( "@action:button", "Parse Text" ) );
	previewIngView->header()->setLabel( 0, i18nc( "@title:column", "Ingredient" ) );
	previewIngView->header()->setLabel( 1, i18nc( "@title:column", "Amount" ) );
	previewIngView->header()->setLabel( 2, i18nc( "@title:column", "Unit" ) );
	previewIngView->header()->setLabel( 3, i18nc( "@title:column", "Preparation Method" ) );
}

void IngredientParserDialog::accept()
{
	for ( Q3ListViewItem *it = previewIngView->firstChild(); it; it = it->nextSibling() ) {
		if ( it->rtti() == INGGRPLISTVIEWITEM_RTTI ) {
			QString group = ((IngGrpListViewItem*)it)->group();
			for ( IngListViewItem *sub_it = (IngListViewItem*)it->firstChild(); sub_it; sub_it = (IngListViewItem*)sub_it->nextSibling() ) {
				Ingredient ing = sub_it->ingredient();
				ing.group = group;
				m_ingList.append(ing);
			}
		}
		else
			m_ingList.append(((IngListViewItem*)it)->ingredient());
	}

	QDialog::accept();
}

void IngredientParserDialog::removeIngredient()
{
	delete previewIngView->selectedItem();
	if ( !previewIngView->firstChild() )
		enableButtonOk( false );
}

void IngredientParserDialog::convertToHeader()
{
	QList<Q3ListViewItem*> items = previewIngView->selectedItems();
	if ( items.isEmpty() )
		return;
	else if ( items.count() > 1 )
		convertToHeader(items);
	else { //items.count = 1
		Q3ListViewItem *item = items.first();
		if ( item->rtti() == INGLISTVIEWITEM_RTTI ) {
			Q3ListViewItem *new_item = new IngGrpListViewItem(previewIngView,
			(item->parent())?item->parent():item,
			((IngListViewItem*)item)->ingredient().name, -1);

			Q3ListViewItem *next_sibling;
			Q3ListViewItem *last_item = 0;
			for ( Q3ListViewItem * it = (item->parent())?item->nextSibling():new_item->nextSibling(); it; it = next_sibling ) {
				if ( it->rtti() == INGGRPLISTVIEWITEM_RTTI )
					break;

				next_sibling = it->nextSibling(); //get the next sibling of this item before we move it

				if ( it->parent() )
					it->parent()->takeItem(it);
				else
					previewIngView->takeItem( it );

				new_item->insertItem( it );

				if ( last_item )
					it->moveItem( last_item );
				last_item = it;
			}

			new_item->setOpen(true);

			delete item;
		}
	}
}

void IngredientParserDialog::convertToHeader( const QList<Q3ListViewItem*> &items )
{
	//Port to kde4
	/*
	if ( items.isEmpty() > 0 ) {
		QListIterator<Q3ListViewItem> it(items);
		Q3ListViewItem *item = it.current();

		if ( item->rtti() != INGLISTVIEWITEM_RTTI )
			return;

		QString group = ((IngListViewItem*)item)->ingredient().name;
		Q3ListViewItem *ingGroupItem = new IngGrpListViewItem(previewIngView,
			(item->parent())?item->parent():item, group, -1);
		delete item; //delete the ingredient header which was detected as an ingredient
		++it;

		Q3ListViewItem *last_item = 0;
		while ( (item = it.current()) != 0 ) {
			//ignore anything that isn't an ingredient (e.g. headers)
			if ( item->rtti() == INGLISTVIEWITEM_RTTI ) {
				if ( item->parent() )
					item->parent()->takeItem(item);
				else
					previewIngView->takeItem( item );

				ingGroupItem->insertItem( item );

				if ( last_item )
					item->moveItem( last_item );
				last_item = item;
			}

			++it;
		}

		ingGroupItem->setOpen(true);
		previewIngView->clearSelection();
	}
	*/
}

void IngredientParserDialog::parseText()
{
	previewIngView->clear();

	Q3ListViewItem *last_item = 0;

	int line_num = 0;
	QStringList ingredients;
	if (ingredientTextEdit->document()->isEmpty())
		ingredients = QStringList();
	else
		ingredients = ingredientTextEdit->toPlainText().split( '\n', QString::SkipEmptyParts);

	for ( QStringList::const_iterator it = ingredients.constBegin(); it != ingredients.constEnd(); ++it ) {
		QString line = (*it).simplified();

		++line_num;
		int format_at = 0;
		Ingredient ing;


		//======amount======//
		int first_space = line.indexOf( " ", format_at+1 );
		if ( first_space == -1 )
			first_space = line.length();

		int second_space = line.indexOf( " ", first_space+1 );
		if ( second_space == -1 )
			second_space = line.length();

		Ingredient i;
		bool ok;
		i.setAmount(line.mid(format_at,second_space-format_at),&ok);
		if ( !ok ) {
			i.setAmount(line.mid(format_at,first_space-format_at),&ok);
			if ( ok ) format_at = first_space+1;
		}
		else
			format_at = second_space+1;

		if ( ok ) {
			ing.amount = i.amount;
			ing.amount_offset = i.amount_offset;
		}
		else
			kDebug()<<"no amount on line "<<line_num;


		//======unit======//
		first_space = line.indexOf( " ", format_at+1 );
		if ( first_space == -1 )
			first_space = line.length();

		bool isUnit = false;
		QString unitCheck = line.mid(format_at,first_space-format_at);

		for ( UnitList::const_iterator unit_it = m_unitList.constBegin(); unit_it != m_unitList.constEnd(); ++unit_it ) {
			if ( (*unit_it).name() == unitCheck || (*unit_it).plural() == unitCheck || (*unit_it).nameAbbrev() == unitCheck || (*unit_it).pluralAbbrev() == unitCheck ) {
				isUnit = true;
				format_at = first_space+1;
				break;
			}
		}

		if ( isUnit ) {
			ing.units.setName(unitCheck);
			ing.units.setPlural(unitCheck);
		}
		else
			kDebug()<<"no unit on line "<<line_num;


		//======ingredient======//
		int ing_end = line.indexOf( QRegExp("(,|;)"), format_at+1 );
		if ( ing_end == -1 )
			ing_end = line.length();

		ing.name = line.mid(format_at,ing_end-format_at);
		format_at = ing_end+2;


		//======prep method======//
		int end = line.length();
		ing.prepMethodList = ElementList::split(",",line.mid(format_at,end-format_at));

		last_item = new IngListViewItem(previewIngView,last_item,ing);
		enableButtonOk( true );
	}
}

#include "ingredientparserdialog.moc"
