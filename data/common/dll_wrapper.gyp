# Copyright (c) 2011 The Chromium Embedded Framework Authors. All rights
# reserved. Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file.
#
# Modified by AppJS
{
  'variables': {
    'chromium_code': 1,
    'conditions': [
      [ 'OS=="mac"', {
        # Don't use clang with CEF binary releases due to Chromium tree structure dependency.
        'clang': 0,
      }]
    ]
  },
  'includes': [
    # Bring in the source file lists for cefclient.
    'cef_paths2.gypi',
  ],
  'targets': [
    {
      'target_name': 'libcef_dll_wrapper',
      'type': 'static_library',
      'msvs_guid': 'A9D6DC71-C0DC-4549-AEA0-3B15B44E86A9',
      'defines': [
        'USING_CEF_SHARED',
      ],
      'include_dirs': [
        '.',
      ],
      'sources': [
        '<@(includes_common)',
        '<@(includes_capi)',
        '<@(includes_wrapper)',
        '<@(libcef_dll_wrapper_sources_common)',
      ],
      'conditions': [
        ['OS=="mac"', {
          'xcode_settings': {
            'ARCHS': [ 'i386' ]
          },
        }],
        ['OS=="linux"', {
        	'cflags': [
        		'<!@(pkg-config --cflags gtk+-2.0)',
        	],
          'link_settings': {
	            'ldflags': [
	              '<!@(pkg-config --libs-only-L --libs-only-other gtk+-2.0 gthread-2.0)',
	            ],
	            'libraries': [
	              '<!@(pkg-config --libs-only-l gtk+-2.0 gthread-2.0)',
	            ],
	        },
        }],
        ['OS=="win"', {
          'defines': [
            'NOMINMAX'
          ]
        }]
      ],
      'xcode_settings': {
        # Target build path.
        'SYMROOT': 'xcodebuild',
      },
    },
  ]
}
