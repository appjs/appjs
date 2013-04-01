{
  'includes': [
    #'./deps/node/common.gypi',
    './deps/node/config.gypi'
  ],
  'target_defaults': {
    'defines': ['APPJS_BUILD'],
  },
  'targets': [
    {
      'target_name': 'appjs_publish',
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
      'variables': {
        'chromium_code': 1,
      },
      'sources': [
        'src/main.cpp',
        'src/appjs.cpp',
        'src/appjs_app.cpp',
        'src/appjs_window.cpp',
        'src/appjs_menu.cpp',
        'src/appjs_status_icon.cpp',
        'src/native_window/native_window.cpp',
        'src/native_menu/native_menu.cpp',
        'src/native_status_icon/native_status_icon.cpp',
        'src/includes/cef_handler.cpp',
        'src/includes/cef.cpp',
        'src/includes/cef_loop.cpp',
        'src/includes/cef_scheme_handler.cpp',
        'src/includes/cef_sync_handler.cpp',
        'src/includes/util.cpp',
        'src/includes/client_app.cpp',
        'src/includes/client_renderer.cpp',
        'src/includes/client_app_delegates.cpp'
      ],
      'dependencies': [
        #'./deps/cef/dll_wrapper.gyp:libcef_dll_wrapper',
        './deps/node/node.gyp:node',
        '../cef/cef.gyp:libcef',
      ],
      'include_dirs': [
        '../',
        './src',
        '../cef/',
        './deps/node/src',
        './deps/node/deps/uv/src/ares',
        './deps/node/deps/uv/include',
        './deps/node/deps/v8/include',
        './deps/node/deps/openssl/openssl/include/openssl',
        './deps/node/deps/http_parser',
        './deps/node/deps/zlib',
      ],
      'cflags': [
        #'-fPIC',
        #'-Wall',
        #'-std=c++0x',
        '-fvisibility=default',
      ],
      'conditions': [
        ['OS=="mac"', {
          'sources': [
            'src/native_window/native_window_mac.mm',
            'src/native_menu/native_menu_mac.mm',
            'src/native_status_icon/native_status_icon_mac.mm'
          ],
          'defines': [
            '__MAC__',
          ],
          'cflags': [ '-m32' ],
          'ldflags': [ '-m32' ],
          'xcode_settings': {
            'OTHER_CFLAGS': ['-ObjC++'],
            'OTHER_LDFLAGS':['-Xlinker -rpath -Xlinker @loader_path/../../../../appjs-darwin/libs/'],
            'ARCHS': [ 'i386' ]
          },
          'link_settings': {
            'libraries': [
               '<(module_root_dir)/deps/cef/Release/lib.target/libcef.dylib',
               '<(module_root_dir)/build/Release/cef_dll_wrapper.a',
               '-lobjc'
             ]
          }
        }],
        ['OS=="linux"', {
          'sources': [
            'src/native_window/native_window_linux.cpp',
            'src/native_menu/native_menu_linux.cpp',
            'src/native_status_icon/native_status_icon_linux.cpp'
          ],
          'defines': [
            '__LINUX__',
            '<!@(uname -a | grep "Ubuntu" > /dev/null && echo "__UBUNTU__" || echo "__NOTUBUNTU__")'
          ],
          'cflags': [
            '<!@(pkg-config --cflags gtk+-2.0 gthread-2.0)',
          ],
          'link_settings': {
              'ldflags': [
                '<!@(pkg-config --libs-only-L --libs-only-other gtk+-2.0 gthread-2.0)',
                #'-Wl,-R,\'$$ORIGIN/../../../../appjs-linux-<(target_arch)/libs/\'',
              ],
              'libraries': [
                '<!@(pkg-config --libs-only-l gtk+-2.0 gthread-2.0)',
                #'<(module_root_dir)/deps/cef/Release/lib.target/libcef.so',
                #'<(module_root_dir)/build/Release/obj.target/deps/cef/cef_dll_wrapper.node'
                'obj/cef/libcef_static.a',
                'obj/cef/libcef.a'
              ],
          }
        }],
        ['OS=="win"', {
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
          'sources': [
            'src/includes/util_win.cpp',
            'src/native_window/native_window_win.cpp',
            'src/native_menu/native_menu_win.cpp',
            'src/native_status_icon/native_status_icon_win.cpp'
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
              '<(module_root_dir)/deps/cef/lib/Release/libcef.lib',
              '<(module_root_dir)/build/Release/lib/libcef_dll_wrapper.node'
            ],
          },
        }]
      ]
    }
  ]
}
