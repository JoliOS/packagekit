/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2008-2009 Richard Hughes <richard@hughsie.com>
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

#include "config.h"

#include <stdio.h>
#include <gio/gio.h>
#include <glib/gi18n.h>
#include <packagekit-glib2/packagekit.h>

#include "egg-debug.h"

#include "pk-client-sync.h"
#include "pk-console-shared.h"

/**
 * pk_console_get_number:
 **/
guint
pk_console_get_number (const gchar *question, guint maxnum)
{
	gint answer = 0;
	gint retval;

	/* pretty print */
	g_print ("%s", question);

	do {
		/* get a number */
		retval = scanf("%u", &answer);

		/* positive */
		if (retval == 1 && answer > 0 && answer <= (gint) maxnum)
			break;
		g_print (_("Please enter a number from 1 to %i: "), maxnum);
	} while (TRUE);
	return answer;
}

/**
 * pk_console_get_prompt:
 **/
gboolean
pk_console_get_prompt (const gchar *question, gboolean defaultyes)
{
	gchar answer = '\0';
	gboolean ret = FALSE;

	/* pretty print */
	g_print ("%s", question);
	if (defaultyes)
		g_print (" [Y/n] ");
	else
		g_print (" [N/y] ");

	do {
		/* ITS4: ignore, we are copying into the same variable, not a string */
		answer = (gchar) fgetc (stdin);

		/* positive */
		if (answer == 'y' || answer == 'Y') {
			ret = TRUE;
			break;
		}
		/* negative */
		if (answer == 'n' || answer == 'N')
			break;

		/* default choice */
		if (answer == '\n' && defaultyes) {
			ret = TRUE;
			break;
		}
		if (answer == '\n' && !defaultyes)
			break;
	} while (TRUE);

	/* remove the trailing \n */
	answer = (gchar) fgetc (stdin);
	if (answer != '\n')
		ungetc (answer, stdin);

	return ret;
}

/**
 * pk_console_resolve_package:
 **/
gchar *
pk_console_resolve_package (PkClient *client, PkBitfield filter, const gchar *package, GError **error)
{
	gchar *package_id = NULL;
	gboolean valid;
	gchar **tmp;
	PkResults *results;
	GPtrArray *array = NULL;
	guint i;
	gchar *printable;
	const PkItemPackage *item;

	/* have we passed a complete package_id? */
	valid = pk_package_id_check (package);
	if (valid)
		return g_strdup (package);

	/* split */
	tmp = g_strsplit (package, ",", -1);

	/* get the list of possibles */
	results = pk_client_resolve_sync (client, filter, tmp, NULL, NULL, NULL, error);
	if (results == NULL)
		goto out;

	/* get the packages returned */
	array = pk_results_get_package_array (results);
	if (array == NULL) {
		*error = g_error_new (1, 0, "did not get package struct for %s", package);
		goto out;
	}

	/* nothing found */
	if (array->len == 0) {
		*error = g_error_new (1, 0, "could not find %s", package);
		goto out;
	}

	/* just one thing found */
	if (array->len == 1) {
		item = g_ptr_array_index (array, 0);
		package_id = g_strdup (item->package_id);
		goto out;
	}

	/* TRANSLATORS: more than one package could be found that matched, to follow is a list of possible packages  */
	g_print ("%s\n", _("More than one package matches:"));
	for (i=0; i<array->len; i++) {
		item = g_ptr_array_index (array, i);
		printable = pk_package_id_to_printable (item->package_id);
		g_print ("%i. %s\n", i+1, printable);
		g_free (printable);
	}

	/* TRANSLATORS: This finds out which package in the list to use */
	i = pk_console_get_number (_("Please choose the correct package: "), array->len);
	item = g_ptr_array_index (array, i-1);
	package_id = g_strdup (item->package_id);
out:
	if (results != NULL)
		g_object_unref (results);
	if (array != NULL)
		g_ptr_array_unref (array);
	g_strfreev (tmp);
	return package_id;
}

