/***************************************************************************
*   Copyright (C) 2004 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "kretextedit.h"

#include <q3textstream.h>
//Added by qt3to4:
#include <QKeyEvent>
#include <Q3TextEdit>
#include <KStandardShortcut>

#include <kaction.h>
#include <kdebug.h>

KreTextEdit::KreTextEdit( QWidget *parent ) : Q3TextEdit( parent ), KCompletionBase()
{
	KCompletion * comp = completionObject(); //creates the completion object
	comp->setIgnoreCase( true );

	completing = false;

	connect( this, SIGNAL( clicked( int, int ) ), SLOT( haltCompletion() ) );
}

void KreTextEdit::haltCompletion()
{
	completing = false;
}

void KreTextEdit::keyPressEvent( QKeyEvent *e )
{
	// Filter key-events if completion mode is not set to CompletionNone

	KeyBindingMap keys = getKeyBindings();
	KShortcut cut;
	bool noModifier = ( e->modifiers() == Qt::NoModifier || e->modifiers() == Qt::ShiftModifier );

	if ( noModifier ) {
		QString keycode = e->text();
		if ( !keycode.isEmpty() && keycode.unicode() ->isPrint() ) {
			Q3TextEdit::keyPressEvent ( e );
			tryCompletion();
			e->accept();
			return ;
		}
	}

	// Handles completion
	if ( keys[ TextCompletion ].isEmpty() )
		cut = KStandardShortcut::shortcut( KStandardShortcut::TextCompletion );
	else
		cut = keys[ TextCompletion ];

	//using just the standard Ctrl+E isn't user-friendly enough for Grandma...
	if ( completing && ( cut.contains( e->key() ) || e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return ) ) {
		int paraFrom, indexFrom, paraTo, indexTo;
		getSelection ( &paraFrom, &indexFrom, &paraTo, &indexTo );

		removeSelection();
		setCursorPosition( paraTo, indexTo );

		completing = false;
		return ;
	}

	// handle rotation

	// Handles previous match
	if ( keys[ PrevCompletionMatch ].isEmpty() )
		cut = KStandardShortcut::shortcut( KStandardShortcut::PrevCompletion );
	else
		cut = keys[ PrevCompletionMatch ];

	if ( cut.contains( e->key() ) ) {
		rotateText( KCompletionBase::PrevCompletionMatch );
		return ;
	}

	// Handles next match
	if ( keys[ NextCompletionMatch ].isEmpty() )
		cut = KStandardShortcut::shortcut( KStandardShortcut::NextCompletion );
	else
		cut = keys[ NextCompletionMatch ];

	if ( cut.contains( e->key() ) ) {
		rotateText( KCompletionBase::NextCompletionMatch );
		return ;
	}

	//any other key events will end any text completion execpt for modifiers
	switch ( e->key() ) {
	case Qt::Key_Shift:
	case Qt::Key_Control:
	case Qt::Key_Alt:
	case Qt::Key_Meta:
		break;
	default:
		completing = false;
		break;
	}

	// Let KTextEdit handle any other keys events.
	Q3TextEdit::keyPressEvent ( e );
}

void KreTextEdit::setCompletedText( const QString &txt )
{
	int para, index;
	getCursorPosition( &para, &index );

	QString para_text = text( para );
	int word_length = index - completion_begin;

	insert( txt.right( txt.length() - word_length ) );
	setSelection( para, index, para, completion_begin + txt.length() );
	setCursorPosition( para, index );

	completing = true;
}

void KreTextEdit::setCompletedItems( const QStringList &/*items*/ , bool)
{}

void KreTextEdit::tryCompletion()
{
	int para, index;
	getCursorPosition( &para, &index );

	QString para_text = text( para );
	if ( para_text.at( index ).isSpace() || completing ) {
		if ( !completing )
			completion_begin = para_text.lastIndexOf( ' ', index - 1 ) + 1;

		QString completing_word = para_text.mid( completion_begin, index - completion_begin );

		QString match = compObj() ->makeCompletion( completing_word );

		if ( !match.isNull() && match != completing_word )
			setCompletedText( match );
		else
			completing = false;
	}
}

void KreTextEdit::rotateText( KCompletionBase::KeyBindingType type )
{
	KCompletion * comp = compObj();
	if ( comp && completing &&
	        ( type == KCompletionBase::PrevCompletionMatch ||
	          type == KCompletionBase::NextCompletionMatch ) ) {
		QString input = ( type == KCompletionBase::PrevCompletionMatch ) ? comp->previousMatch() : comp->nextMatch();

		// Skip rotation if previous/next match is null or the same text
		int para, index;
		getCursorPosition( &para, &index );
		QString para_text = text( para );
		QString complete_word = para_text.mid( completion_begin, index - completion_begin );
		if ( input.isNull() || input == complete_word )
			return ;
		setCompletedText( input );
	}
}

void KreTextEdit::addCompletionItem( const QString &name )
{
	compObj() ->addItem( name );
}

void KreTextEdit::removeCompletionItem( const QString &name )
{
	compObj() ->removeItem( name );
}

void KreTextEdit::clearCompletionItems()
{
	compObj() ->clear();
}

#include "kretextedit.moc"
