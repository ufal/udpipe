use 5.006;
use strict;
use warnings FATAL => 'all';
use Module::Build;

my $class = Module::Build->subclass(
    class => "Module::Build::UDPipe",
    code => <<'SUBCLASS');
sub ACTION_build {
    my $self = shift;
    print STDERR "Building requires C++11 compiler, either g++ 4.7 or newer, or clang++ 3.2 or newer.\n";
    $self->SUPER::ACTION_build;
}
SUBCLASS

my $builder = $class->new(
    module_name          => 'Ufal::UDPipe',
    license              => 'Mozilla_2_0',
    dist_author          => 'Milan Straka <straka@ufal.mff.cuni.cz>',
    dist_abstract        => 'Bindings to UDPipe library',
    dist_version_from    => 'lib/Ufal/UDPipe.pm',
    release_status       => 'stable',
    configure_requires   => {
        'Module::Build'  => 0,
    },
    build_requires       => {
        'ExtUtils::CBuilder' => 0,
        'Test::More' => 0,
    },
    add_to_cleanup       => ['Ufal-UDPipe-*'],
    extra_compiler_flags => ['-x', 'c++', '-std=c++11', '-fvisibility=hidden', '-w', '-Wno-reserved-user-defined-literal'],
    extra_linker_flags   => ['-lstdc++'],
    c_source             => 'udpipe',
    meta_merge => {
        resources => {
            license => 'https://www.mozilla.org/en-US/MPL/2.0/',
            homepage => 'http://ufal.mff.cuni.cz/udpipe',
            repository => 'https://github.com/ufal/udpipe.git',
            bugtracker => 'https://github.com/ufal/udpipe/issues',
        },
    },
);

$builder->create_build_script();
