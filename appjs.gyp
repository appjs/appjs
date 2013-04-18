{
  'includes': [
    'appjs_paths.gypi',
    './deps/node/config.gypi'
  ],
  'target_defaults': {
    'defines': ['APPJS_BUILD'],
  },
  'targets': [
    {
      'target_name': 'appjs_package',
      'type':'none',
      'dependencies': [
        'appjs'
      ],
      'copies':[
        {
          'destination': './app/data/bin',
          'files': [
            '<(PRODUCT_DIR)/appjs',
          ]
        }
      ]
    },
    {
      'target_name': 'appjs',
      'type': 'executable',
      'msvs_guid': 'A9D6DC71-C0DC-4549-AEB1-3B15B44E86A9',
      'defines!': ['CONTENT_IMPLEMENTATION'],
      'sources': [
        '<@(common_sources)'
      ],
      'dependencies': [
        '<@(common_dependencies)'
      ],
      'include_dirs': [
        '<@(common_include_dirs)'
      ],
      'cflags': [
        '-fvisibility=default',
      ],
      'conditions': [
        ['OS=="mac"', {
          'sources': [
            '<@(mac_sources)'
          ],
          'defines': [
            '__MAC__',
          ],
          'cflags': [ '-m32' ],
          'ldflags': [ '-m32' ],
          'xcode_settings': {
            'OTHER_CFLAGS': ['-ObjC++'],
            'ARCHS': [ 'i386' ]
          },
          'link_settings': {
            'libraries': [
              'obj/cef/libcef_static.a',
              'obj/cef/libcef.a'
              '-lobjc'
            ]
          }
        }],
        ['OS=="linux"', {
          'sources': [
            '<@(linux_sources)'
          ],
          'defines': [
            '__LINUX__'
          ],
          'cflags': [
            '<!@(pkg-config --cflags gtk+-2.0 gthread-2.0)',
          ],
          'link_settings': {
            'ldflags': [
              '<!@(pkg-config --libs-only-L --libs-only-other gtk+-2.0 gthread-2.0)',
            ],
            'libraries': [
              '<!@(pkg-config --libs-only-l gtk+-2.0 gthread-2.0)',
              'obj/cef/libcef_static.a',
              'obj/cef/libcef.a'
            ],
          }
        }],
        ['OS=="win"', {
          'sources': [
            '<@(win_sources)'
          ],
          'defines': [
            '__WIN__',
            '_WINSOCKAPI_',
            '_UNICODE',
            'UNICODE'
          ],
          'link_settings': {
            'libraries': [
              'GdiPlus.lib',
              'Shlwapi.lib',
            ],
          },
          'msvs_settings': {
            'VCCLCompilerTool': {
              'WholeProgramOptimization': 'true', # /GL, whole program optimization, needed for LTCG
              'OmitFramePointers': 'true',
              'EnableFunctionLevelLinking': 'true',
              'EnableIntrinsicFunctions': 'true',
              'RuntimeTypeInfo': 'false',
              'ExceptionHandling': '1',
            },
            'VCLibrarianTool': {
              'AdditionalOptions': [
                '/LTCG', # link time code generation
              ],
            },
            'VCLinkerTool': {
              'LinkTimeCodeGeneration': 1, # link-time code generation
              'OptimizeReferences': 2, # /OPT:REF
              'EnableCOMDATFolding': 2, # /OPT:ICF
              'LinkIncremental': 1, # disable incremental linking
            },
          },
        }]
      ]
    }
  ]
}
