/****************************************************************************
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>       *
*                                                                           *
*   This program is free software; you can redistribute it and/or modify    *
*   it under the terms of the GNU General Public License as published by    *
*   the Free Software Foundation; either version 2 of the License, or       *
*   (at your option) any later version.                                     *
****************************************************************************/

#include "amountdelegate.h"

#include "dialogs/recipeinput/ingredientseditor.h"
#include "widgets/fractioninput.h"
#include "datablocks/mixednumberrange.h"

#include <KLineEdit>


AmountDelegate::AmountDelegate(QObject *parent): QStyledItemDelegate(parent)
{
}


QWidget * AmountDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem & option ,
	const QModelIndex & index ) const
{
	Q_UNUSED(option)
	Q_UNUSED(index)
	FractionInput * editor = new FractionInput( parent );
	return editor;
}

void AmountDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	FractionInput * fractionInput = static_cast<FractionInput*>( editor );
	fractionInput->setText( index.data( Qt::EditRole ).toString() );
}

void AmountDelegate::setModelData(QWidget *editor, 
	QAbstractItemModel *model, const QModelIndex &index) const
{
	//Set the amount string in the model
	FractionInput * fractionInput = static_cast<FractionInput*>( editor );
	model->setData( index, fractionInput->text(), Qt::EditRole );

	//Set if the amount is plural in the model
	QModelIndex unitIndex = model->index( index.row(), IngredientsEditor::unitColumn() );
	bool isPlural = fractionInput->valueRange().isPlural();
	model->setData( unitIndex, QVariant(isPlural), IngredientsEditor::IsPluralRole );
}

void AmountDelegate::updateEditorGeometry(QWidget *editor, 
	const QStyleOptionViewItem &option, const QModelIndex & index ) const
{
	Q_UNUSED(index)
	editor->setGeometry(option.rect);
}

