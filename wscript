#!/usr/bin/env python

from os.path import abspath, join
from itertools import chain

import platform

from waflib.Configure import conf

def abj(*p):
    return abspath(join(*p))

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

    env = conf.env

    # A related case is how to add the top-level directory containing a configuration header

    if not VARIANTS:
        conf.fatal('No variants enabled for system "%s"', SYSTEM)

    conf.env.VARIANTS = VARIANTS

    # Check for gcc on windows, as it picks up mingw-gcc
    assert 'gcc' in VARIANTS
    conf.setenv('gcc', conf.env.derive())
    conf.env.stash()
    try:

        # Include path for variant "config.h"
        conf.env.INCLUDES_config = [
                join(conf.path.get_bld().abspath(), 'gcc')]

        conf.load('gcc')
        conf.env.CFLAGS = ['-g']

        if conf.env.DEST_OS.startswith('win'):
            # If we are mingw-gcc we need -mwindows, note that DEST_OS is
            # set by conf.load above.
            conf.env.LINKFLAGS_platform = [
                '-mwindows'
                ]

        conf.check_clock_monotonic()
        conf.check_clock_monotonic_cross_cc()
        conf.check_gettimeofday()
        conf.check_gettimeofday_cross_cc()

    except:
        conf.env.revert()

    conf.setenv('') # Restore default environ

    if 'msvc' in VARIANTS:
        conf.setenv('msvc', env.derive())
        conf.env.stash()
        try:
            # Include path for variant "config.h"
            conf.env.INCLUDES_config = [
                    join(conf.path.get_bld().abspath(), 'msvc')]

            conf.load('msvc')

            conf.check_gettimeofday()
            conf.check_gettimeofday_cross_cc()

        except:
            conf.env.revert()

    conf.setenv('') # Restore default environ
    conf.recurse("3rdparty/gtest-1.7.0/src")

    conf.setenv('') # Restore default environ
    conf.recurse("spanclock")

    # Write out the variant specific config.h
    for v in VARIANTS:
        conf.setenv(v)
        conf.write_config_header(join(v, 'config.h'))

    conf.setenv('') # Restore default environ



@conf
def feature_test_program(bld, **prgkw):
    """Add test programs.

    Finds all files matching test-*.c which are accompanied by a single file
    which can be produced by striping test- from the match.

    We accomodate a cpp file wrapping a c file provided both have the same base
    name, in which case the cpp should #include the c file.

    test-foo.c, foo.c -> test-foo.exe
    test-foo.c, foo-bar.c -> None

    """

    if 'use' not in prgkw:
        prgkw['use'] = 'platform config'

    for test in chain(
            bld.path.ant_glob('test-*.c'),
            bld.path.ant_glob('test-*.cpp')):

        # produce the basename of the feature by removing the 'test-' prefix
        # and the file extention from the test file name.
        featbasename = str(test).rsplit('.', 1)[0].replace('test-', '', 1)

        for ext in [".cpp", ".c"]:

            # If there are both .c and .cpp interfaces, assume we have .cpp
            # wrapping (via #include) a .c interface. If .cpp is found first we
            # will NOT create a node for the .c

            feat = test.parent.find_node(featbasename + ext)
            if feat is not None:
                bld.program(source=[test, feat], target=featbasename, **prgkw)
                break
        else:
            bld.fatal("No implementation for %s" % str(test))



def build(bld):
    bld.recurse("3rdparty/gtest-1.7.0/src")
    bld.recurse("spanclock")

from waflib.Build import BuildContext, CleanContext, \
    InstallContext, UninstallContext

for x in VARIANTS:
    for y in (BuildContext, CleanContext, InstallContext, UninstallContext):
        name = y.__name__.replace('Context','').lower()
        class tmp(y):
            cmd = name + '-' + x
            variant = x
#:end
