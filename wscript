from waflib.Configure import conf

import sys

def options(opt):
    opt.load('compiler_c')

def configure(conf):
    conf.setenv('gcc')
    conf.load('gcc')
    conf.env.CFLAGS = ['-g']

    # Adds system libraries for windows for gcc on mingw
    if conf.env.DEST_OS.startswith('win'):
        conf.env.LINKFLAGS_PLATFORM = [
            '-mwindows'
            ]
    conf.setenv('msvc')
    conf.load('msvc')


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
    bld.recurse("hirestime")

from waflib.Build import BuildContext, CleanContext, \
        InstallContext, UninstallContext

for x in 'gcc msvc'.split():
        for y in (BuildContext, CleanContext, InstallContext, UninstallContext):
                name = y.__name__.replace('Context','').lower()
                class tmp(y):
                        cmd = name + '-' + x
                        variant = x
