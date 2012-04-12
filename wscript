import os

def set_options(opt):
  opt.tool_options('compiler_cxx')

def configure(conf):
  conf.check_tool('compiler_cxx')
  conf.check_tool('node_addon')
  conf.check_cfg(atleast_pkgconfig_version = '0.0.0')
  conf.check_cfg(package='gtk+-2.0', args='--cflags --libs',
    uselib_store='GTK')  

def build(bld):
  bld.env.append_value('LINKFLAGS',['-l:/data/work/develop/experiments/chromium/src/out/Debug/obj.target/cef/libcef_dll_wrapper.a','-l:/data/work/develop/experiments/chromium/src/out/Debug/obj.target/cef/libcef.so'])
  obj = bld.new_task_gen('cxx', 'shlib', 'node_addon')
  obj.cxxflags = ['-g','-pthread','-Wl,-z,noexecstack','-fPIC','-Wl,--threads','-Wl,--thread-count=4','-Wl,--icf=none','-Wl,-rpath=/data/work/develop/experiments/chromium/src/out/Debug/lib.target/','-Wl,-rpath-link=/data/work/develop/experiments/chromium/src/out/Debug/lib.target', '-Wall', '-std=c++0x','-L/data/work/develop/experiments/chromium/src/out/Debug/lib.target','-I/data/work/develop/experiments/chromium/src/cef','-lcef']
  obj.target = 'appjs'
  obj.source = bld.glob("src/*.cpp")
  obj.source += bld.glob("src/linux/*.cpp")
  obj.source += bld.glob("src/cef/*.cpp")
  obj.includes = ["src/","/data/work/develop/experiments/chromium/src/cef"]
  obj.uselib = 'GTK'
