import platform

from waflib.Configure import conf
import sys

SYSTEM=platform.system().lower()
VARIANTS=[]
if SYSTEM in ('linux', 'windows'):
    VARIANTS.append('gcc')
if SYSTEM == 'windows':
    VARIANTS.append('msvc')

def options(opt):
    opt.load('compiler_c')

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

        # If we are mingw-gcc we need -mwindows, note that DEST_OS is
        # set by conf.load above.

        if conf.env.DEST_OS.startswith('win'):
            conf.env.LINKFLAGS_PLATFORM = [
                '-mwindows'
                ]
    except:
        conf.env.revert()

    if 'msvc' in VARIANTS:
        conf.setenv('msvc')
        conf.env.stash()
        try:
            conf.load('msvc')
        except:
            conf.env.revert()


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

for x in VARIANTS:
    for y in (BuildContext, CleanContext, InstallContext, UninstallContext):
        name = y.__name__.replace('Context','').lower()
        class tmp(y):
            cmd = name + '-' + x
            variant = x
#:end
