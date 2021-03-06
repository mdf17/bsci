#############################################################################
# Makefile for building: BSCI
# Generated by qmake (3.0) (Qt 5.6.1)
# Project:  BSCI.pro
# Template: subdirs
# Command: /usr/lib64/qt5/bin/qmake -o Makefile BSCI.pro
#############################################################################

MAKEFILE      = Makefile

first: make_first
QMAKE         = /usr/lib64/qt5/bin/qmake
DEL_FILE      = rm -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p
COPY          = cp -f
COPY_FILE     = cp -f
COPY_DIR      = cp -f -R
INSTALL_FILE  = install -m 644 -p
INSTALL_PROGRAM = install -m 755 -p
INSTALL_DIR   = cp -f -R
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
TAR           = tar -cf
COMPRESS      = gzip -9f
DISTNAME      = BSCI1.0.0
DISTDIR = /localhome/MITLL/ma26343/workspace/local/bsci/.tmp/BSCI1.0.0
SUBTARGETS    =  \
		sub-ModuleA \
		sub-ModuleB


sub-ModuleA-qmake_all:  FORCE
	@test -d ModuleA/ || mkdir -p ModuleA/
	cd ModuleA/ && $(QMAKE) /localhome/MITLL/ma26343/workspace/local/bsci/ModuleA/ModuleA.pro -o Makefile
	cd ModuleA/ && $(MAKE) -f Makefile qmake_all
sub-ModuleA: FORCE
	@test -d ModuleA/ || mkdir -p ModuleA/
	cd ModuleA/ && ( test -e Makefile || $(QMAKE) /localhome/MITLL/ma26343/workspace/local/bsci/ModuleA/ModuleA.pro -o Makefile ) && $(MAKE) -f Makefile
sub-ModuleA-make_first: FORCE
	@test -d ModuleA/ || mkdir -p ModuleA/
	cd ModuleA/ && ( test -e Makefile || $(QMAKE) /localhome/MITLL/ma26343/workspace/local/bsci/ModuleA/ModuleA.pro -o Makefile ) && $(MAKE) -f Makefile 
sub-ModuleA-all: FORCE
	@test -d ModuleA/ || mkdir -p ModuleA/
	cd ModuleA/ && ( test -e Makefile || $(QMAKE) /localhome/MITLL/ma26343/workspace/local/bsci/ModuleA/ModuleA.pro -o Makefile ) && $(MAKE) -f Makefile all
sub-ModuleA-clean: FORCE
	@test -d ModuleA/ || mkdir -p ModuleA/
	cd ModuleA/ && ( test -e Makefile || $(QMAKE) /localhome/MITLL/ma26343/workspace/local/bsci/ModuleA/ModuleA.pro -o Makefile ) && $(MAKE) -f Makefile clean
sub-ModuleA-distclean: FORCE
	@test -d ModuleA/ || mkdir -p ModuleA/
	cd ModuleA/ && ( test -e Makefile || $(QMAKE) /localhome/MITLL/ma26343/workspace/local/bsci/ModuleA/ModuleA.pro -o Makefile ) && $(MAKE) -f Makefile distclean
sub-ModuleA-install_subtargets: FORCE
	@test -d ModuleA/ || mkdir -p ModuleA/
	cd ModuleA/ && ( test -e Makefile || $(QMAKE) /localhome/MITLL/ma26343/workspace/local/bsci/ModuleA/ModuleA.pro -o Makefile ) && $(MAKE) -f Makefile install
sub-ModuleA-uninstall_subtargets: FORCE
	@test -d ModuleA/ || mkdir -p ModuleA/
	cd ModuleA/ && ( test -e Makefile || $(QMAKE) /localhome/MITLL/ma26343/workspace/local/bsci/ModuleA/ModuleA.pro -o Makefile ) && $(MAKE) -f Makefile uninstall
sub-ModuleB-qmake_all:  FORCE
	@test -d ModuleB/ || mkdir -p ModuleB/
	cd ModuleB/ && $(QMAKE) /localhome/MITLL/ma26343/workspace/local/bsci/ModuleB/ModuleB.pro -o Makefile
	cd ModuleB/ && $(MAKE) -f Makefile qmake_all
sub-ModuleB: FORCE
	@test -d ModuleB/ || mkdir -p ModuleB/
	cd ModuleB/ && ( test -e Makefile || $(QMAKE) /localhome/MITLL/ma26343/workspace/local/bsci/ModuleB/ModuleB.pro -o Makefile ) && $(MAKE) -f Makefile
