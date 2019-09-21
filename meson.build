project('pex-loader', 'cpp', version: '1.0.0', default_options: ['cpp_std=c++17'])


sources = [
    'src/read_early_header.cpp',
    'src/v0/read_sections.cpp',
]

includes = include_directories(
    'include',
)


test_sources = ['test/src/test.cpp']
test_includes = [include_directories('test/include')] + [includes]


dependencies = [
    dependency('libbinary_format'),
]


libpex_loader = library(
    'pex_loader',
    sources,
    include_directories: includes,
    dependencies: dependencies,
)


catch2_test_executable = executable(
    'catch2_test',
    test_sources,
    include_directories: test_includes,
    link_with: libpex_loader,
    dependencies: dependencies,
)

test('catch2_test_suit', catch2_test_executable)

pkg = import('pkgconfig')
pkg.generate(
    description: 'A library for loading and parsing PEX files. Written for pyke',
    filebase: 'pex-loader',
    libraries: libpex_loader,
    name: 'pex-loader',
    subdirs: 'include',
)
