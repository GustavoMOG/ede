/*
 * $Id: BugzillaSender.h 2715 2009-06-25 14:07:41Z karijes $
 *
 * ede-bug-report, a tool to report bugs on EDE bugzilla instance
 * Part of Equinox Desktop Environment (EDE).
 * Copyright (c) 2009 EDE Authors.
 *
 * This program is licensed under terms of the 
 * GNU General Public License version 2 or newer.
 * See COPYING for details.
 */

#ifndef __BUGZILLASENDER_H__
#define __BUGZILLASENDER_H__

bool bugzilla_send_with_progress(const char *title, const char *content);

#endif
