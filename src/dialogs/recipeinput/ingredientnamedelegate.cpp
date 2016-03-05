/****************************************************************************
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>       *
*                                                                           *
*   This program is free software; you can redistribute it and/or modify    *
*   it under the terms of the GNU General Public License as published by    *
*   the Free Software Foundation; either version 2 of the License, or       *
*   (at your option) any later version.                                     *
****************************************************************************/

#include "ingredientnamedelegate.h"

#include "dialogs/recipeinput/ingredientseditor.h"
#include "backends/recipedb.h"
#include "datablocks/unit.h"

#include <KComboBox>

//#include <kdebug.h>


IngredientNameDelegate::IngredientNameDelegate(QObject *parent): QStyledItemDelegate(parent)
{
}

void IngredientNameDelegate::loadAllIngredientsList( RecipeDB * database )
{
	//FIXME: This doesn't respect the limits configured in the program
	database->loadIngredients( &m_ingredientList );
	//FIXME: it would be nice if we could get this hashmap directly from RecipeDB
	ElementList::const_iterator it = m_ingredientList.constBegin();
	while ( it != m_ingredientList.constEnd() ) {
		m_nameToIdMap[it->name] = it->id;
		++it;
	}
	connect( database, SIGNAL(ingredientCreated(const Element&)),
		this, SLOT(ingredientCreatedSlot(const Element&)) );
	connect( database, SIGNAL(ingredientRemoved(int)),
		this, SLOT(ingredientRemovedSlot(int)) );
}

void IngredientNameDelegate::ingredientCreatedSlot( const Element & element )
{
	m_nameToIdMap[element.name] = element.id;
	m_ingredientList << element;
}

void IngredientNameDelegate::ingredientRemovedSlot( int id )
{
	m_nameToIdMap.remove( m_nameToIdMap.keys(id).first() );
}

QWidget * IngredientNameDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
{
	KComboBox * editor = new KComboBox( parent );
	editor->setAutoFillBackground( true );
	editor->setEditable( true );
	editor->setCompletionMode( KGlobalSettings::CompletionPopup );
	int i = 0;
	ElementList::const_iterator it = m_ingredientList.constBegin();
	while ( it != m_ingredientList.constEnd() ) {
		editor->insertItem( i, it->name );
		editor->completionObject()->addItem( it->name );
		++i;
		++it;
	}
	return editor;
}

void IngredientNameDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QString text = index.data( Qt::EditRole ).toString();
	KComboBox *comboBox = static_cast<KComboBox*>( editor );
	comboBox->setEditText( text );
}

void IngredientNameDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	KComboBox *comboBox = static_cast<KComboBox*>( editor );
	QString text = comboBox->currentText();
	model->setData( index, text, Qt::EditRole );
	if ( m_nameToIdMap.contains(text) ) {
		model->setData( index, m_nameToIdMap[text], IngredientsEditor::IdRole );
	} else {
		model->setData( index, RecipeDB::InvalidId, IngredientsEditor::IdRole );
	}
}

void IngredientNameDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex & index ) const
{
	Q_UNUSED(index)
	editor->setGeometry(option.rect);
}

