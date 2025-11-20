import setuptools
import sys

with open('README') as file:
    readme = file.read()

extra_link_args = []
extra_compile_args = ['-std=c++11', '-w']
if sys.platform == "linux":
    extra_compile_args += ['-g0']
if sys.platform == "darwin":
    extra_compile_args += ['-stdlib=libc++']
    extra_link_args += ['-stdlib=libc++']

setuptools.setup(
    name             = 'ufal.udpipe',
    version          = '',
    description      = 'Bindings to UDPipe library',
    long_description = readme,
    author           = 'Milan Straka',
    author_email     = 'straka@ufal.mff.cuni.cz',
    url              = 'https://ufal.mff.cuni.cz/udpipe',
    license          = 'mpl-2.0',
    license_files    = ['LICENSE'],
    packages         = ['ufal', 'ufal.udpipe'],
    ext_modules      = [setuptools.Extension(
        'ufal.udpipe._udpipe',
        ['ufal/udpipe/udpipe.cpp', 'ufal/udpipe/udpipe_python.cpp'],
        language = 'c++',
        extra_compile_args = extra_compile_args,
        extra_link_args = extra_link_args)],
    classifiers      = [
        'Development Status :: 5 - Production/Stable',
        'Programming Language :: C++',
        'Programming Language :: Python',
        'Programming Language :: Python :: 3',
        'Topic :: Software Development :: Libraries'
    ]
)
