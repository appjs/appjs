@echo off

cd %~dp0

if /i "%1"=="help" goto help
if /i "%1"=="--help" goto help
if /i "%1"=="-help" goto help
if /i "%1"=="/help" goto help
if /i "%1"=="?" goto help
if /i "%1"=="-?" goto help
if /i "%1"=="--?" goto help
if /i "%1"=="/?" goto help

@rem Process arguments.
set config=Release
set target=Build
set target_arch=ia32
set debug_arg=
set nosnapshot_arg=
set noprojgen=
set nobuild=
set nosign=
set nosnapshot=
set test=
set test_args=
set msi=
set upload=
set jslint=

:next-arg
if "%1"=="" goto args-done
if /i "%1"=="debug"         set config=Debug&goto arg-ok
if /i "%1"=="release"       set config=Release&goto arg-ok
if /i "%1"=="clean"         set target=Clean&goto arg-ok
if /i "%1"=="ia32"          set target_arch=ia32&goto arg-ok
if /i "%1"=="x86"           set target_arch=ia32&goto arg-ok
if /i "%1"=="x64"           set target_arch=x64&goto arg-ok
if /i "%1"=="noprojgen"     set noprojgen=1&goto arg-ok
if /i "%1"=="nobuild"       set nobuild=1&goto arg-ok
if /i "%1"=="nosign"        set nosign=1&goto arg-ok
if /i "%1"=="nosnapshot"    set nosnapshot=1&goto arg-ok
if /i "%1"=="test-uv"       set test=test-uv&goto arg-ok
if /i "%1"=="test-internet" set test=test-internet&goto arg-ok
if /i "%1"=="test-pummel"   set test=test-pummel&goto arg-ok
if /i "%1"=="test-simple"   set test=test-simple&goto arg-ok
if /i "%1"=="test-message"  set test=test-message&goto arg-ok
if /i "%1"=="test-all"      set test=test-all&goto arg-ok
if /i "%1"=="test"          set test=test&goto arg-ok
if /i "%1"=="msi"           set msi=1&goto arg-ok
if /i "%1"=="upload"        set upload=1&goto arg-ok
if /i "%1"=="jslint"        set jslint=1&goto arg-ok

echo Warning: ignoring invalid command line option `%1`.

:arg-ok
shift
goto next-arg
:args-done
if defined upload goto upload
if defined jslint goto jslint

if "%config%"=="Debug" set debug_arg=--debug
if defined nosnapshot set nosnapshot_arg=--without-snapshot

:project-gen
@rem Skip project generation if requested.
if defined noprojgen goto msbuild

@rem Generate the VS project.
python configure %debug_arg% %nosnapshot_arg% --dest-cpu=%target_arch%
if errorlevel 1 goto create-msvs-files-failed
if not exist node.sln goto create-msvs-files-failed
echo Project files generated.

:msbuild
@rem Skip project generation if requested.
if defined nobuild goto msi

@rem Bail out early if not running in VS build env.
if defined VCINSTALLDIR goto msbuild-found
if not defined VS100COMNTOOLS goto msbuild-not-found
if not exist "%VS100COMNTOOLS%\..\..\vc\vcvarsall.bat" goto msbuild-not-found
call "%VS100COMNTOOLS%\..\..\vc\vcvarsall.bat"
if not defined VCINSTALLDIR goto msbuild-not-found
goto msbuild-found

:msbuild-not-found
echo Build skipped. To build, this file needs to run from VS cmd prompt.
goto run

:msbuild-found
@rem Build the sln with msbuild.
msbuild node.sln /m /t:%target% /p:Configuration=%config% /clp:NoSummary;NoItemAndPropertyList;Verbosity=minimal /nologo
if errorlevel 1 goto exit

if defined nosign goto msi
signtool sign /a Release\node.exe

:msi
@rem Skip msi generation if not requested
if not defined msi goto run
python "%~dp0tools\deps\node\getnodeversion.py" > "%temp%node_version.txt"
if not errorlevel 0 echo Cannot determine current version of node.js & goto exit
for /F "tokens=*" %%i in (%temp%\node_version.txt) do set NODE_VERSION=%%i
heat dir deps\node\deps\npm -var var.NPMSourceDir -dr NodeModulesFolder -cg NPMFiles -gg -template fragment -nologo -out npm.wxs
msbuild "%~dp0tools\deps\node\msvs\msi\nodemsi.sln" /m /t:Clean,Build /p:Configuration=%config% /p:NodeVersion=%NODE_VERSION% /clp:NoSummary;NoItemAndPropertyList;Verbosity=minimal /nologo
if errorlevel 1 goto exit

if defined nosign goto run
signtool sign /a Release\node.msi

:run
@rem Run tests if requested.
if "%test%"=="" goto exit

if "%config%"=="Debug" set test_args=--mode=debug
if "%config%"=="Release" set test_args=--mode=release

if "%test%"=="test" set test_args=%test_args% simple message
if "%test%"=="test-internet" set test_args=%test_args% internet
if "%test%"=="test-pummel" set test_args=%test_args% pummel
if "%test%"=="test-simple" set test_args=%test_args% simple
if "%test%"=="test-message" set test_args=%test_args% message
if "%test%"=="test-all" set test_args=%test_args%

echo running 'python deps/node/tools/test.py %test_args%'
python deps/node/tools/test.py %test_args%

if "%test%"=="test" goto jslint

goto exit

:create-msvs-files-failed
echo Failed to create vc project files. 
goto exit

:upload
echo uploading .exe .msi .pdb to nodejs.org
python "%~dp0tools\deps\node\getnodeversion.py" > "%temp%\deps\node\node_version.txt"
if not errorlevel 0 echo Cannot determine current version of node.js & goto exit
for /F "tokens=*" %%i in (%temp%\node_version.txt) do set NODE_VERSION=%%i
@echo on
ssh node@nodejs.org mkdir -p web/nodejs.org/dist/v%NODE_VERSION%
scp Release\node.msi node@nodejs.org:~/web/nodejs.org/dist/v%NODE_VERSION%/node-v%NODE_VERSION%.msi
scp Release\node.exe node@nodejs.org:~/web/nodejs.org/dist/v%NODE_VERSION%/node.exe
scp Release\node.pdb node@nodejs.org:~/web/nodejs.org/dist/v%NODE_VERSION%/node.pdb
@echo off
goto exit

:jslint
echo running jslint
set PYTHONPATH=deps/node/tools/closure_linter/
python deps/node/tools/closure_linter/closure_linter/gjslint.py --unix_mode --strict --nojsdoc -r deps/node/lib/ -r deps/node/src/ -r deps/node/test/ --exclude_files deps/node/lib/punycode.js
goto exit

:help
echo vcbuild.bat [debug/release] [msi] [deps/node/test-all/test-uv/test-internet/test-pummel/test-simple/test-message] [clean] [noprojgen] [nobuild] [nosign]
echo Examples:
echo   vcbuild.bat                : builds release build
echo   vcbuild.bat debug          : builds debug build
echo   vcbuild.bat release msi    : builds release build and MSI installer package
echo   vcbuild.bat test           : builds debug build and runs tests
goto exit

:exit