sub-ModuleB-make_first: FORCE
	@test -d ModuleB/ || mkdir -p ModuleB/
	cd ModuleB/ && ( test -e Makefile || $(QMAKE) /localhome/MITLL/ma26343/workspace/local/bsci/ModuleB/ModuleB.pro -o Makefile ) && $(MAKE) -f Makefile 
sub-ModuleB-all: FORCE
	@test -d ModuleB/ || mkdir -p ModuleB/
	cd ModuleB/ && ( test -e Makefile || $(QMAKE) /localhome/MITLL/ma26343/workspace/local/bsci/ModuleB/ModuleB.pro -o Makefile ) && $(MAKE) -f Makefile all
sub-ModuleB-clean: FORCE
	@test -d ModuleB/ || mkdir -p ModuleB/
	cd ModuleB/ && ( test -e Makefile || $(QMAKE) /localhome/MITLL/ma26343/workspace/local/bsci/ModuleB/ModuleB.pro -o Makefile ) && $(MAKE) -f Makefile clean
sub-ModuleB-distclean: FORCE
	@test -d ModuleB/ || mkdir -p ModuleB/
	cd ModuleB/ && ( test -e Makefile || $(QMAKE) /localhome/MITLL/ma26343/workspace/local/bsci/ModuleB/ModuleB.pro -o Makefile ) && $(MAKE) -f Makefile distclean
sub-ModuleB-install_subtargets: FORCE
	@test -d ModuleB/ || mkdir -p ModuleB/
	cd ModuleB/ && ( test -e Makefile || $(QMAKE) /localhome/MITLL/ma26343/workspace/local/bsci/ModuleB/ModuleB.pro -o Makefile ) && $(MAKE) -f Makefile install
sub-ModuleB-uninstall_subtargets: FORCE
	@test -d ModuleB/ || mkdir -p ModuleB/
	cd ModuleB/ && ( test -e Makefile || $(QMAKE) /localhome/MITLL/ma26343/workspace/local/bsci/ModuleB/ModuleB.pro -o Makefile ) && $(MAKE) -f Makefile uninstall

Makefile: BSCI.pro /usr/lib64/qt5/mkspecs/linux-g++/qmake.conf /usr/lib64/qt5/mkspecs/features/spec_pre.prf \
		/usr/lib64/qt5/mkspecs/common/unix.conf \
		/usr/lib64/qt5/mkspecs/common/linux.conf \
		/usr/lib64/qt5/mkspecs/common/sanitize.conf \
		/usr/lib64/qt5/mkspecs/common/gcc-base.conf \
		/usr/lib64/qt5/mkspecs/common/gcc-base-unix.conf \
		/usr/lib64/qt5/mkspecs/common/g++-base.conf \
		/usr/lib64/qt5/mkspecs/common/g++-unix.conf \
		/usr/lib64/qt5/mkspecs/qconfig.pri \
		/usr/lib64/qt5/mkspecs/modules/qt_lib_bootstrap_private.pri \
		/usr/lib64/qt5/mkspecs/modules/qt_lib_concurrent.pri \
		/usr/lib64/qt5/mkspecs/modules/qt_lib_concurrent_private.pri \
		/usr/lib64/qt5/mkspecs/modules/qt_lib_core.pri \
		/usr/lib64/qt5/mkspecs/modules/qt_lib_core_private.pri \
		/usr/lib64/qt5/mkspecs/modules/qt_lib_dbus.pri \
		/usr/lib64/qt5/mkspecs/modules/qt_lib_dbus_private.pri \
		/usr/lib64/qt5/mkspecs/modules/qt_lib_gui.pri \
		/usr/lib64/qt5/mkspecs/modules/qt_lib_gui_private.pri \
		/usr/lib64/qt5/mkspecs/modules/qt_lib_network.pri \
		/usr/lib64/qt5/mkspecs/modules/qt_lib_network_private.pri \
		/usr/lib64/qt5/mkspecs/modules/qt_lib_opengl.pri \
		/usr/lib64/qt5/mkspecs/modules/qt_lib_opengl_private.pri \
		/usr/lib64/qt5/mkspecs/modules/qt_lib_openglextensions.pri \
		/usr/lib64/qt5/mkspecs/modules/qt_lib_openglextensions_private.pri \
		/usr/lib64/qt5/mkspecs/modules/qt_lib_platformsupport_private.pri \
		/usr/lib64/qt5/mkspecs/modules/qt_lib_printsupport.pri \
		/usr/lib64/qt5/mkspecs/modules/qt_lib_printsupport_private.pri \
		/usr/lib64/qt5/mkspecs/modules/qt_lib_sql.pri \
		/usr/lib64/qt5/mkspecs/modules/qt_lib_sql_private.pri \
		/usr/lib64/qt5/mkspecs/modules/qt_lib_testlib.pri \
		/usr/lib64/qt5/mkspecs/modules/qt_lib_testlib_private.pri \
		/usr/lib64/qt5/mkspecs/modules/qt_lib_widgets.pri \
		/usr/lib64/qt5/mkspecs/modules/qt_lib_widgets_private.pri \
		/usr/lib64/qt5/mkspecs/modules/qt_lib_xcb_qpa_lib_private.pri \
		/usr/lib64/qt5/mkspecs/modules/qt_lib_xml.pri \
		/usr/lib64/qt5/mkspecs/modules/qt_lib_xml_private.pri \
		/usr/lib64/qt5/mkspecs/features/qt_functions.prf \
		/usr/lib64/qt5/mkspecs/features/qt_config.prf \
		/usr/lib64/qt5/mkspecs/linux-g++/qmake.conf \
		/usr/lib64/qt5/mkspecs/features/spec_post.prf \
		/usr/lib64/qt5/mkspecs/features/exclusive_builds.prf \
		/usr/lib64/qt5/mkspecs/features/default_pre.prf \
		/usr/lib64/qt5/mkspecs/features/resolve_config.prf \
		/usr/lib64/qt5/mkspecs/features/default_post.prf \
		/usr/lib64/qt5/mkspecs/features/warn_on.prf \
		/usr/lib64/qt5/mkspecs/features/file_copies.prf \
		/usr/lib64/qt5/mkspecs/features/testcase_targets.prf \
		/usr/lib64/qt5/mkspecs/features/exceptions.prf \
		/usr/lib64/qt5/mkspecs/features/yacc.prf \
		/usr/lib64/qt5/mkspecs/features/lex.prf \
		BSCI.pro
	$(QMAKE) -o Makefile BSCI.pro
