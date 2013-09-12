{
  'targets': [
    {
      'target_name': 'publish',
      'type':'none',
      'dependencies': [
        'appjs'
      ],
      'copies':[
        {
          'destination': '<(module_root_dir)/app/data/node_modules/appjs/',
          'files': [
            '<(module_root_dir)/README.md',
            '<(module_root_dir)/package.json',
            '<(module_root_dir)/lib/',
          ]
        },
        {
          'destination': '<(module_root_dir)/app/data/node_modules/appjs/examples/',
          'files': [
            '<(module_root_dir)/examples/hello-world/',
          ]
        },
        {
          'destination': '<(module_root_dir)/app/data/node_modules/appjs/cli/',
          'files': [
            '<(module_root_dir)/cli/postinstall.js',
          ]
        },
        {
          'destination': '<(module_root_dir)/app/data/node_modules/',
          'files': [
            '<(module_root_dir)/node_modules/mime/',
          ]
        },
        {
          'destination': '<(module_root_dir)/app/data/',
          'files': [
            '<(module_root_dir)/examples/hello-world/content/',
            '<(module_root_dir)/examples/hello-world/app.js'
          ]
        }
      ],
      'conditions': [
        ['OS=="mac"', {
          'copies': [
            {
              'destination': '<(module_root_dir)/build/Release/',
              'files': [
                '<(module_root_dir)/deps/cef/Release/lib.target/libcef.dylib',
              ]
            },
            {
              'destination': '<(module_root_dir)/app/data/node_modules/appjs-darwin/libs/',
              'files': [
                '<(module_root_dir)/deps/cef/Release/lib.target/libcef.dylib',
                '<(module_root_dir)/deps/cef/Release/lib.target/ffmpegsumo.so',
              ],
            },
            {
              'destination': '<(module_root_dir)/app/data/node_modules/appjs/bindings/darwin/ia32/',
              'files': [
                '<(PRODUCT_DIR)/appjs.node'
              ]
            },
            {
              'destination': '<(module_root_dir)/app/data/node_modules/appjs-darwin/',
              'files': [
                '<(module_root_dir)/data/mac/package.json',
                '<(module_root_dir)/data/mac/index.js',
                '<(module_root_dir)/data/mac/README.md'
              ],
            },
            {
              'destination': '<(module_root_dir)/app/data/bin/Contents/',
              'files': [
                '<(module_root_dir)/deps/cef/Release/Resources/'
              ]
            },
            {
              'destination': '<(module_root_dir)/app/data/node_modules/appjs-darwin/data/',
              'files': [
                '<(module_root_dir)/deps/cef/Release/Resources/chrome.pak',
              ]
            },
            {
              'destination': '<(module_root_dir)/data/pak',
              'files': [
                '<(module_root_dir)/deps/cef/Release/Resources/chrome.pak',
              ]
            },
            {
              'destination': '<(module_root_dir)/app/',
              'files': [
                '<(module_root_dir)/data/mac/app.sh',
              ]
            },
            {
              'destination': '<(module_root_dir)/app/data/bin/',
              'files': [
                '<(module_root_dir)/data/mac/node-bin/node/',
              ]
            }
          ]
        }],
        ['OS=="linux"', {
          'copies': [
            {
              'destination': '<(module_root_dir)/build/Release/',
              'files': [
                '<(module_root_dir)/deps/cef/Release/lib.target/libcef.so',
              ]
            },
            {
              'destination': '<(module_root_dir)/app/data/node_modules/appjs/bindings/linux/<(target_arch)/',
              'files': [
                '<(PRODUCT_DIR)/appjs.node'
              ]
            },
            {
              'destination': '<(module_root_dir)/app/data/node_modules/appjs-linux-<(target_arch)/libs/',
              'files': [
                '<(module_root_dir)/deps/cef/Release/lib.target/libcef.so',
              ],
            },
            {
              'destination': '<(module_root_dir)/app/data/node_modules/appjs-linux-<(target_arch)/',
              'files': [
                '<(module_root_dir)/data/linux/<(target_arch)/package.json',
                '<(module_root_dir)/data/linux/index.js',
                '<(module_root_dir)/data/linux/README.md',
              ],
            },
            {
              'destination': '<(module_root_dir)/app/',
              'files': [
                '<(module_root_dir)/data/linux/app.sh',
              ]
            },
            {
              'destination': '<(module_root_dir)/app/data/bin/',
              'files': [
                '<(module_root_dir)/data/linux/<(target_arch)/node-bin/node',
                '<(module_root_dir)/deps/cef/Release/lib.target/libffmpegsumo.so'
              ]
            },
            {
              'destination': '<(module_root_dir)/app/data/node_modules/appjs-linux-<(target_arch)/data/',
              'files': [
                '<(module_root_dir)/deps/cef/Release/chrome.pak',
                '<(module_root_dir)/deps/cef/Release/locales/'
              ]
            },
            {
              'destination': '<(module_root_dir)/data/pak',
              'files': [
                '<(module_root_dir)/deps/cef/Release/chrome.pak',
                '<(module_root_dir)/deps/cef/Release/locales/'
              ]
            }
          ]
        }],
        ['OS=="win"', {
          'copies': [
            {
              'destination': '<(module_root_dir)/build/Release/',
              'files': [
                '<(module_root_dir)/deps/cef/Release/libcef.dll',
                '<(module_root_dir)/deps/cef/Release/avcodec-54.dll',
                '<(module_root_dir)/deps/cef/Release/avformat-54.dll',
                '<(module_root_dir)/deps/cef/Release/avutil-51.dll',
                '<(module_root_dir)/deps/cef/Release/d3dcompiler_43.dll',
                '<(module_root_dir)/deps/cef/Release/d3dx9_43.dll',
                '<(module_root_dir)/deps/cef/Release/icudt.dll',
                '<(module_root_dir)/deps/cef/Release/libEGL.dll',
                '<(module_root_dir)/deps/cef/Release/libGLESv2.dll'
              ]
            },
            {
              'destination': '<(module_root_dir)/app/data/node_modules/appjs/bindings/win32/ia32/',
              'files': [
                '<(PRODUCT_DIR)/appjs.node'
              ]
            },
            {
              'destination': '<(module_root_dir)/app/data/node_modules/appjs-win32/libs/',
              'files': [
                '<(module_root_dir)/deps/cef/Release/libcef.dll',
                '<(module_root_dir)/deps/cef/Release/avcodec-54.dll',
                '<(module_root_dir)/deps/cef/Release/avformat-54.dll',
                '<(module_root_dir)/deps/cef/Release/avutil-51.dll',
                '<(module_root_dir)/deps/cef/Release/d3dcompiler_43.dll',
                '<(module_root_dir)/deps/cef/Release/d3dx9_43.dll',
                '<(module_root_dir)/deps/cef/Release/icudt.dll',
                '<(module_root_dir)/deps/cef/Release/libEGL.dll',
                '<(module_root_dir)/deps/cef/Release/libGLESv2.dll',
              ],
            },
            {
              'destination': '<(module_root_dir)/app/data/node_modules/appjs-win32/',
              'files': [
                '<(module_root_dir)/data/win/package.json',
                '<(module_root_dir)/data/win/index.js',
                '<(module_root_dir)/data/win/README.md'
              ],
            },
            {
              'destination': '<(module_root_dir)/app/',
              'files': [
                '<(module_root_dir)/data/win/app.exe',
              ]
            },
            {
              'destination': '<(module_root_dir)/app/data/bin/',
              'files': [
                '<(module_root_dir)/data/win/node-bin/node.exe',
              ]
            },
            {
              'destination': '<(module_root_dir)/app/data/node_modules/appjs-win32/data/',
              'files': [
                '<(module_root_dir)/deps/cef/Release/chrome.pak',
                '<(module_root_dir)/deps/cef/Release/locales/'
              ]
            },
            {
              'destination': '<(module_root_dir)/data/pak',
              'files': [
                '<(module_root_dir)/deps/cef/Release/chrome.pak',
                '<(module_root_dir)/deps/cef/Release/locales/'
              ]
            }
          ]
        }]
      ]
    },
    {
      'target_name': 'appjs',
      'msvs_guid': 'A9D6DC71-C0DC-4549-AEB1-3B15B44E86A9',
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
      ],
      'dependencies': [
        '<(module_root_dir)/deps/cef/dll_wrapper.gyp:libcef_dll_wrapper'
      ],
      'include_dirs': [
        'src/',
        'deps/cef/'
      ],
      'cflags': [
        '-fPIC',
        '-Wall',
        '-std=c++0x'
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
               '<(module_root_dir)/build/Release/cef_dll_wrapper.node',
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
                '-Wl,-R,\'$$ORIGIN/../../../../appjs-linux-<(target_arch)/libs/\'',
              ],
              'libraries': [
                '<!@(pkg-config --libs-only-l gtk+-2.0 gthread-2.0)',
                '<(module_root_dir)/deps/cef/Release/lib.target/libcef.so',
                '<(module_root_dir)/build/Release/obj.target/deps/cef/cef_dll_wrapper.node'
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
			  'AdditionalOptions': [ '/EHsc' ]
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
              '<(module_root_dir)/build/Release/lib/libcef_dll_wrapper.lib'
            ],
          },
        }]
      ]
    }
  ]
}