/**
 * pk_console_resolve_packages:
 **/
gchar **
pk_console_resolve_packages (PkClient *client, PkBitfield filter, gchar **packages, GError **error)
{
	gchar **package_ids;
	guint i;
	guint len;

	/* get length */
	len = g_strv_length (packages);
	egg_debug ("resolving %i packages", len);

	/* create output array*/
	package_ids = g_new0 (gchar *, len+1);

	/* resolve each package */
	for (i=0; i<len; i++) {
		package_ids[i] = pk_console_resolve_package (client, filter, packages[i], error);
		if (package_ids[i] == NULL) {
			/* destroy state */
			g_strfreev (package_ids);
			package_ids = NULL;
			break;
		}
	}
	return package_ids;
}

/**
 * pk_status_enum_to_localised_text:
 **/
const gchar *
pk_status_enum_to_localised_text (PkStatusEnum status)
{
	const gchar *text = NULL;
	switch (status) {
	case PK_STATUS_ENUM_UNKNOWN:
		/* TRANSLATORS: This is when the transaction status is not known */
		text = _("Unknown state");
		break;
	case PK_STATUS_ENUM_SETUP:
		/* TRANSLATORS: transaction state, the daemon is in the process of starting */
		text = _("Starting");
		break;
	case PK_STATUS_ENUM_WAIT:
		/* TRANSLATORS: transaction state, the transaction is waiting for another to complete */
		text = _("Waiting in queue");
		break;
	case PK_STATUS_ENUM_RUNNING:
		/* TRANSLATORS: transaction state, just started */
		text = _("Running");
		break;
	case PK_STATUS_ENUM_QUERY:
		/* TRANSLATORS: transaction state, is querying data */
		text = _("Querying");
		break;
	case PK_STATUS_ENUM_INFO:
		/* TRANSLATORS: transaction state, getting data from a server */
		text = _("Getting information");
		break;
	case PK_STATUS_ENUM_REMOVE:
		/* TRANSLATORS: transaction state, removing packages */
		text = _("Removing packages");
		break;
	case PK_STATUS_ENUM_DOWNLOAD:
		/* TRANSLATORS: transaction state, downloading package files */
		text = _("Downloading packages");
		break;
	case PK_STATUS_ENUM_INSTALL:
		/* TRANSLATORS: transaction state, installing packages */
		text = _("Installing packages");
		break;
	case PK_STATUS_ENUM_REFRESH_CACHE:
		/* TRANSLATORS: transaction state, refreshing internal lists */
		text = _("Refreshing software list");
		break;
	case PK_STATUS_ENUM_UPDATE:
		/* TRANSLATORS: transaction state, installing updates */
		text = _("Installing updates");
		break;
	case PK_STATUS_ENUM_CLEANUP:
		/* TRANSLATORS: transaction state, removing old packages, and cleaning config files */
		text = _("Cleaning up packages");
		break;
	case PK_STATUS_ENUM_OBSOLETE:
		/* TRANSLATORS: transaction state, obsoleting old packages */
		text = _("Obsoleting packages");
		break;
	case PK_STATUS_ENUM_DEP_RESOLVE:
		/* TRANSLATORS: transaction state, checking the transaction before we do it */
		text = _("Resolving dependencies");
		break;
	case PK_STATUS_ENUM_SIG_CHECK:
		/* TRANSLATORS: transaction state, checking if we have all the security keys for the operation */
		text = _("Checking signatures");
		break;
	case PK_STATUS_ENUM_ROLLBACK:
		/* TRANSLATORS: transaction state, when we return to a previous system state */
		text = _("Rolling back");
		break;
	case PK_STATUS_ENUM_TEST_COMMIT:
		/* TRANSLATORS: transaction state, when we're doing a test transaction */
		text = _("Testing changes");
		break;
	case PK_STATUS_ENUM_COMMIT:
		/* TRANSLATORS: transaction state, when we're writing to the system package database */
		text = _("Committing changes");
		break;
	case PK_STATUS_ENUM_REQUEST:
		/* TRANSLATORS: transaction state, requesting data from a server */
		text = _("Requesting data");
		break;
	case PK_STATUS_ENUM_FINISHED:
		/* TRANSLATORS: transaction state, all done! */
		text = _("Finished");
		break;
	case PK_STATUS_ENUM_CANCEL:
		/* TRANSLATORS: transaction state, in the process of cancelling */
		text = _("Cancelling");
		break;
	case PK_STATUS_ENUM_DOWNLOAD_REPOSITORY:
		/* TRANSLATORS: transaction state, downloading metadata */
		text = _("Downloading repository information");
		break;
	case PK_STATUS_ENUM_DOWNLOAD_PACKAGELIST:
		/* TRANSLATORS: transaction state, downloading metadata */
		text = _("Downloading list of packages");
		break;
	case PK_STATUS_ENUM_DOWNLOAD_FILELIST:
		/* TRANSLATORS: transaction state, downloading metadata */
		text = _("Downloading file lists");
		break;
	case PK_STATUS_ENUM_DOWNLOAD_CHANGELOG:
		/* TRANSLATORS: transaction state, downloading metadata */
		text = _("Downloading lists of changes");
		break;
	case PK_STATUS_ENUM_DOWNLOAD_GROUP:
		/* TRANSLATORS: transaction state, downloading metadata */
		text = _("Downloading groups");
		break;
	case PK_STATUS_ENUM_DOWNLOAD_UPDATEINFO:
		/* TRANSLATORS: transaction state, downloading metadata */
		text = _("Downloading update information");
		break;
	case PK_STATUS_ENUM_REPACKAGING:
		/* TRANSLATORS: transaction state, repackaging delta files */
		text = _("Repackaging files");
		break;
	case PK_STATUS_ENUM_LOADING_CACHE:
		/* TRANSLATORS: transaction state, loading databases */
		text = _("Loading cache");
		break;
	case PK_STATUS_ENUM_SCAN_APPLICATIONS:
		/* TRANSLATORS: transaction state, scanning for running processes */
		text = _("Scanning applications");
		break;
	case PK_STATUS_ENUM_GENERATE_PACKAGE_LIST:
		/* TRANSLATORS: transaction state, generating a list of packages installed on the system */
		text = _("Generating package lists");
		break;
	case PK_STATUS_ENUM_WAITING_FOR_LOCK:
		/* TRANSLATORS: transaction state, when we're waiting for the native tools to exit */
		text = _("Waiting for package manager lock");
		break;
	case PK_STATUS_ENUM_WAITING_FOR_AUTH:
		/* TRANSLATORS: waiting for user to type in a password */
		text = _("Waiting for authentication");
		break;
	case PK_STATUS_ENUM_SCAN_PROCESS_LIST:
		/* TRANSLATORS: we are updating the list of processes */
		text = _("Updating running applications");
		break;
	case PK_STATUS_ENUM_CHECK_EXECUTABLE_FILES:
		/* TRANSLATORS: we are checking executable files currently in use */
		text = _("Checking applications in use");
		break;
	case PK_STATUS_ENUM_CHECK_LIBRARIES:
		/* TRANSLATORS: we are checking for libraries currently in use */
		text = _("Checking libraries in use");
		break;
	default:
		egg_warning ("status unrecognised: %s", pk_status_enum_to_text (status));
	}
	return text;
}