/usr/lib64/qt5/mkspecs/features/spec_pre.prf:
/usr/lib64/qt5/mkspecs/common/unix.conf:
/usr/lib64/qt5/mkspecs/common/linux.conf:
/usr/lib64/qt5/mkspecs/common/sanitize.conf:
/usr/lib64/qt5/mkspecs/common/gcc-base.conf:
/usr/lib64/qt5/mkspecs/common/gcc-base-unix.conf:
/usr/lib64/qt5/mkspecs/common/g++-base.conf:
/usr/lib64/qt5/mkspecs/common/g++-unix.conf:
/usr/lib64/qt5/mkspecs/qconfig.pri:
/usr/lib64/qt5/mkspecs/modules/qt_lib_bootstrap_private.pri:
/usr/lib64/qt5/mkspecs/modules/qt_lib_concurrent.pri:
/usr/lib64/qt5/mkspecs/modules/qt_lib_concurrent_private.pri:
/usr/lib64/qt5/mkspecs/modules/qt_lib_core.pri:
/usr/lib64/qt5/mkspecs/modules/qt_lib_core_private.pri:
/usr/lib64/qt5/mkspecs/modules/qt_lib_dbus.pri:
/usr/lib64/qt5/mkspecs/modules/qt_lib_dbus_private.pri:
/usr/lib64/qt5/mkspecs/modules/qt_lib_gui.pri:
/usr/lib64/qt5/mkspecs/modules/qt_lib_gui_private.pri:
/usr/lib64/qt5/mkspecs/modules/qt_lib_network.pri:
/usr/lib64/qt5/mkspecs/modules/qt_lib_network_private.pri:
/usr/lib64/qt5/mkspecs/modules/qt_lib_opengl.pri:
/usr/lib64/qt5/mkspecs/modules/qt_lib_opengl_private.pri:
/usr/lib64/qt5/mkspecs/modules/qt_lib_openglextensions.pri:
/usr/lib64/qt5/mkspecs/modules/qt_lib_openglextensions_private.pri:
/usr/lib64/qt5/mkspecs/modules/qt_lib_platformsupport_private.pri:
/usr/lib64/qt5/mkspecs/modules/qt_lib_printsupport.pri:
/usr/lib64/qt5/mkspecs/modules/qt_lib_printsupport_private.pri:
/usr/lib64/qt5/mkspecs/modules/qt_lib_sql.pri:
/usr/lib64/qt5/mkspecs/modules/qt_lib_sql_private.pri:
/usr/lib64/qt5/mkspecs/modules/qt_lib_testlib.pri:
/usr/lib64/qt5/mkspecs/modules/qt_lib_testlib_private.pri:
/usr/lib64/qt5/mkspecs/modules/qt_lib_widgets.pri:
/usr/lib64/qt5/mkspecs/modules/qt_lib_widgets_private.pri:
/usr/lib64/qt5/mkspecs/modules/qt_lib_xcb_qpa_lib_private.pri:
/usr/lib64/qt5/mkspecs/modules/qt_lib_xml.pri:
/usr/lib64/qt5/mkspecs/modules/qt_lib_xml_private.pri:
/usr/lib64/qt5/mkspecs/features/qt_functions.prf:
/usr/lib64/qt5/mkspecs/features/qt_config.prf:
/usr/lib64/qt5/mkspecs/linux-g++/qmake.conf:
/usr/lib64/qt5/mkspecs/features/spec_post.prf:
/usr/lib64/qt5/mkspecs/features/exclusive_builds.prf:
/usr/lib64/qt5/mkspecs/features/default_pre.prf:
/usr/lib64/qt5/mkspecs/features/resolve_config.prf:
/usr/lib64/qt5/mkspecs/features/default_post.prf:
/usr/lib64/qt5/mkspecs/features/warn_on.prf:
/usr/lib64/qt5/mkspecs/features/file_copies.prf:
/usr/lib64/qt5/mkspecs/features/testcase_targets.prf:
/usr/lib64/qt5/mkspecs/features/exceptions.prf:
/usr/lib64/qt5/mkspecs/features/yacc.prf:
/usr/lib64/qt5/mkspecs/features/lex.prf:
BSCI.pro:
qmake: FORCE
	@$(QMAKE) -o Makefile BSCI.pro

