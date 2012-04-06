import os

def set_options(opt):
  opt.tool_options('compiler_cxx')

def configure(conf):
  conf.check_tool('compiler_cxx')
  conf.check_tool('node_addon')
  conf.check_cfg(atleast_pkgconfig_version = '0.0.0')
  conf.check_cfg(package='gtk+-3.0', args='--cflags --libs',
    uselib_store='GTK')
  conf.check_cfg(package='webkitgtk-3.0', args='--cflags --libs',
    uselib_store='GTK')

def build(bld):
  obj = bld.new_task_gen('cxx', 'shlib', 'node_addon')
  obj.cxxflags = ['-g', '-Wall', '-std=c++0x']
  obj.target = 'appjs'
  obj.source = bld.glob("src/*.cc")
  obj.uselib = 'GTK'
