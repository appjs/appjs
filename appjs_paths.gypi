{
  'variables': {
    'common_dependencies': [
      './deps/node/node.gyp:node',
      '../cef/cef.gyp:libcef',
    ],
    'common_sources': [
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
    'common_include_dirs': [
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
    'win_sources': [
      'src/includes/util_win.cpp',
      'src/native_window/native_window_win.cpp',
      'src/native_menu/native_menu_win.cpp',
      'src/native_status_icon/native_status_icon_win.cpp'
    ],
    'linux_sources': [
      'src/native_window/native_window_linux.cpp',
      'src/native_menu/native_menu_linux.cpp',
      'src/native_status_icon/native_status_icon_linux.cpp'
    ],
    'mac_sources': [
      'src/native_window/native_window_mac.mm',
      'src/native_menu/native_menu_mac.mm',
      'src/native_status_icon/native_status_icon_mac.mm'
    ]
  }
}