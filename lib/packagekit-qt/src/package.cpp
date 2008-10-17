/*
 * This file is part of the QPackageKit project
 * Copyright (C) 2008 Adrien Bustany <madcat@mymadcat.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB. If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "package.h"
#include "util.h"

using namespace PackageKit;

////// Details class
class Package::Details::Private
{
public:
	Package* package;
	Package::License license;
	Client::Group group;
	QString description;
	QString url;
	uint size;
};

Package::Details::Details(Package* p, const QString& license, const QString& group, const QString& detail, const QString& url, qulonglong size) : d(new Private)
{
	d->package = p;

	int licenseValue = Util::enumFromString<Package>(license, "License");
	if(licenseValue == -1)
		d->license = UnknownLicense;
	else
		d->license = (License)licenseValue;

	int groupValue = Util::enumFromString<Client>(group, "Group");
	if(groupValue == -1)
		d->group = Client::UnknownGroup;
	else
		d->group = (Client::Group)groupValue;

	d->description = detail;
	d->url = url;
	d->size = size;
}

Package::Details::~Details()
{
	delete d;
}

Package* Package::Details::package()
{
	return d->package;
}

Package::License Package::Details::license()
{
	return d->license;
}

Client::Group Package::Details::group()
{
	return d->group;
}

QString Package::Details::description()
{
	return d->description;
}

QString Package::Details::url()
{
	return d->url;
}

qulonglong Package::Details::size()
{
	return d->size;
}

////// Package class

class Package::Private
{
public:
	QString id;
	QString name;
	QString version;
	QString arch;
	QString data;
	QString summary;
	Package::State state;
	Package::Details* details;
};

Package::Package(const QString& packageId, const QString& state, const QString& summary) : QObject(NULL), d(new Private)
{
	d->id = packageId;

	// Break down the packageId
	QStringList tokens = packageId.split(";");
	if(tokens.size() == 4) {
		d->name = tokens.at(0);
		d->version = tokens.at(1);
		d->arch = tokens.at(2);
		d->data = tokens.at(3);
	}

	int stateValue = Util::enumFromString<Package>(state, "State");
	if(stateValue == -1)
		d->state = UnknownState;
	else
		d->state = (State)stateValue;

	d->summary = summary;

	d->details = NULL;
}

Package::~Package()
{
	if(hasDetails())
		delete d->details;
}

QString Package::id()
{
	return d->id;
}

QString Package::name()
{
	return d->name;
}

QString Package::version()
{
	return d->version;
}

QString Package::arch()
{
	return d->arch;
}

QString Package::data()
{
	return d->data;
}

QString Package::summary()
{
	return d->summary;
}

Package::State Package::state()
{
	return d->state;
}

bool Package::hasDetails()
{
	return (d->details != NULL);
}

Package::Details* Package::details()
{
	return d->details;
}

void Package::setDetails(Package::Details* det)
{
	d->details = det;
}

#include "package.moc"