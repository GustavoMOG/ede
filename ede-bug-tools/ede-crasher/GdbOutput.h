/*
 * $Id: GdbOutput.h 3435 2012-10-23 10:20:21Z karijes $
 *
 * ede-crasher, a crash handler tool
 * Part of Equinox Desktop Environment (EDE).
 * Copyright (c) 2008-2011 EDE Authors.
 *
 * This program is licensed under terms of the 
 * GNU General Public License version 2 or newer.
 * See COPYING for details.
 */

#ifndef __GDBOUTPUT_H__
#define __GDBOUTPUT_H__

bool gdb_output_generate(const char *path, edelib::TempFile &t, int pid = -1);

#endif
