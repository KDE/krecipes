/* ============================================================
 * Authors: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *          Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2004-11-22
 * Description : a bar widget to display image thumbnails
 *
 * Copyright © 2004-2005 Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright © 2005-2006 Tom Albers <tomalbers@kde.nl>
 * Copyright © 2004-2006 Gilles Caulier <caulier dot gilles at kdemail dot net>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Local includes.

#include "thumbbar.h"

//Added by qt3to4:
#include <QPaintEvent>
#include <QFrame>
#include <QMouseEvent>

// C Ansi includes.

extern "C"
{
#include <unistd.h>
}

// C++ includes.

#include <cmath>

// Qt includes.

#include <QPixmap>
#include <QTimer>
#include <QPainter>
#include <q3dict.h>
#include <qpoint.h>
#include <QDateTime>
#include <QFileInfo>

// KDE includes.

#include <kcodecs.h>
#include <kapplication.h>
#include <kiconloader.h>
#include <kio/previewjob.h>
#include <klocale.h>
#include <kmimetype.h>
#include <kfileitem.h>
#include <kglobal.h>


class ThumbBarViewPriv
{
public:

    ThumbBarViewPriv()
    {
        firstItem  = 0;
        lastItem   = 0;
        currItem   = 0;
        count      = 0;
        itemDict.setAutoDelete(false);
    }

    bool                      clearing;

    int                       count;
    int                       margin;
    int                       tileSize;
    int                       orientation;

    QTimer                   *timer;

    ThumbBarItem             *firstItem;
    ThumbBarItem             *lastItem;
    ThumbBarItem             *currItem;

    ThumbBarToolTip          *tip;

    Q3Dict<ThumbBarItem>       itemDict;
};

class ThumbBarItemPriv
{
public:

    ThumbBarItemPriv()
    {
        pos    = 0;
        pixmap = 0;
        next   = 0;
        prev   = 0;
        view   = 0;
    }

    int           pos;

    QPixmap      *pixmap;

    QString          url;

    ThumbBarItem *next;
    ThumbBarItem *prev;

    ThumbBarView *view;

};

ThumbBarView::ThumbBarView(QWidget* parent, int orientation)
            : Q3ScrollView(parent)
{
    d = new ThumbBarViewPriv;
    d->margin      = 5;
    d->tileSize    = 200;
    d->orientation = orientation;

    d->tip   = new ThumbBarToolTip(this);
    d->timer = new QTimer(this);

    connect(d->timer, SIGNAL(timeout()),
            this, SLOT(slotUpdate()));

    viewport()->setBackgroundMode(Qt::NoBackground);
    viewport()->setMouseTracking(true);
    setFrameStyle(QFrame::NoFrame);

    if (d->orientation == Vertical)
    {
       setHScrollBarMode(Q3ScrollView::AlwaysOff);
       setFixedWidth(d->tileSize + 2*d->margin
                     + verticalScrollBar()->sizeHint().width());
    }
    else
    {
       setVScrollBarMode(Q3ScrollView::AlwaysOff);
       setFixedHeight(d->tileSize + 2*d->margin
                      + horizontalScrollBar()->sizeHint().height());
    }
}

ThumbBarView::~ThumbBarView()
{
    clear(false);

    delete d->timer;
    delete d->tip;
    delete d;
}

int ThumbBarView::countItems()
{
    return d->count;
}

void ThumbBarView::clear(bool updateView)
{
    d->clearing = true;

    ThumbBarItem *item = d->firstItem;
    while (item)
    {
        ThumbBarItem *tmp = item->d->next;
        delete item;
        item = tmp;
    }

    d->firstItem = 0;
    d->lastItem  = 0;
    d->count     = 0;
    d->currItem  = 0;

    if (updateView)
        slotUpdate();

    d->clearing = false;
}

void ThumbBarView::triggerUpdate()
{
    d->timer->setSingleShot( true );
    d->timer->start( 0 );
}

ThumbBarItem* ThumbBarView::currentItem() const
{
    return d->currItem;
}

ThumbBarItem* ThumbBarView::firstItem() const
{
    return d->firstItem;
}

ThumbBarItem* ThumbBarView::lastItem() const
{
    return d->lastItem;
}

ThumbBarItem* ThumbBarView::findItem(const QPoint& pos) const
{
    int itemPos;

    if (d->orientation == Vertical)
        itemPos = pos.y();
    else
        itemPos = pos.x();

    for (ThumbBarItem *item = d->firstItem; item; item = item->d->next)
    {
        if (itemPos >= item->d->pos && itemPos <= (item->d->pos+d->tileSize+2*d->margin))
        {
            return item;
        }
    }

    return 0;
}

ThumbBarItem* ThumbBarView::findItemByURL(const QString& url) const
{
    for (ThumbBarItem *item = d->firstItem; item; item = item->d->next)
    {
        if (url == item->url())
        {
            return item;
        }
    }

    return 0;
}

void ThumbBarView::setSelected(ThumbBarItem* item)
{
    if (!item)
        return;

    if (d->currItem == item)
        return;

    if (d->currItem)
    {
        ThumbBarItem* item = d->currItem;
        d->currItem = 0;
        item->repaint();
    }

    d->currItem = item;
    if (d->currItem)
    {
        // We want the complete thumb visible and the next one.
        // find the middle of the image and give a margin of 1,5 image
        // When changed, watch regression for bug 104031
        if (d->orientation == Qt::Vertical)
            ensureVisible(0, (int)(item->d->pos+d->margin+d->tileSize*.5),
                          0, (int)(d->tileSize*1.5+3*d->margin));
        else
            ensureVisible((int)(item->d->pos+d->margin+d->tileSize*.5), 0,
                          (int)(d->tileSize*1.5+3*d->margin), 0);

        item->repaint();
        emit signalURLSelected(item->url());
    }
}

void ThumbBarView::invalidateThumb(ThumbBarItem* /*item*/)
{
}

