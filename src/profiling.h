/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef PROFILING_H
#define PROFILING_H

//#include <config-krecipes.h>

#define KRECIPES_PROFILING

#ifdef KRECIPES_PROFILING
    #include <kdebug.h>
  static QTime dbg_timer;

  #define START_TIMER(MSG) \
   dbg_timer.start(); kDebug()<<MSG;
  #define END_TIMER() \
   kDebug()<<"...took "<<dbg_timer.elapsed()<<" ms";
#else
  #define START_TIMER(MSG)
  #define END_TIMER()
#endif

#endif //PROFILING_H
