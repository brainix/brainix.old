/*----------------------------------------------------------------------------*\
 |	main.c								      |
 |									      |
 |	Copyright © 2002-2007, Team Brainix, original authors.		      |
 |		All rights reserved.					      |
\*----------------------------------------------------------------------------*/

/*
 | This program is Free Software; you can redistribute it and/or modify it under
 | the terms of the GNU General Public License as published by the Free Software
 | Foundation; either version 2 of the License, or (at your option) any later
 | version.
 |
 | This program is distributed in the hope that it will be useful, but WITHOUT
 | ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 | FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 | details.
 |
 | You should have received a copy of the GNU General Public License along with
 | this program; if not, write to:
 |
 |	The Free Software Foundation, Inc.
 |	59 Temple Place, Suite 330
 |	Boston MA 02111-1307
 */

#include <fs/fs.h>
#include <fs/op_handler.h>

void leaky_free(char ***p);

/*----------------------------------------------------------------------------*\
 |				   fs_main()				      |
\*----------------------------------------------------------------------------*/
void fs_main(void)
{
	/* Initialize the file system. */
	block_init(); /* Initialize the block cache.                      */
	inode_init(); /* Initialize the inode table.                      */
	super_init(); /* Initialize the superblock table.                 */
	dev_init();   /* Initialize the device driver PID table.          */
	descr_init(); /* Init the file ptr and proc-specific info tables. */

	/* Wait for a message. */
	while ((msg = msg_receive(ANYONE))->op != SHUTDOWN)
	{
		super_user = proc_info(msg->from, EUID) == 0;
		err_code = 0;

		/* Do the requested work. */
		op_handler_t *fs_handler = 
		{
			&msg,
			fs_run 
		};
//		(*fs_handler->fs_run)(&msg);
		op_main(fs_handler);
	}

	/* Deinitialize the file system. */
	umount_root();
	dev_deinit();
	msg_reply(msg);
	while (true)
		msg_free(msg_receive(ANYONE));
}