void ThumbBarView::viewportPaintEvent(QPaintEvent* e)
{
    int cy = 0, cx = 0, ts = 0, y1 = 0, y2 = 0, x1 = 0, x2 = 0;
    QPixmap bgPix, tile;
    QRect er(e->rect());

    QColor hilightColor = QPalette().highlight().color();
    QColor backgroundColor = QPalette().window().color();

    if (d->orientation == Vertical)
    {
       cy = viewportToContents(er.topLeft()).y();

       bgPix = QPixmap(contentsRect().width(), er.height());

       ts = d->tileSize + 2*d->margin;
       tile = QPixmap(visibleWidth(), ts);

       y1 = (cy/ts)*ts;
       y2 = ((y1 + er.height())/ts +1)*ts;
    }
    else
    {
       cx = viewportToContents(er.topLeft()).x();

       bgPix = QPixmap(er.width(), contentsRect().height());

       ts = d->tileSize + 2*d->margin;
       tile = QPixmap(ts, visibleHeight());

       x1 = (cx/ts)*ts;
       x2 = ((x1 + er.width())/ts +1)*ts;
    }

    bgPix.fill(backgroundColor);

    for (ThumbBarItem *item = d->firstItem; item; item = item->d->next)
    {
        if (d->orientation == Vertical)
        {
            if (y1 <= item->d->pos && item->d->pos <= y2)
            {
                if (item == d->currItem)
                    tile.fill(hilightColor);
                else
                    tile.fill(backgroundColor);

                QPainter p(&tile);
                p.setPen(Qt::white);
                p.drawRect(0, 0, tile.width(), tile.height());
                p.end();

                if (item->d->pixmap)
                {
                    int x = (tile.width() -item->d->pixmap->width())/2;
                    int y = (tile.height()-item->d->pixmap->height())/2;
                    bitBlt(&tile, x, y, item->d->pixmap);
                }

                bitBlt(&bgPix, 0, item->d->pos - cy, &tile);
            }
        }
        else
        {
            if (x1 <= item->d->pos && item->d->pos <= x2)
            {
                if (item == d->currItem)
                    tile.fill(hilightColor);
                else
                    tile.fill(backgroundColor);

                QPainter p(&tile);
                p.setPen(Qt::white);
                p.drawRect(0, 0, tile.width(), tile.height());
                p.end();

                if (item->d->pixmap)
                {
                    int x = (tile.width() -item->d->pixmap->width())/2;
                    int y = (tile.height()-item->d->pixmap->height())/2;
                    bitBlt(&tile, x, y, item->d->pixmap);
                }

                bitBlt(&bgPix, item->d->pos - cx, 0, &tile);
            }
        }
    }

    if (d->orientation == Vertical)
       bitBlt(viewport(), 0, er.y(), &bgPix);
    else
       bitBlt(viewport(), er.x(), 0, &bgPix);
}

