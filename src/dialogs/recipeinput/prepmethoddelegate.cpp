/****************************************************************************
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>       *
*                                                                           *
*   This program is free software; you can redistribute it and/or modify    *
*   it under the terms of the GNU General Public License as published by    *
*   the Free Software Foundation; either version 2 of the License, or       *
*   (at your option) any later version.                                     *
****************************************************************************/

#include "prepmethoddelegate.h"

#include "dialogs/recipeinput/ingredientseditor.h"
#include "widgets/fractioninput.h"
#include "datablocks/mixednumberrange.h"

#include <KLineEdit>


PrepMethodDelegate::PrepMethodDelegate(QObject *parent): QStyledItemDelegate(parent)
{
}

void PrepMethodDelegate::loadAllPrepMethodsList( RecipeDB * database )
{
	database->loadPrepMethods( &m_prepMethodsList );
	//FIXME: it would be nice if we could get this hash map from RecipeDB directly
	ElementList::iterator it = m_prepMethodsList.begin();
	while ( it != m_prepMethodsList.end() ) {
		m_nameToIdMap[it->name] = it->id;
		m_idToIteratorMap[it->id] = it;
		++it;
	}

	//TODO: connect database signals
}

QWidget * PrepMethodDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem & option ,
	const QModelIndex & index ) const
{
	Q_UNUSED(option)
	Q_UNUSED(index)
	KLineEdit* editor = new KLineEdit( parent );

	//Fill the completion items
	ElementList::const_iterator it = m_prepMethodsList.constBegin();
	while ( it != m_prepMethodsList.constEnd() ) {
		editor->completionObject()->addItem( it->name );
		++it;
	}

	return editor;
}

void PrepMethodDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	KLineEdit * lineEdit = static_cast<KLineEdit*>( editor );
	lineEdit->setText( index.data( Qt::EditRole ).toString() );
}

void PrepMethodDelegate::setModelData(QWidget *editor, 
	QAbstractItemModel *model, const QModelIndex &index) const
{
	KLineEdit * lineEdit = static_cast<KLineEdit*>( editor );
	//Set the preparation methods id's in the model
	QList<QVariant> ids;
	QStringList stringList = lineEdit->text().split(",",QString::SkipEmptyParts);
	QStringList prepMethodNamesList;
	foreach( QString name, stringList ) {
		name = name.trimmed();
		if ( !name.isEmpty() ) {
			if ( m_nameToIdMap.contains(name) ) {
				ids << m_nameToIdMap[name];
			} else {
				ids << RecipeDB::InvalidId;
			}
			prepMethodNamesList << name;
		}
	}
	model->setData( index, QVariant(ids), IngredientsEditor::IdRole );
	//Set the preparation methods string in the model
	QString prepMethodsString = prepMethodNamesList.join(", ");
	model->setData( index, QVariant(prepMethodsString), Qt::EditRole );
}

void PrepMethodDelegate::updateEditorGeometry(QWidget *editor, 
	const QStyleOptionViewItem &option, const QModelIndex & index ) const
{
	Q_UNUSED(index)
	editor->setGeometry(option.rect);
}

