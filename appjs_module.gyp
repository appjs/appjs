{
  'targets': [
    {
      'target_name': 'appjs',
      'msvs_guid': 'A9D6DC71-C0DC-4549-AEB1-3B15B44E86A9',
      'sources': [
        'src/main.cpp',
        'src/appjs.cpp',
        'src/appjs_app.cpp',
        'src/appjs_window.cpp',
        'src/native_window/native_window.cpp',
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
        '-Wno-c++11-extensions',
        '-std=c++0x'
      ],
      'conditions': [
        ['OS=="mac"', {
          'sources': [
            'src/native_window/native_window_mac.mm'
          ],
          'defines': [
            '__MAC__',
          ],
          'cflags': [ '-m32' ],
          'ldflags': [ '-m32' ],
          'xcode_settings': {
            'OTHER_LDFLAGS':['-Xlinker -rpath -Xlinker @loader_path/../../../../appjs-darwin/libs/'],
            'ARCHS': [ 'i386' ]
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
            'src/native_window/native_window_linux.cpp',
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
          'sources': [
            'src/includes/util_win.cpp',
            'src/native_window/native_window_win.cpp',
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