/**
 * pk_info_enum_to_localised_text:
 **/
static const gchar *
pk_info_enum_to_localised_text (PkInfoEnum info)
{
	const gchar *text = NULL;
	switch (info) {
	case PK_INFO_ENUM_LOW:
		/* TRANSLATORS: The type of update */
		text = _("Trivial");
		break;
	case PK_INFO_ENUM_NORMAL:
		/* TRANSLATORS: The type of update */
		text = _("Normal");
		break;
	case PK_INFO_ENUM_IMPORTANT:
		/* TRANSLATORS: The type of update */
		text = _("Important");
		break;
	case PK_INFO_ENUM_SECURITY:
		/* TRANSLATORS: The type of update */
		text = _("Security");
		break;
	case PK_INFO_ENUM_BUGFIX:
		/* TRANSLATORS: The type of update */
		text = _("Bug fix ");
		break;
	case PK_INFO_ENUM_ENHANCEMENT:
		/* TRANSLATORS: The type of update */
		text = _("Enhancement");
		break;
	case PK_INFO_ENUM_BLOCKED:
		/* TRANSLATORS: The type of update */
		text = _("Blocked");
		break;
	case PK_INFO_ENUM_INSTALLED:
	case PK_INFO_ENUM_COLLECTION_INSTALLED:
		/* TRANSLATORS: The state of a package */
		text = _("Installed");
		break;
	case PK_INFO_ENUM_AVAILABLE:
	case PK_INFO_ENUM_COLLECTION_AVAILABLE:
		/* TRANSLATORS: The state of a package, i.e. not installed */
		text = _("Available");
		break;
	default:
		egg_warning ("info unrecognised: %s", pk_info_enum_to_text (info));
	}
	return text;
}