void ThumbBarView::contentsMousePressEvent(QMouseEvent* e)
{
    ThumbBarItem* barItem = 0;

    if (d->orientation == Vertical)
    {
       int y = e->pos().y();

       for (ThumbBarItem *item = d->firstItem; item; item = item->d->next)
       {
           if (y >= item->d->pos &&
               y <= (item->d->pos + d->tileSize + 2*d->margin))
           {
                barItem = item;
                break;
           }
       }
    }
    else
    {
       int x = e->pos().x();

       for (ThumbBarItem *item = d->firstItem; item; item = item->d->next)
       {
           if (x >= item->d->pos &&
               x <= (item->d->pos + d->tileSize + 2*d->margin))
           {
                barItem = item;
                break;
           }
       }
    }

    if (!barItem || barItem == d->currItem)
        return;

    if (d->currItem)
    {
        ThumbBarItem* item = d->currItem;
        d->currItem = 0;
        item->repaint();
    }

    d->currItem = barItem;
    barItem->repaint();

    emit signalURLSelected(barItem->url());
}

void ThumbBarView::insertItem(ThumbBarItem* item)
{
    if (!item) return;

    if (!d->firstItem)
    {
        d->firstItem = item;
        d->lastItem  = item;
        item->d->prev = 0;
        item->d->next = 0;
    }
    else
    {
        d->lastItem->d->next = item;
        item->d->prev = d->lastItem;
        item->d->next = 0;
        d->lastItem = item;

    }

    d->itemDict.insert(item->url(), item);

    d->count++;
    triggerUpdate();
    emit signalItemAdded();
}

void ThumbBarView::removeItem(ThumbBarItem* item)
{
    if (!item) return;

    d->count--;

    if (item == d->firstItem)
    {
        d->firstItem = d->currItem = d->firstItem->d->next;
        if (d->firstItem)
            d->firstItem->d->prev = 0;
        else
            d->firstItem = d->lastItem = d->currItem = 0;
    }
    else if (item == d->lastItem)
    {
        d->lastItem = d->currItem = d->lastItem->d->prev;
        if ( d->lastItem )
           d->lastItem->d->next = 0;
        else
            d->firstItem = d->lastItem = d->currItem = 0;
    }
    else
    {
        ThumbBarItem *i = item;
        if (i)
        {
            if (i->d->prev )
            {
                i->d->prev->d->next = d->currItem = i->d->next;
            }
            if ( i->d->next )
            {
                i->d->next->d->prev = d->currItem = i->d->prev;
            }
        }
    }

    d->itemDict.remove(item->url());

    if (!d->clearing)
    {
        triggerUpdate();
    }
}

void ThumbBarView::rearrangeItems()
{
    QStringList urlList;

    int pos = 0;
    ThumbBarItem *item = d->firstItem;

    while (item)
    {
        item->d->pos = pos;
        pos += d->tileSize + 2*d->margin;
        if (!(item->d->pixmap))
            urlList.append(item->d->url);
        item = item->d->next;
    }

    if (d->orientation == Vertical)
       resizeContents(width(), d->count*(d->tileSize+2*d->margin));
    else
       resizeContents(d->count*(d->tileSize+2*d->margin), height());
}

