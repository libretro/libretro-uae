

# AC_PROG_OBJC([LIST-OF-COMPILERS])
#
AC_DEFUN([AS_PROG_OBJC],
[
AC_CHECK_TOOLS(OBJC,
	[m4_default([$1], [objcc objc gcc cc CC])],
	none)
AC_SUBST(OBJC)
OBJC_LDFLAGS="-lobjc"
AC_SUBST(OBJC_LDFLAGS)
if test "x$OBJC" != xnone ; then
  _AM_DEPENDENCIES(OBJC)
  AC_MSG_CHECKING([if Objective C compiler works])
  cat >>conftest.m <<EOF
#include <objc/Object.h>
@interface Moo:Object
{
}
- moo;
int main();
@end

@implementation Moo
- moo
{
  exit(0);
}

int main()
{
  id moo;
  moo = [[Moo new]];
  [[moo moo]];
  return 1;
}
@end
EOF
  ${OBJC} conftest.m ${OBJC_LDFLAGS} >&5 2>&5
  if test -f a.out -o -f a.exe ; then
    result=yes
  else
    result=no
    echo failed program is: >&5
    cat conftest.m >&5
  fi
  rm -f conftest.m a.out a.exe
  AC_MSG_RESULT([$result])
fi

])


