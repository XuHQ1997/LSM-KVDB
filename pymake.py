import pathlib
import subprocess


# parse srcfile's dependency by g++ -MM
def gMM_parse(srcfile, include_dirs):
    cmd = 'g++'
    args = []

    args.append('-std=c++11')
    for include in include_dirs:
        args.append('-I ' + include)
    args.append('-MM')
    args.append(str(srcfile))

    result = subprocess.Popen(' '.join([cmd, *args]),
                              stdout=subprocess.PIPE, 
                              stderr=subprocess.PIPE, 
                              cwd=pathlib.Path().cwd(),
                              shell=True,
                              encoding='utf-8')
    print(result.args)
    if result.wait() != 0:
        _, err = result.communicate()
        print(srcfile, 'compile err')
        print(err)
        exit(-1)
    out, _ = result.communicate()
    target, dependency = out.split(':')

    cppfiles, hfiles = set(), set()
    for line in dependency.split(' '):
        if 'third_party' in line:
            continue

        file = line.strip()
        file_path = pathlib.Path(file)
        suffix = file_path.suffix

        if suffix == '.h':
            hfiles.add(str(file_path))
            cpp_file_path = file_path.parent / (file_path.stem + '.cpp')
            if cpp_file_path.exists():
                cppfiles.add(str(cpp_file_path))
        elif suffix == '.cpp':
            cppfiles.add(str(file_path))
    return target, cppfiles, hfiles


class MakefileGenerator:
    def __init__(
        self,
        compiler='g++',
        cflags='-O --std=c++11',
        include_dirs=None, 
        library_dirs=None,
        libraries=None
    ):
        self.compiler = compiler
        self.cflags = cflags
        self.include_dirs = include_dirs
        self.library_dirs = library_dirs
        self.libraries = libraries

        self.I_args = ' '
        if self.include_dirs:
            self.I_args = '-I' + ' -I'.join(self.include_dirs)
        self.L_args = ' '
        if self.library_dirs:
            self.L_args = '-L' + ' -L'.join(self.library_dirs)
        self.lib_args = ' '
        if self.libraries:
            self.lib_args = '-l' + ' -l'.join(self.libraries)

        ccmd_format = '{compiler} {cflags} {srcfiles} {I_args} {L_args} {lib_args} -o {target}'
        self.ccmd = ccmd_format.format(compiler=self.compiler, cflags=self.cflags,
                                       I_args=self.I_args, L_args=self.L_args, 
                                       lib_args=self.lib_args,
                                       srcfiles='{srcfiles}', target='{target}')
    
    @staticmethod
    def mkfile_header(target, cppfiles, hfiles):
        header = target + ':'
        if cppfiles:
            header += ' ' + ' '.join(cppfiles)
        if hfiles:
            header += ' ' + ' '.join(hfiles)
        return header

    def __call__(self, target, cppfiles, hfiles):
        header = self.mkfile_header(target, cppfiles, hfiles)
        
        srcfiles = ' '.join(cppfiles)
        ccmd = self.ccmd.format(srcfiles=srcfiles, target=target)
        return header + '\n\t' + ccmd


def makefile_for_test(outfile):
    srcdir = pathlib.Path('test')
    outdir = pathlib.Path('bin/test')
    if not outdir.exists():
        outdir.mkdir(parents=True)

    compiler = 'g++'
    cflags = '--std=c++11'

    include_dirs = ['.', 'third_party/googletest/googletest/include']
    library_dirs = ['third_party/googletest/lib']
    libraries = ['gtest', 'pthread']

    mkg = MakefileGenerator(compiler=compiler,
                            cflags=cflags,
                            include_dirs=include_dirs,
                            library_dirs=library_dirs,
                            libraries=libraries)
    targets = []
    for srcfile in srcdir.glob('*.cpp'):
        target, cppfiles, hfiles = gMM_parse(srcfile, include_dirs)
        target = str(outdir / target)
        targets.append(target)

        mkunit = mkg(target, cppfiles, hfiles)
        outfile.write(mkunit + '\n\n')
    return targets


def makefile_for_all(outfile, *targets):
    phoney = '.PHONEY: all'
    header = 'all: ' + ' '.join(targets)
    cmd = '@echo make success'
    outfile.write(phoney + '\n' + header + '\n\t' + cmd + '\n\n')
    return 'all'


def makefile_for_clean(outfile):
    phoney = '.PHONEY: clean'
    header = 'clean:'
    
    cmds = []
    for bindir in pathlib.Path('bin').iterdir():
        cmds.append('rm ' + str(bindir) + '/*')
    cmd = '\n\t'.join(cmds)

    outfile.write(phoney + '\n' + header + '\n\t' + cmd + '\n\n')
    return 'clean'


if __name__ == '__main__':
    with open('Makefile', 'w', encoding='utf-8') as outfile:
        test_targets = makefile_for_test(outfile)
        makefile_for_all(outfile, *test_targets)
        makefile_for_clean(outfile)
