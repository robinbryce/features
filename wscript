from waflib.Configure import conf

import sys

def options(opt):
    opt.load('compiler_c')

def configure(conf):
    conf.load('compiler_c')
    conf.env.CFLAGS = ['-g']
    conf.env.LINKFLAGS_PLATFORM = [
        '-mwindows' # Adds system libraries for windows for gcc on mingw
        ]


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

    #bld.program(source=['test-hirestime.c', 'hirestime.c'],
    #        target='test-hirestime')
    #return

    bld.recurse("hirestime")
