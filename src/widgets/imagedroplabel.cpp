/***************************************************************************
*   Copyright © 2003-2005 Unai Garro <ugarro@gmail.com>                   *
*   Copyright © 2003-2005 Cyril Bosselut <bosselut@b1project.com>         *
*   Copyright © 2003-2006 Jason Kivlighn <jkivlighn@gmail.com>            *
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "imagedroplabel.h"

#include <kio/netaccess.h>
#include <KMessageBox>
#include <KUrl>

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QImage>
#include <QSize>
#include <QFileInfo>


#define PHOTO_SIZE QSize(221,166)


ImageDropLabel::ImageDropLabel( QWidget *parent ) :
		QLabel( parent ),
		m_sourcePhoto( 0 )
{
	setAcceptDrops( true );
}

void ImageDropLabel::setPhoto( QPixmap * photo )
{
	m_sourcePhoto = photo;
}

void ImageDropLabel::refresh()
{
	QSize sizeDelta = PHOTO_SIZE - m_sourcePhoto->size();
	if ( !sizeDelta.isValid() && !m_sourcePhoto->isNull() ) {
		//The m_sourcePhoto is bigger than this widget
		//so we scale it
		QImage image = m_sourcePhoto->toImage();
		QPixmap pm_scaled;
		pm_scaled = pm_scaled.fromImage( image.scaled( PHOTO_SIZE,
			Qt::KeepAspectRatio, Qt::SmoothTransformation ) );

		setPixmap( pm_scaled );
		*m_sourcePhoto = pm_scaled; // to save scaled later on
	} else {
		setPixmap( *m_sourcePhoto );
	}
}

void ImageDropLabel::dragEnterEvent( QDragEnterEvent* event )
{
	if ( event->mimeData()->hasUrls() || event->mimeData()->hasImage() )
		 event->acceptProposedAction();
}

void ImageDropLabel::dropEvent( QDropEvent* event )
{
	if ( event->mimeData()->hasImage() ) {
		*m_sourcePhoto = qvariant_cast<QPixmap>(event->mimeData()->imageData());
		refresh();
		emit changed();
	} else if ( event->mimeData()->hasUrls() ) {
		QList<QUrl> urlList = event->mimeData()->urls();
		if ( urlList.count() == 1 ) {

			//Find out the file name
			KUrl url = urlList.first();
			QString filename;
			if (!url.isLocalFile()) {
				if (!KIO::NetAccess::download(url,filename,this)) {
					KMessageBox::error(this, KIO::NetAccess::lastErrorString() );
					return;
				}
			} else {
				filename = url.toLocalFile();
			}

			//Load the image
			m_sourcePhoto->load( filename );
			refresh();

			//Delete temp file
			if (!url.isLocalFile()) {
				KIO::NetAccess::removeTempFile( filename );
			}

			emit changed();
		}
	}
}

