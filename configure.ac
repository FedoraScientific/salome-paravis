#
#  PLEASE DO NOT MODIFY configure.in FILE
#
#  ALL CHANGES WILL BE DISCARDED BY THE NEXT
#  build_configure COMMAND
#
#  CHANGES MUST BE MADE IN configure.in.base FILE
#
#
# Author : Marc Tajchman (CEA)
# Date : 28/06/2001
# Modified by : Patrick GOLDBRONN (CEA)
# Modified by : Marc Tajchman (CEA)
#
# Created from configure.in.base
#

AC_INIT([PARAVIS - SALOME GUI for ParaView], [0.1.0], [webmaster.salome@opencascade.com], [PARAVIS])
AC_CONFIG_AUX_DIR(salome_adm/unix/config_files)
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE

XVERSION=`echo $VERSION | awk -F. '{printf("0x%02x%02x%02x",$1,$2,$3)}'`
AC_SUBST(XVERSION)

# set up MODULE_NAME variable for dynamic construction of directories (resources, etc.)
MODULE_NAME=paravis
AC_SUBST(MODULE_NAME)

dnl
dnl Initialize source and build root directories
dnl

ROOT_BUILDDIR=`pwd`
ROOT_SRCDIR=`echo $0 | sed -e "s,[[^/]]*$,,;s,/$,,;s,^$,.,"`
cd $ROOT_SRCDIR
ROOT_SRCDIR=`pwd`
cd $ROOT_BUILDDIR

AC_SUBST(ROOT_SRCDIR)
AC_SUBST(ROOT_BUILDDIR)

echo
echo Source root directory : $ROOT_SRCDIR
echo Build  root directory : $ROOT_BUILDDIR
echo
echo

AC_CHECK_PROG(SHELL,sh)
AC_SUBST(SHELL)

if test -z "$AR"; then
   AC_CHECK_PROGS(AR,ar xar,:,$PATH)
fi
AC_SUBST(AR)

dnl Export the AR macro so that it will be placed in the libtool file
dnl correctly.
export AR

echo
echo ---------------------------------------------
echo testing make
echo ---------------------------------------------
echo

AC_PROG_MAKE_SET
AC_PROG_INSTALL
dnl 
dnl libtool macro check for CC, LD, NM, LN_S, RANLIB, STRIP + pour les librairies dynamiques !

AC_ENABLE_DEBUG(yes)
AC_DISABLE_PRODUCTION

echo ---------------------------------------------
echo testing libtool
echo ---------------------------------------------

dnl first, we set static to no!
dnl if we want it, use --enable-static
AC_ENABLE_STATIC(no)

AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL

dnl Fix up the INSTALL macro if it s a relative path. We want the
dnl full-path to the binary instead.
case "$INSTALL" in
   *install-sh*)
      INSTALL="\${KERNEL_ROOT_DIR}/salome_adm/unix/config_files/install-sh -c"
      ;;
esac

echo
echo ---------------------------------------------
echo testing C/C++
echo ---------------------------------------------
echo

cc_ok=no
dnl inutil car libtool
dnl AC_PROG_CC
AC_PROG_CXX
AC_CXX_WARNINGS
AC_CXX_TEMPLATE_OPTIONS
AC_DEPEND_FLAG
# AC_CC_WARNINGS([ansi])
cc_ok=yes

dnl Library libdl :
AC_CHECK_LIB(dl,dlopen)

dnl add library libm :
AC_CHECK_LIB(m,ceil)

dnl Library librt : for alpha/osf
AC_CHECK_LIB(rt,nanosleep)

dnl 
dnl Check if we use std iostream by default or if we must add
dnl a compiler directive for that
dnl

AC_CXX_USE_STD_IOSTREAM

dnl 
dnl Well we use sstream which is not in gcc pre-2.95.3
dnl We must test if it exists. If not, add it in include !
dnl

AC_CXX_HAVE_SSTREAM

dnl
dnl ---------------------------------------------
dnl testing linker
dnl ---------------------------------------------
dnl

