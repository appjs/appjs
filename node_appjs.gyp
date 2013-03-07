{
  'includes': [
    #'deps/node/tools/addon.gypi',
    'deps/node/common.gypi',
    'deps/node/config.gypi'
  ],
  'targets': [
    {
      'target_name': 'appjs',
      'type':'static_library',
      #'product_prefix': '',
      #'product_extension': 'node',
      'defines!': ['CONTENT_IMPLEMENTATION'],
      'defines': ['BUILDING_NODE_EXTENSION','__LINUX__','<!@(uname -a | grep "Ubuntu" > /dev/null && echo "__UBUNTU__" || echo "__NOTUBUNTU__")'],
      'variables': {
        'chromium_code': 1,
      },
      'sources': [
        'src/appjs.cpp',
        'src/appjs_app.cpp',
        'src/appjs_window.cpp',
        'src/appjs_menu.cpp',
        'src/appjs_status_icon.cpp',
        'src/native_window/native_window.cpp',
        'src/native_menu/native_menu.cpp',
        'src/native_status_icon/native_status_icon.cpp',
        'src/native_window/native_window_linux.cpp',
        'src/native_menu/native_menu_linux.cpp',
        'src/native_status_icon/native_status_icon_linux.cpp',
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
        './deps/cef/dll_wrapper.gyp:libcef_dll_wrapper',
        '../cef/cef.gyp:libcef',
        #'../base/base.gyp:base',
        #'../content/content.gyp:content_app',
        #'../content/content.gyp:content_browser',
        #'../content/content.gyp:content_common'
      ],
      'include_dirs': [
        '../',
        './src',
        './deps/cef/',
        './deps/node/src',
        './deps/node/deps/uv/src/ares',
        './deps/node/deps/uv/include',
        './deps/node/deps/v8/include',
        './deps/node/deps/openssl/openssl/include/openssl',
        './deps/node/deps/http_parser',
        './deps/node/deps/zlib',
      ],
      'cflags': [
        '<!@(pkg-config --cflags gtk+-2.0 gthread-2.0)',
        '-fvisibility=default',
        #'-fPIC',
        '-Wall',
        '-std=c++0x'
      ],
      'link_settings': {
          'ldflags': [
            '<!@(pkg-config --libs-only-L --libs-only-other gtk+-2.0 gthread-2.0)',
            #'-Wl,-R,\'$$ORIGIN/../../../../appjs-linux-<(target_arch)/libs/\'',
          ],
          'libraries': [
            '<!@(pkg-config --libs-only-l gtk+-2.0 gthread-2.0)',
            #'<(INTERMEDIATE_DIR)/../../../lib/libcef.so',
            #'<(INTERMEDIATE_DIR)/../../cef/libcef_static.a',
            #'<(INTERMEDIATE_DIR)/../../cef/libcef_dll_wrapper.a'
            'obj/cef/libcef_static.a',
            'obj/cef/libcef.a'
          ],
      }
    }
  ]
}
