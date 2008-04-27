#!/usr/bin/python
#
# Copyright (C) 2007-2008 Richard Hughes <richard@hughsie.com>
#
# Licensed under the GNU General Public License Version 2
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

import sys

from conaryBackend import PackageKitConaryBackend
repoid = sys.argv[1]
para = sys.argv[2]
value=sys.argv[3]
backend = PackageKitConaryBackend(sys.argv[2:])
backend.repo_set_data(repoid,para,value)
sys.exit(0)