AC_LINKER_OPTIONS

dnl
dnl ---------------------------------------------
dnl testing MPICH
dnl ---------------------------------------------
dnl

CHECK_MPI
CHECK_MPICH

dnl
dnl ---------------------------------------------
dnl testing WITHIHM
dnl ---------------------------------------------
dnl

CHECK_WITHIHM

echo
echo ---------------------------------------------
echo testing LEX \& YACC
echo ---------------------------------------------
echo

lex_yacc_ok=no
AC_PROG_YACC
AC_PROG_LEX
lex_yacc_ok=yes

echo
echo ---------------------------------------------
echo testing python
echo ---------------------------------------------
echo

CHECK_PYTHON

AM_PATH_PYTHON(2.3)

dnl echo
dnl echo ---------------------------------------------
dnl echo testing java
dnl echo ---------------------------------------------
dnl echo

dnl CHECK_JAVA

#echo
#echo ---------------------------------------------
#echo testing swig
#echo ---------------------------------------------
#echo
#
#CHECK_SWIG

echo
echo ---------------------------------------------
echo testing threads
echo ---------------------------------------------
echo

ENABLE_PTHREADS

echo
echo ---------------------------------------------
echo testing openGL
echo ---------------------------------------------
echo

CHECK_OPENGL

echo
echo ---------------------------------------------
echo testing QT
echo ---------------------------------------------
echo

CHECK_QT

echo
echo ---------------------------------------------
echo testing HDF5
echo ---------------------------------------------
echo

CHECK_HDF5

echo
echo ---------------------------------------------
echo testing ParaView
echo ---------------------------------------------
echo

CHECK_PARAVIEW

echo
echo ---------------------------------------------
echo Testing OpenCascade
echo ---------------------------------------------
echo

CHECK_CAS

echo
echo ---------------------------------------------
echo Testing html generators
echo ---------------------------------------------
echo

CHECK_HTML_GENERATORS

echo
echo ---------------------------------------------
echo Testing Kernel
echo ---------------------------------------------
echo

CHECK_KERNEL

echo
echo ---------------------------------------------
echo Testing GUI
echo ---------------------------------------------
echo

CHECK_SALOME_GUI

echo
echo ---------------------------------------------
echo Summary
echo ---------------------------------------------
echo

echo Configure
variables="cc_ok lex_yacc_ok python_ok threads_ok OpenGL_ok qt_ok hdf5_ok paraview_ok occ_ok doxygen_ok graphviz_ok Kernel_ok SalomeGUI_ok"

for var in $variables
do
   printf "   %10s : " `echo \$var | sed -e "s,_ok,,"`
   eval echo \$$var
done

dnl We don t need to say when we re entering directories if we re using
dnl GNU make becuase make does it for us.
if test "X$GMAKE" = "Xyes"; then
   AC_SUBST(SETX) SETX=":"
else
   AC_SUBST(SETX) SETX="set -x"
fi
echo
echo ---------------------------------------------
echo generating Makefiles and configure files
echo ---------------------------------------------
echo

AC_OUTPUT_COMMANDS([ \
      chmod +x ./bin/*; \
])

# This list is initiated using autoscan and must be updated manually
# when adding a new file <filename>.in to manage. When you execute
# autoscan, the Makefile list is generated in the output file configure.scan.
# This could be helpfull to update de configuration.
AC_OUTPUT([ \
  ./salome_adm/unix/SALOMEconfig.h \
  ./adm_local/Makefile \
  ./adm_local/unix/Makefile \
  ./adm_local/unix/config_files/Makefile \
  ./bin/VERSION \
  ./bin/Makefile \
  ./doc/Makefile \
  ./doc/salome/Makefile \
  ./doc/salome/gui/Makefile \
  ./doc/salome/gui/doxyfile \
  ./src/Makefile \
  ./src/PVGUI/Makefile \
  ./resources/Makefile \
  ./PARAVIS_version.h \
  Makefile \
])
