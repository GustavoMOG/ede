/*
 * $Id: Logout.h 2462 2009-02-17 12:24:55Z karijes $
 *
 * Evoke, head honcho of everything
 * Part of Equinox Desktop Environment (EDE).
 * Copyright (c) 2008-2009 EDE Authors.
 *
 * This program is licensed under terms of the 
 * GNU General Public License version 2 or newer.
 * See COPYING for details.
 */

#ifndef __LOGOUT_H__
#define __LOGOUT_H__

#define LOGOUT_RET_CANCEL  -1 
#define LOGOUT_RET_LOGOUT   0
#define LOGOUT_RET_RESTART  1
#define LOGOUT_RET_SHUTDOWN 2

#define LOGOUT_OPT_RESTART  (1 << 1)
#define LOGOUT_OPT_SHUTDOWN (1 << 2)

int logout_dialog_show(int screen_w, int screen_h, int opt);

#endif
