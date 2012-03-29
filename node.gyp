{
  'variables': {
    'v8_use_snapshot%': 'true',
    # Turn off -Werror in V8
    # See http://codereview.chromium.org/8159015
    'werror': '',
    'node_use_dtrace': 'false',
    'node_shared_v8%': 'false',
    'node_shared_zlib%': 'false',
    'node_use_openssl%': 'true',
    'node_use_system_openssl%': 'false',
    'library_files': [
      'deps/node/src/node.js',
      'lib/_third_party_main.js',
      'deps/node/lib/_debugger.js',
      'deps/node/lib/_linklist.js',
      'deps/node/lib/assert.js',
      'deps/node/lib/buffer.js',
      'deps/node/lib/buffer_ieee754.js',
      'deps/node/lib/child_process.js',
      'deps/node/lib/console.js',
      'deps/node/lib/constants.js',
      'deps/node/lib/crypto.js',
      'deps/node/lib/cluster.js',
      'deps/node/lib/dgram.js',
      'deps/node/lib/dns.js',
      'deps/node/lib/events.js',
      'deps/node/lib/freelist.js',
      'deps/node/lib/fs.js',
      'lib/zip/adm_zip.js',
      'lib/zip/adm_zip_util.js',
      'lib/zip/adm_zip_util_constants.js',
      'lib/zip/adm_zip_util_errors.js',
      'lib/zip/adm_zip_util_utils.js',
      'lib/zip/adm_zip_entry.js',
      'lib/zip/adm_zip_file.js',
      'lib/zip/adm_zip_headers.js',
      'lib/zip/adm_zip_headers_data.js',
      'lib/zip/adm_zip_headers_entry.js',
      'lib/zip/adm_zip_headers_main.js',
      'lib/zip/adm_zip_methods.js',
      'lib/zip/adm_zip_methods_inflater.js',
      'lib/zip/adm_zip_methods_deflater.js',
      'lib/archive.js',
      'lib/vfs.js',
      'deps/node/lib/http.js',
      'deps/node/lib/https.js',
      'lib/module.js',
      'deps/node/lib/net.js',
      'deps/node/lib/os.js',
      'deps/node/lib/path.js',
      'lib/vpath.js',
      'deps/node/lib/punycode.js',
      'deps/node/lib/querystring.js',
      'deps/node/lib/readline.js',
      'deps/node/lib/repl.js',
      'deps/node/lib/stream.js',
      'deps/node/lib/string_decoder.js',
      'deps/node/lib/sys.js',
      'deps/node/lib/timers.js',
      'deps/node/lib/tls.js',
      'deps/node/lib/tty.js',
      'deps/node/lib/url.js',
      'deps/node/lib/util.js',
      'deps/node/lib/vm.js',
      'deps/node/lib/zlib.js',
    ],
  },

  'targets': [
    {
      'target_name': 'node',
      'type': 'executable',

      'dependencies': [
        'deps/node/deps/http_parser/http_parser.gyp:http_parser',
        'deps/node/deps/uv/uv.gyp:uv',
        'node_js2c#host',
      ],

      'include_dirs': [
        'deps/node/src',
        'deps/node/deps/uv/src/ares',
        '<(SHARED_INTERMEDIATE_DIR)' # for node_natives.h
      ],

      'sources': [
        'deps/node/src/fs_event_wrap.cc',
        'deps/node/src/cares_wrap.cc',
        'deps/node/src/handle_wrap.cc',
        'deps/node/src/node.cc',
        'deps/node/src/node_buffer.cc',
        'deps/node/src/node_constants.cc',
        'deps/node/src/node_extensions.cc',
        'deps/node/src/node_file.cc',
        'deps/node/src/node_http_parser.cc',
        'deps/node/src/node_javascript.cc',
        'deps/node/src/node_main.cc',
        'deps/node/src/node_os.cc',
        'deps/node/src/node_script.cc',
        'deps/node/src/node_string.cc',
        'deps/node/src/node_zlib.cc',
        'deps/node/src/pipe_wrap.cc',
        'deps/node/src/stream_wrap.cc',
        'deps/node/src/tcp_wrap.cc',
        'deps/node/src/timer_wrap.cc',
        'deps/node/src/tty_wrap.cc',
        'deps/node/src/process_wrap.cc',
        'deps/node/src/v8_typed_array.cc',
        'deps/node/src/udp_wrap.cc',
        # headers to make for a more pleasant IDE experience
        'deps/node/src/handle_wrap.h',
        'deps/node/src/node.h',
        'deps/node/src/node_buffer.h',
        'deps/node/src/node_constants.h',
        'deps/node/src/node_crypto.h',
        'deps/node/src/node_extensions.h',
        'deps/node/src/node_file.h',
        'deps/node/src/node_http_parser.h',
        'deps/node/src/node_javascript.h',
        'deps/node/src/node_os.h',
        'deps/node/src/node_root_certs.h',
        'deps/node/src/node_script.h',
        'deps/node/src/node_string.h',
        'deps/node/src/node_version.h',
        'deps/node/src/pipe_wrap.h',
        'deps/node/src/req_wrap.h',
        'deps/node/src/stream_wrap.h',
        'deps/node/src/v8_typed_array.h',
        'deps/node/deps/http_parser/http_parser.h',
        '<(SHARED_INTERMEDIATE_DIR)/node_natives.h',
        # javascript files to make for an even more pleasant IDE experience
        '<@(library_files)',
        # node.gyp is added to the project by default.
        'deps/node/common.gypi',
      ],

      'defines': [
        'NODE_WANT_INTERNALS=1',
        'ARCH="<(target_arch)"',
        'PLATFORM="<(OS)"',
      ],

      'conditions': [
        [ 'node_use_openssl=="true"', {
          'defines': [ 'HAVE_OPENSSL=1' ],
          'sources': [ 'deps/node/src/node_crypto.cc' ],
          'conditions': [
            [ 'node_use_system_openssl=="false"', {
              'dependencies': [ './deps/node/deps/openssl/openssl.gyp:openssl' ],
            }]]
        }, {
          'defines': [ 'HAVE_OPENSSL=0' ]
        }],

        [ 'node_use_dtrace=="true"', {
          'sources': [
            'deps/node/src/node_dtrace.cc',
            'deps/node/src/node_dtrace.h',
            # why does node_provider.h get generated into src and not
            # SHARED_INTERMEDIATE_DIR?
            'deps/node/src/node_provider.h',
          ],
        }],

        [ 'node_shared_v8=="true"', {
          'sources': [
            '<(node_shared_v8_includes)/v8.h',
            '<(node_shared_v8_includes)/v8-debug.h',
          ],
        }, {
          'sources': [
            'deps/node/deps/v8/include/v8.h',
            'deps/node/deps/v8/include/v8-debug.h',
          ],
          'dependencies': [ 'deps/node/deps/v8/tools/gyp/v8.gyp:v8' ],
        }],

        [ 'node_shared_zlib=="false"', {
          'dependencies': [ 'deps/node/deps/zlib/zlib.gyp:zlib' ],
        }],

        [ 'OS=="win"', {
          'sources': [
            'deps/node/tools/msvs/res/node.rc',
          ],
          'defines': [
            'FD_SETSIZE=1024',
            # we need to use node's preferred "win32" rather than gyp's preferred "win"
            'PLATFORM="win32"',
            '_UNICODE=1',
          ],
          'libraries': [ '-lpsapi.lib' ]
        },{ # POSIX
          'defines': [ '__POSIX__' ],
          'sources': [
            'deps/node/src/node_signal_watcher.cc',
            'deps/node/src/node_stat_watcher.cc',
            'deps/node/src/node_io_watcher.cc',
          ]
        }],
        [ 'OS=="mac"', {
          'libraries': [ '-framework Carbon' ],
          'defines!': [
            'PLATFORM="mac"',
          ],
          'defines': [
            # we need to use node's preferred "darwin" rather than gyp's preferred "mac"
            'PLATFORM="darwin"',
          ],
        }],
        [ 'OS=="freebsd"', {
          'libraries': [
            '-lutil',
            '-lkvm',
          ],
        }],
        [ 'OS=="solaris"', {
          'libraries': [
            '-lkstat',
          ],
        }],
      ],
      'msvs-settings': {
        'VCLinkerTool': {
          'SubSystem': 1, # /subsystem:console
        },
      },
    },

    {
      'target_name': 'node_js2c',
      'type': 'none',
      'toolsets': ['host'],
      'actions': [
        {
          'action_name': 'node_js2c',

          'inputs': [
            '<@(library_files)',
            './deps/node/config.gypi',
          ],

          'outputs': [
            '<(SHARED_INTERMEDIATE_DIR)/node_natives.h',
          ],

          # FIXME can the following conditions be shorted by just setting
          # macros.py into some variable which then gets included in the
          # action?

          'conditions': [
            [ 'node_use_dtrace=="true"', {
              'action': [
                'python',
                'deps/node/tools/js2c.py',
                '<@(_outputs)',
                '<@(_inputs)',
              ],
            }, { # No Dtrace
              'action': [
                'python',
                'deps/node/tools/js2c.py',
                '<@(_outputs)',
                '<@(_inputs)',
                'deps/node/src/macros.py'
              ],
            }]
          ],
        },
      ],
    }, # end node_js2c
  ] # end targets
}

