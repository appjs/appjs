{
  "conditions": [
    ['OS=="win"', {
      'targets': [{
        'target_name': 'launch',
        'msvs_guid': 'C1D7BDA2-FDAB-4A42-BC9B-43F2EA62166E',
        'type': 'executable',
        'sources': [ 
          'launch.cpp'
        ],
        'include_dirs': [
          'src',
        ],
        'msvs_settings': {
          'VCLinkerTool': {
            'SubSystem': '2', # Set /SUBSYSTEM:WINDOWS
          },
        },
      }]
    }]
  ]
}