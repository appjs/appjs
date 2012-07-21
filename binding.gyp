{
  'targets': [
    {
      'target_name': 'wrap',
      'type':'none',
      'dependencies': [
        'publish'
      ],
      'copies':[
        {
          'destination': '<(module_root_dir)/app/data/node_modules/',
          'files': [

          ]
        }
      ]
    },
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
            '<(module_root_dir)/examples/',
            '<(module_root_dir)/lib/',
            '<(module_root_dir)/data/common/appjs/package.json',
          ]
        },
        {
          'destination': '<(module_root_dir)/app/data/node_modules/appjs/node_modules/',
          'files': [
            '<(module_root_dir)/node_modules/mime/',
          ]
        },
        {
          'destination': '<(module_root_dir)/app/data/',
          'files': [
            '<(module_root_dir)/data/common/content/',
            '<(module_root_dir)/data/common/app.js'
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
              'destination': '<(module_root_dir)/app/data/node_modules/appjs-mac/build/Release',
              'files': [
                '<(module_root_dir)/deps/cef/Release/lib.target/libcef.dylib',
                '<(module_root_dir)/deps/cef/Release/lib.target/ffmpegsumo.so',
                '<(PRODUCT_DIR)/appjs.node'
              ],
            },
            {
              'destination': '<(module_root_dir)/app/data/node_modules/appjs-mac',
              'files': [
                '<(module_root_dir)/data/mac/package.json',
                '<(module_root_dir)/data/mac/index.js',
                '<(module_root_dir)/data/mac/README.md'
              ],
            },
            {
              'destination': '<(module_root_dir)/app/data/bin/Contents/',
              'files': [
                '<(module_root_dir)/deps/cef/Resources/'
              ]
            },
            {
              'destination': '<(module_root_dir)/app/data/node_modules/appjs-mac/data/pak',
              'files': [
                '<(module_root_dir)/deps/cef/Resources/chrome.pak',
              ]
            },
            {
              'destination': '<(module_root_dir)/data/pak',
              'files': [
                '<(module_root_dir)/deps/cef/Resources/chrome.pak',
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
        },{ # !mac
          'copies': [
            {
              'destination': '<(module_root_dir)/app/data/node_modules/appjs-<(OS)/data/pak',
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
        ['OS=="linux"', {
          'copies': [
            {
              'destination': '<(module_root_dir)/build/Release/',
              'files': [
                '<(module_root_dir)/deps/cef/Release/lib.target/libcef.so',
              ]
            },
            {
              'destination': '<(module_root_dir)/app/data/node_modules/appjs-linux/build/Release/',
              'files': [
                '<(module_root_dir)/deps/cef/Release/lib.target/libcef.so',
                '<(PRODUCT_DIR)/appjs.node'
              ],
            },
            {
              'destination': '<(module_root_dir)/app/data/node_modules/appjs-linux/',
              'files': [
                '<(module_root_dir)/data/linux/package.json',
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
                '<(module_root_dir)/data/linux/node-bin/<!@(node -e "console.log(require(\'os\').arch())")/node',
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
              'destination': '<(module_root_dir)/app/data/node_modules/appjs-win/build/Release/',
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
                '<(PRODUCT_DIR)/appjs.node'
              ],
            },
            {
              'destination': '<(module_root_dir)/app/data/node_modules/appjs-win/',
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
        'src/base/native_window.cpp',
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
            'src/includes/cef_base_mac.mm',
            'src/includes/cef_handler_mac.mm',
            'src/mac/native_window_mac.mm'
          ],
          'defines': [
            '__MAC__',
          ],
          'xcode_settings': {
            'OTHER_LDFLAGS':['-Xlinker -rpath -Xlinker @loader_path/']
          },
          'link_settings': {
            'libraries': [
               '<(module_root_dir)/deps/cef/Release/lib.target/libcef.dylib',
               '<(module_root_dir)/build/Release/cef_dll_wrapper.node'
             ]
          }
        }],
        ['OS=="linux"', {
          'sources': [
            'src/includes/cef_base_gtk.cpp',
            'src/includes/cef_handler_gtk.cpp',
            'src/linux/native_window_linux.cpp',
          ],
          'defines': [
            '__LINUX__',
            '<!@(uname -a | grep "Ubuntu" > /dev/null && echo "__UBUNTU__" || echo "__NOTUBUNTU__")'
          ],
          'cflags': [
            '<!@(pkg-config --cflags gtk+-2.0)',
          ],
          'link_settings': {
              'ldflags': [
                '<!@(pkg-config --libs-only-L --libs-only-other gtk+-2.0 gthread-2.0)',
                '-Wl,-R,\'$$ORIGIN/\'',
              ],
              'libraries': [
                '<!@(pkg-config --libs-only-l gtk+-2.0 gthread-2.0)',
                '<(module_root_dir)/deps/cef/Release/lib.target/libcef.so',
                '<(module_root_dir)/build/Release/obj.target/deps/cef/cef_dll_wrapper.node'
              ],
          }
        }],
        ['OS=="win"', {
          'sources': [
            'src/includes/cef_base_win.cpp',
            'src/includes/cef_handler_win.cpp',
            'src/windows/native_window_win.cpp'
          ],
          'defines': [
            '__WIN__',
            '_WINSOCKAPI_'
          ],
          'link_settings': {
              'libraries': [
                'GdiPlus.lib',
                '<(module_root_dir)/deps/cef/lib/Release/libcef.lib',
                '<(module_root_dir)/build/Release/lib/libcef_dll_wrapper.node'
              ],
          },
        }]
      ]
    }
  ]
}
