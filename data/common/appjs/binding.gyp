{
  'targets': [
    {
      'target_name': 'add-dependencies',
      'type':'none',
      'actions':[
        {
          'action_name':'npm',
          'inputs':[''],
          'outputs':[''],
          'action':['npm','install','appjs-<(OS)']
        },
      ]
    }
  ]
}
