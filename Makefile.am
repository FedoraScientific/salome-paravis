#  Copyright (C) 2005  CEA/DEN, EDF R&D, OPEN CASCADE, PRINCIPIA R&D
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
# -* Makefile *-
#
# Author :
# Modified by : Alexander BORODIN (OCN) - autotools usage
#

include $(top_srcdir)/adm_local/unix/make_common_starter.am

ACLOCAL_AMFLAGS = -I adm_local/unix/config_files \
                  -I ${KERNEL_ROOT_DIR}/salome_adm/unix/config_files \
                  -I ${GUI_ROOT_DIR}/adm_local/unix/config_files

SUBDIRS = adm_local resources src bin 

DISTCLEANFILES = a.out aclocal.m4 configure

salomeinclude_DATA = PARAVIS_version.h

EXTRA_DIST += \
	build_configure \
	clean_configure

dist-hook:
	rm -rf `find $(distdir) -name CVS`
