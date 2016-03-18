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


QWidget * PrepMethodDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem & option ,
	const QModelIndex & index ) const
{
	Q_UNUSED(option)
	Q_UNUSED(index)
	KLineEdit* editor = new KLineEdit( parent );
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
	//Set the preparation methods string in the model
	KLineEdit * lineEdit = static_cast<KLineEdit*>( editor );
	model->setData( index, lineEdit->text(), Qt::EditRole );
	//Set the preparation methods id's in the model
	//TODO
}

void PrepMethodDelegate::updateEditorGeometry(QWidget *editor, 
	const QStyleOptionViewItem &option, const QModelIndex & index ) const
{
	Q_UNUSED(index)
	editor->setGeometry(option.rect);
}

