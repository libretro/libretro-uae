dnl

AC_INIT(sysconfig.h.in)
AC_PREREQ(2.50)
AC_CONFIG_HEADER(sysconfig.h)
AC_CANONICAL_TARGET

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_MAKE_SET
AC_PROG_INSTALL

AC_AIX
AC_ISC_POSIX

dnl Checks for header files
AC_HEADER_STDC

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE

AC_CHECK_SIZEOF(char, 1)
AC_CHECK_SIZEOF(short, 2)
AC_CHECK_SIZEOF(int, 4)
AC_CHECK_SIZEOF(long, 4)
AC_CHECK_SIZEOF(long long, 8)
AC_CHECK_SIZEOF(__int64, 8)
AC_CHECK_SIZEOF(void *)

if test $ac_cv_prog_gcc = yes; then
  CFLAGS="$CFLAGS -Wall -W -Wno-unused" 
fi

dnl Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([strdup strcasecmp strcmpi stricmp])

AC_SUBST(ac_cv_c_inline)
AC_SUBST(SET_MAKE)

AC_SUBST(top_srcdir)

dnl Do we want a possible optimization for bigendian targets?
dnl So far, this has proved beneifical only on OS X (and probably Dawrwin), so we
dnl only enable it there. Should really check compiler version too.
if [[ "$target_cpu" = "powerpc" ]]; then
  case $target_os in
    darwin*)
      CFLAGS="$CFLAGS -DUSE_DUBIOUS_BIGENDIAN_OPTIMIZATION"
      ;;
  esac
fi

AC_CONFIG_FILES([Makefile])

AC_CONFIG_LINKS([missing.c:../missing.c
                 writelog.c:../writelog.c
                 readcpu.c:../readcpu.c
                 build68k.c:../build68k.c
                 gencpu.c:../gencpu.c
		 gencomp.c:../gencomp.c
                 genblitter.c:../genblitter.c
		 blitops.c:../blitops.c
		 genlinetoscr.c:../genlinetoscr.c
])

AC_OUTPUT
