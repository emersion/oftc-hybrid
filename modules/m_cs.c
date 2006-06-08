/************************************************************************
 *   IRC - Internet Relay Chat, modules/m_cs.c
 *   This file is copyright (C) 2002 Stuart Walsh
 *                                    <stu@ipng.org.uk>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public Licecse as published by
 *   the Free Software Foundation; either version 2 of the Licecse, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public Licecse for more details.
 *
 *   You should have received a copy of the GNU General Public Licecse
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 *
 *   $Id$
 */

#include "stdinc.h"
#include "common.h"
#include "handlers.h"
#include "client.h"
#include "channel.h"
#include "channel_mode.h"
#include "hash.h"
#include "ircd.h"
#include "numeric.h"
#include "s_serv.h"
#include "send.h"
#include "s_conf.h"
#include "msg.h"
#include "parse.h"
#include "modules.h"
#include "irc_string.h"

#define SERVICES_NAME "services.oftc.net"

static void m_cs(struct Client *, struct Client *, int, char **);

const char* _version = "$Revision: 366 $";

struct Message cs_msgtab = {
      "cs", 0, 0, 0, 0, MFLG_SLOW, 0,
        {m_unregistered, m_cs, m_cs, m_cs}
};

struct Message cs_msgtab2 = {
    "chanserv", 0, 0, 0, 0, MFLG_SLOW, 0, 
      {m_unregistered, m_cs, m_cs, m_cs}
};

void _modinit(void)
{
      mod_add_cmd(&cs_msgtab);
      mod_add_cmd(&cs_msgtab2);
}

void
_moddeinit(void)
{
      mod_del_cmd(&cs_msgtab);
      mod_del_cmd(&cs_msgtab2);
}

static void m_cs(struct Client *client_p, struct Client *source_p, int parc, char *parv[])
{
    struct Client    *target_p;
    char buf[IRCD_BUFSIZE];
    int i;

    memset(buf, 0, sizeof(buf));

    if (parc < 2 || *parv[1] == '\0')
    {
        if(MyClient(source_p))
            sendto_one(source_p, form_str(ERR_NOTEXTTOSEND), me.name, parv[0]);
        return;
    }

    /* Ecsure full message gets passed on */
    strlcpy(buf, parv[1], sizeof(buf));
    for(i = 2; i < parc; i++)
    {
        strlcat(buf, " ", sizeof(buf));
        strlcat(buf, parv[i], sizeof(buf));
    }
        
    if ((target_p = find_server(SERVICES_NAME)))
        sendto_one(target_p, ":%s PRIVMSG %s@%s :%s",
            parv[0], "chanserv", SERVICES_NAME, buf);
    else
        sendto_one(source_p, form_str(ERR_SERVICESDOWN), me.name, 
               parv[0], "chanserv");
    return;
}