/**
 * pk_info_enum_to_localised_present:
 **/
const gchar *
pk_info_enum_to_localised_present (PkInfoEnum info)
{
	const gchar *text = NULL;
	switch (info) {
	case PK_INFO_ENUM_DOWNLOADING:
		/* TRANSLATORS: The action of the package, in present tense */
		text = _("Downloading");
		break;
	case PK_INFO_ENUM_UPDATING:
		/* TRANSLATORS: The action of the package, in present tense */
		text = _("Updating");
		break;
	case PK_INFO_ENUM_INSTALLING:
		/* TRANSLATORS: The action of the package, in present tense */
		text = _("Installing");
		break;
	case PK_INFO_ENUM_REMOVING:
		/* TRANSLATORS: The action of the package, in present tense */
		text = _("Removing");
		break;
	case PK_INFO_ENUM_CLEANUP:
		/* TRANSLATORS: The action of the package, in present tense */
		text = _("Cleaning up");
		break;
	case PK_INFO_ENUM_OBSOLETING:
		/* TRANSLATORS: The action of the package, in present tense */
		text = _("Obsoleting");
		break;
	case PK_INFO_ENUM_REINSTALLING:
		/* TRANSLATORS: The action of the package, in present tense */
		text = _("Reinstalling");
		break;
	default:
		text = pk_info_enum_to_localised_text (info);
	}
	return text;
}

/**
 * pk_info_enum_to_localised_past:
 **/
const gchar *
pk_info_enum_to_localised_past (PkInfoEnum info)
{
	const gchar *text = NULL;
	switch (info) {
	case PK_INFO_ENUM_DOWNLOADING:
		/* TRANSLATORS: The action of the package, in past tense */
		text = _("Downloaded");
		break;
	case PK_INFO_ENUM_UPDATING:
		/* TRANSLATORS: The action of the package, in past tense */
		text = _("Updated");
		break;
	case PK_INFO_ENUM_INSTALLING:
		/* TRANSLATORS: The action of the package, in past tense */
		text = _("Installed");
		break;
	case PK_INFO_ENUM_REMOVING:
		/* TRANSLATORS: The action of the package, in past tense */
		text = _("Removed");
		break;
	case PK_INFO_ENUM_CLEANUP:
		/* TRANSLATORS: The action of the package, in past tense */
		text = _("Cleaned up");
		break;
	case PK_INFO_ENUM_OBSOLETING:
		/* TRANSLATORS: The action of the package, in past tense */
		text = _("Obsoleted");
		break;
	case PK_INFO_ENUM_REINSTALLING:
		/* TRANSLATORS: The action of the package, in past tense */
		text = _("Reinstalled");
		break;
	default:
		text = pk_info_enum_to_localised_text (info);
	}
	return text;
}
