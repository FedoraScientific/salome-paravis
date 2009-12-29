dnl  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
dnl
dnl  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
dnl  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
dnl
dnl  This library is free software; you can redistribute it and/or
dnl  modify it under the terms of the GNU Lesser General Public
dnl  License as published by the Free Software Foundation; either
dnl  version 2.1 of the License.
dnl
dnl  This library is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl  Lesser General Public License for more details.
dnl
dnl  You should have received a copy of the GNU Lesser General Public
dnl  License along with this library; if not, write to the Free Software
dnl  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
dnl
dnl  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
dnl
###########################################################
# File    : check_Visu.m4
# Author  : Vadim SANDLER (OCN)
# Created : 13/07/05
# Check availability of VISU binary distribution
###########################################################

AC_DEFUN([CHECK_VISU],[

AC_CHECKING(for VISU)

Visu_ok=no

VISU_LDFLAGS=""
VISU_CXXFLAGS=""


AC_ARG_WITH(visu,
	    [  --with-visu=DIR root directory path of VISU module installation ],
	    VISU_DIR="$withval",VISU_DIR="")

if test "x$VISU_DIR" == "x" ; then
   # no --with-visu-dir option used
   if test "x$VISU_ROOT_DIR" != "x" ; then
      # VISU_ROOT_DIR environment variable defined
      VISU_DIR=$VISU_ROOT_DIR
   fi
fi

if test -f ${VISU_DIR}/idl/salome/VISU_Gen.idl ; then
   Visu_ok=yes
   AC_MSG_RESULT(Using VISU module distribution in ${VISU_DIR})

   if test "x$VISU_ROOT_DIR" == "x" ; then
      VISU_ROOT_DIR=${VISU_DIR}
   fi
   AC_SUBST(VISU_ROOT_DIR)

   VISU_LDFLAGS=-L${VISU_DIR}/lib${LIB_LOCATION_SUFFIX}/salome
   VISU_CXXFLAGS=-I${VISU_DIR}/include/salome

   AC_SUBST(VISU_LDFLAGS)
   AC_SUBST(VISU_CXXFLAGS)

else
   AC_MSG_WARN("Cannot find VISU module sources")
fi
  
AC_MSG_RESULT(for VISU: $Visu_ok)
 
])dnl
 