void ThumbBarView::repaintItem(ThumbBarItem* item)
{
    if (item)
    {
       if (d->orientation == Vertical)
           repaintContents(0, item->d->pos, visibleWidth(), d->tileSize+2*d->margin);
       else
           repaintContents(item->d->pos, 0, d->tileSize+2*d->margin, visibleHeight());
    }
}

void ThumbBarView::slotUpdate()
{
    rearrangeItems();
    viewport()->update();
}

// -------------------------------------------------------------------------

ThumbBarItem::ThumbBarItem(ThumbBarView* view, const QString& url)
{
	d = new ThumbBarItemPriv;
	d->url  = url;
	d->view = view;
        d->pos = view->countItems();
	d->view->insertItem(this);

	QSize s( 200, 150 );

	//load the given url

	QFileInfo fi(url);

	d->pixmap = new QPixmap();
	d->pixmap->load(fi.absolutePath()+'/'+fi.baseName()+".png","PNG");
	repaint();
}

ThumbBarItem::~ThumbBarItem()
{
    d->view->removeItem(this);

    if (d->pixmap)
        delete d->pixmap;

    delete d;
}

int ThumbBarItem::position() const
{
    return d->pos;
}

QString ThumbBarItem::url() const
{
    return d->url;
}

ThumbBarItem* ThumbBarItem::next() const
{
    return d->next;
}

ThumbBarItem* ThumbBarItem::prev() const
{
    return d->prev;
}

QRect ThumbBarItem::rect() const
{
    if (d->view->d->orientation == ThumbBarView::Vertical)
    {
        return QRect(0, d->pos,
                     d->view->visibleWidth(),
                     d->view->d->tileSize + 2*d->view->d->margin);
    }
    else
    {
        return QRect(d->pos, 0,
                     d->view->d->tileSize + 2*d->view->d->margin,
                     d->view->visibleHeight());
    }
}

void ThumbBarItem::repaint()
{
    d->view->repaintItem(this);
}

// -------------------------------------------------------------------------

ThumbBarToolTip::ThumbBarToolTip(ThumbBarView* parent)
               : m_view(parent)
{
}

void ThumbBarToolTip::maybeTip(const QPoint& /*pos*/)
{
    /*if ( !parentWidget() || !m_view)
        return;

    ThumbBarItem* item = m_view->findItem( m_view->viewportToContents(pos) );
    if (!item)
        return;

    QRect r(item->rect());
    r = QRect( m_view->contentsToViewport(r.topLeft()), r.size() );

    QString cellBeg(QString::fromLatin1("<tr><td><nobr><font size=-1>"));
    QString cellMid(QString::fromLatin1("</font></nobr></td>"
                    "<td><nobr><font size=-1>"));
    QString cellEnd(QString::fromLatin1("</font></nobr></td></tr>"));

    QString tipText;
    tipText  = QString::fromLatin1("<table cellspacing=0 cellpadding=0>");
    tipText += cellBeg + i18n("Name:") + cellMid;
    tipText += item->url() + cellEnd;

    tipText += cellBeg + i18n("Type:") + cellMid;
    tipText += KMimeType::findByUrl(item->url())->comment() + cellEnd;

    KFileItem fileItem(KFileItem::Unknown, KFileItem::Unknown, item->url());

    QDateTime date;
    date.setTime_t(fileItem.time(KIO::UDS_MODIFICATION_TIME));
    tipText += cellBeg + i18n("Modification Date:") + cellMid +
               KGlobal::locale()->formatDateTime(date, true, true)
               + cellEnd;

    tipText += cellBeg + i18n("Size:") + cellMid;
    tipText += i18n("%1 (%2)")
               .arg(KIO::convertSize(fileItem.size()))
               .arg(KGlobal::locale()->formatNumber(fileItem.size(), 0))
               + cellEnd;

    tipText += QString::fromLatin1("</table>");

    tip(r, tipText);*/
}

#include "thumbbar.moc"
