/***************************************************************************
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef KRETEXTEDIT_H
#define KRETEXTEDIT_H

#include <KTextEdit>
#include <ktextedit.h>
#include <kcompletion.h>

#include "datablocks/elementlist.h"
//Added by qt3to4:
#include <QKeyEvent>

/*  @author Jason Kivlighn
 *  @brief An extended KTextEdit that allows for text completion
 */
class KreTextEdit : public KTextEdit, KCompletionBase
{
    Q_OBJECT

public:
    KreTextEdit( QWidget *parent );

	virtual void createHighlighter();

    virtual void setCompletedText( const QString &text );
    virtual void setCompletedItems( const QStringList &items , bool);

public slots:
	void addCompletionItem( const QString & );
	void removeCompletionItem( const QString & );
	void clearCompletionItems();

protected:
	void keyPressEvent( QKeyEvent * );

private slots:
	void haltCompletion();

private:
	void tryCompletion();
	void rotateText( KCompletionBase::KeyBindingType type );
	bool completing;
	int completion_begin;

};

#endif //KRETEXTEDIT_H
