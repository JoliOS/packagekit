/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2009 Richard Hughes <richard@hughsie.com>
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

#if !defined (__PACKAGEKIT_H_INSIDE__) && !defined (PK_COMPILATION)
#error "Only <packagekit.h> can be included directly."
#endif

#ifndef __PK_SOURCE_H
#define __PK_SOURCE_H

#include <glib-object.h>

G_BEGIN_DECLS

#define PK_TYPE_SOURCE			(pk_source_get_type ())
#define PK_SOURCE(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), PK_TYPE_SOURCE, PkSource))
#define PK_SOURCE_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), PK_TYPE_SOURCE, PkSourceClass))
#define PK_IS_SOURCE(o)			(G_TYPE_CHECK_INSTANCE_TYPE ((o), PK_TYPE_SOURCE))
#define PK_IS_SOURCE_CLASS(k)		(G_TYPE_CHECK_CLASS_TYPE ((k), PK_TYPE_SOURCE))
#define PK_SOURCE_GET_CLASS(o)		(G_TYPE_INSTANCE_GET_CLASS ((o), PK_TYPE_SOURCE, PkSourceClass))

typedef struct _PkSourcePrivate		PkSourcePrivate;
typedef struct _PkSource		PkSource;
typedef struct _PkSourceClass		PkSourceClass;

struct _PkSource
{
	 GObject			 parent;
	 PkSourcePrivate		*priv;
};

struct _PkSourceClass
{
	GObjectClass	parent_class;
	/* padding for future expansion */
	void (*_pk_reserved1) (void);
	void (*_pk_reserved2) (void);
	void (*_pk_reserved3) (void);
	void (*_pk_reserved4) (void);
	void (*_pk_reserved5) (void);
};

GType		 pk_source_get_type		(void);
PkSource	*pk_source_new			(void);

G_END_DECLS

#endif /* __PK_SOURCE_H */
