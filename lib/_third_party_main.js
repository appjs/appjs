var Module = require('module');
process.argv[1] = './app.js';
process.nextTick(Module.runMain);

