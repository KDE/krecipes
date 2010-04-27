/* ============================================================
 * Authors: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *          Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2004-11-22
 * Description : a bar widget to display image thumbnails
 *
 * Copyright © 2004-2005 Renchi Raju <renchi@pooh.tam.uiuc.edu>
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

#ifndef THUMBBAR_H
#define THUMBBAR_H

// Qt includes.

#include <q3scrollview.h>
//Added by qt3to4:
#include <QPaintEvent>
#include <QMouseEvent>

// Local includes.


class ThumbBarItem;
class ThumbBarViewPriv;
class ThumbBarItemPriv;

class ThumbBarView : public Q3ScrollView
{
    Q_OBJECT

public:
    
    enum Orientation
    {
        Horizontal=0,      
        Vertical         
    };

public:

    explicit ThumbBarView(QWidget* parent, int orientation=Qt::Vertical);
    ~ThumbBarView();

    int  countItems();
    
    void clear(bool updateView=true);
    void triggerUpdate();

    void removeItem(ThumbBarItem* item);

    ThumbBarItem* currentItem() const;
    void setSelected(ThumbBarItem* item);

    ThumbBarItem* firstItem() const;
    ThumbBarItem* lastItem()  const;
    ThumbBarItem* findItem(const QPoint& pos) const;
    ThumbBarItem* findItemByURL(const QString& url) const;

    void invalidateThumb(ThumbBarItem* item);
        
protected:

    void viewportPaintEvent(QPaintEvent* e);
    void contentsMousePressEvent(QMouseEvent* e);

private:

    void insertItem(ThumbBarItem* item);
    void rearrangeItems();
    void repaintItem(ThumbBarItem* item);

signals:

    void signalURLSelected(const QString& url);
    void signalItemAdded(void);
    
private slots:

    void slotUpdate();

private:

    ThumbBarViewPriv* d;

    friend class ThumbBarItem;
};

// -------------------------------------------------------------------------

class ThumbBarItem
{
public:

    ThumbBarItem(ThumbBarView* view, const QString& url);
    ~ThumbBarItem();

    QString url() const;
    
    ThumbBarItem* next() const;
    ThumbBarItem* prev() const;
    int           position() const;
    QRect         rect() const;

    void          repaint();

private:

    ThumbBarItemPriv* d;

    friend class ThumbBarView;
};

// -------------------------------------------------------------------------

class ThumbBarToolTip 
{
public:

    ThumbBarToolTip(ThumbBarView* parent);

protected:
    
    void maybeTip(const QPoint& pos);

private:

    ThumbBarView* m_view;
};

#endif /* THUMBBAR_H */