qmake_all: sub-ModuleA-qmake_all sub-ModuleB-qmake_all FORCE

make_first: sub-ModuleA-make_first sub-ModuleB-make_first  FORCE
all: sub-ModuleA-all sub-ModuleB-all  FORCE
clean: sub-ModuleA-clean sub-ModuleB-clean  FORCE
distclean: sub-ModuleA-distclean sub-ModuleB-distclean  FORCE
	-$(DEL_FILE) Makefile
install_subtargets: sub-ModuleA-install_subtargets sub-ModuleB-install_subtargets FORCE
uninstall_subtargets: sub-ModuleA-uninstall_subtargets sub-ModuleB-uninstall_subtargets FORCE

sub-ModuleA-check:
	@test -d ModuleA/ || mkdir -p ModuleA/
	cd ModuleA/ && ( test -e Makefile || $(QMAKE) /localhome/MITLL/ma26343/workspace/local/bsci/ModuleA/ModuleA.pro -o Makefile ) && $(MAKE) -f Makefile check
sub-ModuleB-check:
	@test -d ModuleB/ || mkdir -p ModuleB/
	cd ModuleB/ && ( test -e Makefile || $(QMAKE) /localhome/MITLL/ma26343/workspace/local/bsci/ModuleB/ModuleB.pro -o Makefile ) && $(MAKE) -f Makefile check
check: sub-ModuleA-check sub-ModuleB-check

sub-ModuleA-benchmark:
	@test -d ModuleA/ || mkdir -p ModuleA/
	cd ModuleA/ && ( test -e Makefile || $(QMAKE) /localhome/MITLL/ma26343/workspace/local/bsci/ModuleA/ModuleA.pro -o Makefile ) && $(MAKE) -f Makefile benchmark
sub-ModuleB-benchmark:
	@test -d ModuleB/ || mkdir -p ModuleB/
	cd ModuleB/ && ( test -e Makefile || $(QMAKE) /localhome/MITLL/ma26343/workspace/local/bsci/ModuleB/ModuleB.pro -o Makefile ) && $(MAKE) -f Makefile benchmark
benchmark: sub-ModuleA-benchmark sub-ModuleB-benchmark
install:install_subtargets  FORCE

uninstall: uninstall_subtargets FORCE

FORCE:

dist: distdir FORCE
	(cd `dirname $(DISTDIR)` && $(TAR) $(DISTNAME).tar $(DISTNAME) && $(COMPRESS) $(DISTNAME).tar) && $(MOVE) `dirname $(DISTDIR)`/$(DISTNAME).tar.gz . && $(DEL_FILE) -r $(DISTDIR)

