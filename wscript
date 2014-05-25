#!/usr/bin/env python

import platform

from waflib.Configure import conf

SYSTEM=platform.system().lower()
VARIANTS=[]
if SYSTEM in ('linux', 'windows'):
    VARIANTS.append('gcc')
if SYSTEM == 'windows':
    VARIANTS.append('msvc')


def _check_gettimeofday(conf, cross=False):
    """ Can we make use of gettimeofday ?

    If cross is False, we check runime support on the current BUILD host.
    Otherwise we simply check we can compile with the appropriate api's and
    defines referenced.

    If yes, define "HAVE_GETTIMEOFDAY"


    """
    return conf.check_cc(fragment="""
        # include <sys/time.h>
        int main() {
            struct timeval res;
            return gettimeofday(&res, 0);
        }
        """,
        msg = "HAVE we got gettimeofday ?",
        execute=not cross, # execute the program to perform the test.
        define_ret = False, # The exit status is 0 on success
        define_name="HAVE_GETTIMEOFDAY",
        mandatory=False # If the test fails we will fall back to
                        # gettimeofday
        )


@conf
def check_gettimeofday(conf):
    return _check_gettimeofday(conf, cross=False)


@conf
def check_gettimeofday_cross_cc(conf):
    return _check_gettimeofday(conf, cross=True)


def _check_clock_monotonic(conf, cross=False):
    """ Can we make use of the posix monotonic clock ?

    We verify both that we have the clock_gettime api and that the
    CLOCK_MONOTONIC clock id is supported.

    If cross is False, we check runime support on the current BUILD host.
    Otherwise we simply check we can compile with the appropriate api's and
    defines referenced.

    If yes, define "HAVE_CLOCK_MONOTONIC"


    """
    return conf.check_cc(fragment="""
        # include <time.h>
        int main() {
            struct timespec res;
            return clock_gettime(CLOCK_MONOTONIC, &res);
        }
        """,
        msg = "HAVE we got CLOCK_MONOTONIC ?",
        execute=not cross, # execute the program to perform the test.
        define_ret = False, # The exit status is 0 on success
        define_name="HAVE_CLOCK_MONOTONIC",
        mandatory=False # If the test fails we will fall back to
                        # gettimeofday
        )


@conf
def check_clock_monotonic(conf):
    return _check_clock_monotonic(conf, cross=False)


@conf
def check_clock_monotonic_cross_cc(conf):
    return _check_clock_monotonic(conf, cross=True)


def options(opt):
    opt.load('compiler_c')
    opt.load('compiler_cxx')


def configure(conf):

    if not VARIANTS:
        conf.fatal('No variants enabled for system "%s"', SYSTEM)

    # Check for gcc on windows, as it picks up mingw-gcc
    assert 'gcc' in VARIANTS
    conf.setenv('gcc')
    conf.env.stash()
    try:
        conf.load('gcc')
        conf.env.CFLAGS = ['-g']
        conf.env.prepend_value('INCLUDES', [conf.path.get_bld().abspath()])

        if conf.env.DEST_OS.startswith('win'):
            # If we are mingw-gcc we need -mwindows, note that DEST_OS is
            # set by conf.load above.
            conf.env.LINKFLAGS_PLATFORM = [
                '-mwindows'
                ]
        else:
            # Assume posix like.

            conf.check_clock_monotonic()
            conf.check_clock_monotonic_cross_cc()
            conf.check_gettimeofday()
            conf.check_gettimeofday_cross_cc()

        conf.write_config_header('config.h')
    except:
        conf.env.revert()

    if 'msvc' in VARIANTS:
        conf.setenv('msvc')
        conf.env.stash()
        try:
            conf.load('msvc')
            conf.env.prepend_value('INCLUDES', [conf.path.get_bld().abspath()])
            conf.write_config_header('config.h')
        except:
            conf.env.revert()
    conf.recurse("spanclock")
    conf.recurse("3rdparty/gtest-1.7.0/src")



@conf
def feature_test_program(bld):
    """Add test programs.

    Finds all files matching test-*.c which are accompanied by a single file
    which can be produced by striping test- from the match.

    test-foo.c, foo.c -> test-foo.exe
    test-foo.c, foo-bar.c -> None

    """
    for test in bld.path.ant_glob('test-*.c'):
        feat=test.parent.find_node(str(test).replace('test-', '', 1))
        if feat is None:
            continue
        bld.program(source=[test, feat], target=str(feat).rsplit('.', 1)[0],
            use='PLATFORM')


def build(bld):
    bld.recurse("spanclock")
    bld.recurse("3rdparty/gtest-1.7.0/src")

from waflib.Build import BuildContext, CleanContext, \
    InstallContext, UninstallContext

for x in VARIANTS:
    for y in (BuildContext, CleanContext, InstallContext, UninstallContext):
        name = y.__name__.replace('Context','').lower()
        class tmp(y):
            cmd = name + '-' + x
            variant = x
#:end
