/****************************************************************************
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>       *
*                                                                           *
*   This program is free software; you can redistribute it and/or modify    *
*   it under the terms of the GNU General Public License as published by    *
*   the Free Software Foundation; either version 2 of the License, or       *
*   (at your option) any later version.                                     *
****************************************************************************/

#include "unitdelegate.h"

#include "dialogs/recipeinput/ingredientseditor.h"
#include "backends/recipedb.h"

#include <KComboBox>

//#include <kdebug.h>


UnitDelegate::UnitDelegate(QObject *parent): QStyledItemDelegate(parent)
{
}

void UnitDelegate::loadAllUnitsList( RecipeDB * database )
{
	database->loadUnits( &m_unitList );
	//FIXME: it would be nice if we could get this hashmap directly from RecipeDB
	/*ElementList::const_iterator it = m_ingredientList.constBegin();
	while ( it != m_ingredientList.constEnd() ) {
		m_ingredientNameToIdMap[it->name] = it->id;
		++it;
	}
	connect( database, SIGNAL(ingredientCreated(const Element&)),
		this, SLOT(ingredientCreatedSlot(const Element&)) );
	connect( database, SIGNAL(ingredientRemoved(int)),
		this, SLOT(ingredientRemovedSlot(int)) );
*/
}

/*void UnitDelegate::ingredientCreatedSlot( const Element & element )
{
	m_ingredientNameToIdMap[element.name] = element.id;
	m_ingredientList << element;
}*/

/*void UnitDelegate::ingredientRemovedSlot( int id )
{
	m_ingredientNameToIdMap.remove( m_ingredientNameToIdMap.keys(id).first() );
}*/

QWidget * UnitDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */,
	const QModelIndex & index ) const
{
	//Set up the combo box
	KComboBox * editor = new KComboBox( parent );
	editor->setAutoFillBackground( true );
	editor->setEditable( true );
	editor->setCompletionMode( KGlobalSettings::CompletionPopup );

	//Fill the items and the completion objects
	UnitList::const_iterator unit_it = m_unitList.constBegin();
	if ( !index.data(IngredientsEditor::IsPluralRole).toBool() ) {
		int i = 0;
		while ( unit_it != m_unitList.constEnd() ) {
			editor->insertItem( i, unit_it->name() );
			editor->completionObject()->addItem( unit_it->name() );
			++i; ++unit_it;
		}
	} else {
		int i = 0;
		while ( unit_it != m_unitList.constEnd() ) {
			editor->insertItem( i, unit_it->plural() );
			editor->completionObject()->addItem( unit_it->plural() );
			++i; ++unit_it;
		}
	}
	return editor;
}

void UnitDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
/*	QString text = index.data( Qt::EditRole ).toString();
	KComboBox *comboBox = static_cast<KComboBox*>( editor );
	comboBox->setEditText( text );*/
}

void UnitDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
/*	KComboBox *comboBox = static_cast<KComboBox*>( editor );
	QString text = comboBox->currentText();
	model->setData( index, text, Qt::EditRole );

	if ( index.data(IngredientsEditor::IsHeaderRole).toBool() ) {
		//The edited item is a header
		if ( m_headerNameToIdMap.contains(text) ) {
			model->setData( index, m_headerNameToIdMap[text], IngredientsEditor::IdRole );
		} else {
			model->setData( index, RecipeDB::InvalidId, IngredientsEditor::IdRole );
		}
	} else {
		//The edited item is an ingredient
		if ( m_ingredientNameToIdMap.contains(text) ) {
			model->setData( index, m_ingredientNameToIdMap[text], IngredientsEditor::IdRole );
		} else {
			model->setData( index, RecipeDB::InvalidId, IngredientsEditor::IdRole );
		}
	}*/
}

void UnitDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex & index ) const
{
	Q_UNUSED(index)
	editor->setGeometry(option.rect);
}

