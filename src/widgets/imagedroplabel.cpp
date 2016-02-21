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

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QImage>

ImageDropLabel::ImageDropLabel( QWidget *parent, QPixmap &_sourcePhoto ) : QLabel( parent ),
		sourcePhoto( _sourcePhoto )
{
	setAcceptDrops( true );
}

void ImageDropLabel::dragEnterEvent( QDragEnterEvent* event )
{
	if ( event->mimeData()->hasImage() )
		 event->acceptProposedAction();
}

void ImageDropLabel::dropEvent( QDropEvent* event )
{
	QImage image;

	if ( event->mimeData()->hasImage() ) {
		image = qvariant_cast<QImage>(event->mimeData()->imageData());
		if ( ( image.width() > width() || image.height() > height() ) || ( image.width() < width() && image.height() < height() ) ) {
			QPixmap pm_scaled;
			pm_scaled = pm_scaled.fromImage( image.scaled( QSize( width(), height()),
				Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
			setPixmap( pm_scaled );

			sourcePhoto = pm_scaled; // to save scaled later on
		}
		else {
			sourcePhoto = QPixmap::fromImage( image );
			setPixmap( sourcePhoto );
		}

		emit changed();
	}
}

