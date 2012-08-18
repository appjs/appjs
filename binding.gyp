{
  'variables':{
    'version': '0.0.18'
  },
  'targets': [
    {
      'target_name': 'appjs-sdk',
      'type':'none',
      'dependencies': [
        '<(module_root_dir)/appjs_module.gyp:appjs'
      ],
      'copies':[
        {
          'destination': '<(module_root_dir)/appjs-sdk/dists/<(version)/node_modules/appjs/',
          'files': [
            '<(module_root_dir)/README.md',
            '<(module_root_dir)/package.json',
            '<(module_root_dir)/lib/',
            '<(module_root_dir)/data/common/bin/',
            '<(module_root_dir)/data/common/cli/'
          ]
        },
        {
          'destination': '<(module_root_dir)/appjs-sdk/dists/<(version)/test',
          'files': [
            '<(module_root_dir)/tests/test-client/',
          ]
        },
        {
          'destination': '<(module_root_dir)/appjs-sdk/dists/<(version)/node_modules/',
          'files': [
            '<(module_root_dir)/node_modules/mime/',
            '<(module_root_dir)/node_modules/nopt/'
          ]
        }
      ],
      'conditions': [
        ['OS=="linux"', {
          'copies': [
            {
              'destination': '<(module_root_dir)/appjs-sdk/dists/<(version)/node_modules/appjs/bindings/linux/<(target_arch)/',
              'files': [
                '<(PRODUCT_DIR)/appjs.node'
              ]
            },
            {
              'destination': '<(module_root_dir)/appjs-sdk/dists/<(version)/node_modules/appjs-linux-<(target_arch)/libs/',
              'files': [
                '<(module_root_dir)/deps/cef/Release/lib.target/libcef.so',
              ],
            },
            {
              'destination': '<(module_root_dir)/appjs-sdk/dists/<(version)/node_modules/appjs-linux-<(target_arch)/',
              'files': [
                '<(module_root_dir)/data/linux/<(target_arch)/package.json',
                '<(module_root_dir)/data/linux/index.js',
                '<(module_root_dir)/data/linux/README.md',
              ],
            },
            {
              'destination': '<(module_root_dir)/appjs-sdk/bin/',
              'files': [
                '<(module_root_dir)/data/linux/appjs.sh',
              ]
            },
            {
              'destination': '<(module_root_dir)/appjs-sdk/bin/',
              'files': [
                '<(module_root_dir)/data/linux/<(target_arch)/node-bin/node',
                '<(module_root_dir)/deps/cef/Release/lib.target/libffmpegsumo.so'
              ]
            },
            {
              'destination': '<(module_root_dir)/appjs-sdk/dists/<(version)/node_modules/appjs-linux-<(target_arch)/data/',
              'files': [
                '<(module_root_dir)/deps/cef/Release/chrome.pak',
                '<(module_root_dir)/deps/cef/Release/locales/'
              ]
            }
          ]
        }]
      ]
    },
  ]
}
