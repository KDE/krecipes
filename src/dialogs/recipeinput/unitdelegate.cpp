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
#include "datablocks/unit.h"

#include <KComboBox>

//#include <kdebug.h>


UnitDelegate::UnitDelegate(QObject *parent): QStyledItemDelegate(parent)
{
}

void UnitDelegate::loadAllUnitsList( RecipeDB * database )
{
/*	//FIXME: This doesn't respect the limits configured in the program
	database->loadIngredients( &m_ingredientList );
	//FIXME: it would be nice if we could get this hashmap directly from RecipeDB
	ElementList::const_iterator it = m_ingredientList.constBegin();
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
/*	int i = 0;
	ElementList::const_iterator it;
	ElementList::const_iterator list_end;
	if ( index.data(IngredientsEditor::IsHeaderRole).toBool() ) {
		it = m_headerList.constBegin();
		list_end = m_headerList.constEnd();
		QFont font = editor->font();
		font.setBold( true );
		font.setUnderline( true );
		editor->setFont( font );
	} else {
		it = m_ingredientList.constBegin();
		list_end = m_ingredientList.constEnd();
	}
	while ( it != list_end ) {
		editor->insertItem( i, it->name );
		editor->completionObject()->addItem( it->name );
		++i;
		++it;
	}
*/
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

