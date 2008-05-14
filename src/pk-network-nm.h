/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2007-2008 Richard Hughes <richard@hughsie.com>
 *
 * Licensed under the GNU General Public License Version 2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:pk-network-nm
 * @short_description: An NetworkManager access GObject
 *
 * This allows a switchable network backend.
 */

#ifndef __PK_NETWORK_NM_H
#define __PK_NETWORK_NM_H

#include <glib-object.h>
#include <pk-enum.h>

G_BEGIN_DECLS

#define PK_TYPE_NETWORK_NM		(pk_network_nm_get_type ())
#define PK_NETWORK_NM(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), PK_TYPE_NETWORK_NM, PkNetworkNm))
#define PK_NETWORK_NM_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), PK_TYPE_NETWORK_NM, PkNetworkNmClass))
#define PK_IS_NETWORK_NM(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), PK_TYPE_NETWORK_NM))
#define PK_IS_NETWORK_NM_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), PK_TYPE_NETWORK_NM))
#define PK_NETWORK_NM_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), PK_TYPE_NETWORK_NM, PkNetworkNmClass))
#define PK_NETWORK_NM_ERROR		(pk_network_nm_error_quark ())
#define PK_NETWORK_NM_TYPE_ERROR	(pk_network_nm_error_get_type ()) 

typedef struct _PkNetworkNmPrivate	PkNetworkNmPrivate;
typedef struct _PkNetworkNm		PkNetworkNm;
typedef struct _PkNetworkNmClass	PkNetworkNmClass;

struct _PkNetworkNm
{
	 GObject		 parent;
	 PkNetworkNmPrivate	*priv;
};

struct _PkNetworkNmClass
{
	GObjectClass	parent_class;
};

GType		 pk_network_nm_get_type		  	(void) G_GNUC_CONST;
PkNetworkNm	*pk_network_nm_new			(void);
PkNetworkEnum	 pk_network_nm_get_network_state	(PkNetworkNm	*network_nm);

G_END_DECLS

#endif /* __PK_NETWORK_NM_H */
