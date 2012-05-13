{
  'targets': [
    {
      'target_name': 'appjs',
      'sources': [ 
      	'src/main.cpp',
      	'src/appjs.cpp',
      	'src/appjs_app.cpp',
      	'src/appjs_window.cpp',
      	'src/includes/cef_handler.cpp',
        'src/includes/cef.cpp',
      	'src/includes/cef_loop.cpp',
        'src/includes/cef_scheme_handler.cpp',
        'src/includes/util.cpp',
      ],
      'dependencies': [
      	'<(module_root_dir)/deps/cef/dll_wrapper.gyp:libcef_dll_wrapper'
      ],
      'include_dirs': [
        'src/',
        'deps/cef'
      ],
  	  'cflags': [
  	  	'-fPIC',
  	  	'-Wall',
  	  	'-std=c++0x'
  	  ],
      'conditions': [
        ['OS=="mac"', {

        }],
        ['OS=="linux"', {
        	'sources': [
        		'src/includes/cef_base_gtk.cpp',
            'src/includes/cef_handler_gtk.cpp',
        		'src/linux/mainwindow.cpp',
        	],
        	'defines': [
  		      '__LINUX__',
            '<!@(dpkg-vendor --is Ubuntu && echo __UBUNTU__ || echo __NOT_UBUNTU__)'
  		    ],
        	'cflags': [
        		'<!@(pkg-config --cflags gtk+-2.0)',
        	],
          'link_settings': {
	            'ldflags': [
	              '<!@(pkg-config --libs-only-L --libs-only-other gtk+-2.0 gthread-2.0)',
                '-Wl,-R,<(module_root_dir)/deps/cef/Release/lib.target/'
	            ],
	            'libraries': [
	              '<!@(pkg-config --libs-only-l gtk+-2.0 gthread-2.0)',
	              '<(module_root_dir)/deps/cef/Release/lib.target/libcef.so',
                '<(module_root_dir)/build/Release/obj.target/deps/cef/cef_dll_wrapper.node'
	            ],
	        },
        }],
        ['OS=="win"', {
        	'sources': [
        		'src/includes/cef_base_win.cpp',
        		'src/windows/mainwindow.cpp'
        	],
        	'defines': [
		        '__WIN__',
            'NOMINMAX',
            '_WINSOCKAPI_'
		      ],
          'link_settings': {
	            'libraries': [
                '<(module_root_dir)/deps/cef/Release/lib.target/libcef.lib',
                '<(module_root_dir)/build/Release/obj.target/deps/cef/cef_dll_wrapper.node'
	            ],
	        },
        }]
      ]
    }
  ]
}