distdir: sub-ModuleA-distdir sub-ModuleB-distdir FORCE
	@test -d $(DISTDIR) || mkdir -p $(DISTDIR)
	$(COPY_FILE) --parents /usr/lib64/qt5/mkspecs/features/spec_pre.prf /usr/lib64/qt5/mkspecs/common/unix.conf /usr/lib64/qt5/mkspecs/common/linux.conf /usr/lib64/qt5/mkspecs/common/sanitize.conf /usr/lib64/qt5/mkspecs/common/gcc-base.conf /usr/lib64/qt5/mkspecs/common/gcc-base-unix.conf /usr/lib64/qt5/mkspecs/common/g++-base.conf /usr/lib64/qt5/mkspecs/common/g++-unix.conf /usr/lib64/qt5/mkspecs/qconfig.pri /usr/lib64/qt5/mkspecs/modules/qt_lib_bootstrap_private.pri /usr/lib64/qt5/mkspecs/modules/qt_lib_concurrent.pri /usr/lib64/qt5/mkspecs/modules/qt_lib_concurrent_private.pri /usr/lib64/qt5/mkspecs/modules/qt_lib_core.pri /usr/lib64/qt5/mkspecs/modules/qt_lib_core_private.pri /usr/lib64/qt5/mkspecs/modules/qt_lib_dbus.pri /usr/lib64/qt5/mkspecs/modules/qt_lib_dbus_private.pri /usr/lib64/qt5/mkspecs/modules/qt_lib_gui.pri /usr/lib64/qt5/mkspecs/modules/qt_lib_gui_private.pri /usr/lib64/qt5/mkspecs/modules/qt_lib_network.pri /usr/lib64/qt5/mkspecs/modules/qt_lib_network_private.pri /usr/lib64/qt5/mkspecs/modules/qt_lib_opengl.pri /usr/lib64/qt5/mkspecs/modules/qt_lib_opengl_private.pri /usr/lib64/qt5/mkspecs/modules/qt_lib_openglextensions.pri /usr/lib64/qt5/mkspecs/modules/qt_lib_openglextensions_private.pri /usr/lib64/qt5/mkspecs/modules/qt_lib_platformsupport_private.pri /usr/lib64/qt5/mkspecs/modules/qt_lib_printsupport.pri /usr/lib64/qt5/mkspecs/modules/qt_lib_printsupport_private.pri /usr/lib64/qt5/mkspecs/modules/qt_lib_sql.pri /usr/lib64/qt5/mkspecs/modules/qt_lib_sql_private.pri /usr/lib64/qt5/mkspecs/modules/qt_lib_testlib.pri /usr/lib64/qt5/mkspecs/modules/qt_lib_testlib_private.pri /usr/lib64/qt5/mkspecs/modules/qt_lib_widgets.pri /usr/lib64/qt5/mkspecs/modules/qt_lib_widgets_private.pri /usr/lib64/qt5/mkspecs/modules/qt_lib_xcb_qpa_lib_private.pri /usr/lib64/qt5/mkspecs/modules/qt_lib_xml.pri /usr/lib64/qt5/mkspecs/modules/qt_lib_xml_private.pri /usr/lib64/qt5/mkspecs/features/qt_functions.prf /usr/lib64/qt5/mkspecs/features/qt_config.prf /usr/lib64/qt5/mkspecs/linux-g++/qmake.conf /usr/lib64/qt5/mkspecs/features/spec_post.prf /usr/lib64/qt5/mkspecs/features/exclusive_builds.prf /usr/lib64/qt5/mkspecs/features/default_pre.prf /usr/lib64/qt5/mkspecs/features/resolve_config.prf /usr/lib64/qt5/mkspecs/features/default_post.prf /usr/lib64/qt5/mkspecs/features/warn_on.prf /usr/lib64/qt5/mkspecs/features/file_copies.prf /usr/lib64/qt5/mkspecs/features/testcase_targets.prf /usr/lib64/qt5/mkspecs/features/exceptions.prf /usr/lib64/qt5/mkspecs/features/yacc.prf /usr/lib64/qt5/mkspecs/features/lex.prf BSCI.pro $(DISTDIR)/

sub-ModuleA-distdir: FORCE
	@test -d ModuleA/ || mkdir -p ModuleA/
	cd ModuleA/ && ( test -e Makefile || $(QMAKE) /localhome/MITLL/ma26343/workspace/local/bsci/ModuleA/ModuleA.pro -o Makefile ) && $(MAKE) -e -f Makefile distdir DISTDIR=$(DISTDIR)/ModuleA

sub-ModuleB-distdir: FORCE
	@test -d ModuleB/ || mkdir -p ModuleB/
	cd ModuleB/ && ( test -e Makefile || $(QMAKE) /localhome/MITLL/ma26343/workspace/local/bsci/ModuleB/ModuleB.pro -o Makefile ) && $(MAKE) -e -f Makefile distdir DISTDIR=$(DISTDIR)/ModuleB

