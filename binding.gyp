{
  'targets': [
    {
      'target_name': 'appjs',
      'sources': [ 
      	'src/main.cpp',
      	'src/appjs.cpp',
      	'src/appjs_app.cpp',
      	'src/appjs_window.cpp',
      	'src/appjs_util.cpp',
      	'src/cef/client_handler.cpp',
      	'src/cef/impl_cefloop.cpp',
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
	  	'-std=c++0x',
	  ],
      'conditions': [
        ['OS=="mac"', {

        }],
        ['OS=="linux"', {
        	'sources': [
        		'src/linux/cef.cpp',
        		'src/linux/mainwindow.cpp'
        	],
        	'defines': [
		        '__LINUX__',
		    ],
        	'cflags': [
        		'<!@(pkg-config --cflags gtk+-2.0)',
        	],
			'link_settings': {
	            'ldflags': [
	              '<!@(pkg-config --libs-only-L --libs-only-other gtk+-2.0 gthread-2.0)',
	            ],
	            'libraries': [
	              '<!@(pkg-config --libs-only-l gtk+-2.0 gthread-2.0)',
	              '<(module_root_dir)/deps/cef/Debug/lib.target/libcef.so',
	      		  '<(module_root_dir)/build/Release/obj.target/deps/cef/cef_dll_wrapper.node'
	            ],
	        },
        }],
        ['OS=="win"', {
        	'sources': [
        		'src/windows/cef.cpp',
        		'src/windows/mainwindow.cpp'
        	],
        	'defines': [
		        '__WIN__',
		    ],
			'link_settings': {
	            'libraries': [
	              '<(module_root_dir)/deps/cef/Debug/lib.target/libcef.lib',
	      		  '<(module_root_dir)/build/Release/obj.target/deps/cef/cef_dll_wrapper.node'
	            ],
	        },
        }]
      ]
    }
  ]
}
