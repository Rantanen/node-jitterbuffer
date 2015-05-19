# Build external deps.
{
    'variables': { 'target_arch%': 'x64' },

    'target_defaults': {
        'default_configuration': 'Debug',
        'configuration': {
            'Debug': {
                'defines': [ 'DEBUG', '_DEBUG' ],
                'msvs_settings': {
                    'VSSLCompilerTool': {
                        'RuntimeLibrary': 1, #static debug
                    },
                },
            },
            'Release': {
                'defines': [ 'NODEBUG' ],
                'msvs_settings': {
                    'VSSLCompilerTool': {
                        'RuntimeLibrary': 0, #static release
                    },
                },
            },
        },
        'msvs_settings': {
            'VCLinkerTool': {
                'GenerateDebugInformation': 'true',
            },
        },
    },

    'targets': [
        {
            'target_name': 'libspeexdsp',
            'type': 'static_library',
            'sources': [
                'speex-1.2rc1/libspeex/preprocess.c',
                'speex-1.2rc1/libspeex/jitter.c',
                'speex-1.2rc1/libspeex/mdf.c',
                'speex-1.2rc1/libspeex/fftwrap.c',
                'speex-1.2rc1/libspeex/filterbank.c',
                'speex-1.2rc1/libspeex/resample.c',
                'speex-1.2rc1/libspeex/buffer.c',
                'speex-1.2rc1/libspeex/scal.c',
                'speex-1.2rc1/libspeex/smallft.c',
                'speex-1.2rc1/libspeex/kiss_fft.c',
                'speex-1.2rc1/libspeex/_kiss_fft_guts.h',
                'speex-1.2rc1/libspeex/kiss_fft.h',
                'speex-1.2rc1/libspeex/kiss_fftr.c',
                'speex-1.2rc1/libspeex/kiss_fftr.h'
            ],
            'cflags': [
                '-fvisibility=hidden',
                '-W',
                '-Wstrict-prototypes',
                '-Wno-parentheses',
                '-Wno-unused-parameter',
                '-Wno-sign-compare',
                '-Wno-unused-variable',
            ],
            'include_dirs': [
                'config/speex-1.2rc1/<(OS)/<(target_arch)',
                'speex-1.2rc1/include',
            ],
            'defines': [
                'PIC',
                'HAVE_CONFIG_H',
                '_USE_MATH_DEFINES',
            ]
        }
    ]
}
