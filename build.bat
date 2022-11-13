@echo off


set backend="opengl"
echo [+] active backend is %backend%


set build_dir=%cd%
pushd ..\build\
echo Builiding everything in %build_dir%
echo --------------------------------

set files=
set files="%build_dir%/math/*.c" %files%
set files="%build_dir%/os/*.c" %files%
set files="%build_dir%/os/windows/*.c" %files%
set files="%build_dir%/utils/*.c" %files%
set files="%build_dir%/memory/*.c" %files%
set files="%build_dir%/base/*.c" %files%

set files="%build_dir%/sound/*.c" %files%
set files="%build_dir%/ui/*.c" %files%
set files="%build_dir%/rs/*.c" %files%
set files="%build_dir%/*.c" %files%

REM For GFX2D abstraction
set files="%build_dir%/gfx/2d/*.c" %files%

if %backend%=="opengl" goto OpenGL
if %backend%=="d3d11" goto D3D11




:CompileStart
echo Compiling %files%
echo ----------------------
echo Compilation Start


cl   /MD  -nologo %files%  /I%build_dir%\  /Fe:output.exe     user32.lib gdi32.lib opengl32.lib
echo Compilation End
popd 

exit /b


:OpenGL
	set files="%build_dir%/gfx/opengl/*.c" %files%
	set files="%build_dir%/third-party/glad/*.c" %files%
	goto CompileStart
:D3D11
	set files="%build_dir%/gfx/d3d11/*.c" %files%
	goto CompileStart