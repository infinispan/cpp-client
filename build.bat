if [%generator%] == [] call:usage generator
if [%generator%] == [] call:usage generator
if [%JAVA_HOME_32%] == [] call:usage JAVA_HOME_32
if [%JAVA_HOME_64%] == [] call:usage JAVA_HOME_64
if [%INFINISPAN_VERSION%] == [] call:usage INFINISPAN_VERSION
if [%version.1major%] == [] call:usage version.1major
if [%version.2minor%] == [] call:usage version.2minor
if [%version.3micro%] == [] call:usage version.3micro
if [%version.4qualifier%] == [] call:usage version.4qualifier
if [%SWIG_DIR%] == [] call:usage SWIG_DIR
if [%SWIG_EXECUTABLE%] == [] call:usage SWIG_EXECUTABLE
if [%MVN_PROGRAM%] == [] call:usage MVN_PROGRAM

set JBOSS_HOME=%cd%\infinispan-server-%INFINISPAN_VERSION%
set JBOSS_ZIP=infinispan-server-%INFINISPAN_VERSION%-bin.zip
set JBOSS_URL=http://download.jboss.org/infinispan/%INFINISPAN_VERSION%/%JBOSS_ZIP%

if not exist %JBOSS_HOME% (
  if not exist %JBOSS_ZIP% (
     powershell.exe -Command "(new-object System.Net.WebClient).DownloadFile('%JBOSS_URL%','%JBOSS_ZIP%')"
   )
"C:\Program Files\7-zip\7z.exe" x %JBOSS_ZIP% 
)

call:do_build %generator% 32
call:do_build %generator% 64
goto:eof

:do_build
setlocal
set "arch=%~2"
set "build_dir=build_win%arch%"

if "%arch%" == "32" (
  set "JAVA_HOME=%JAVA_HOME_32%"
  set "full_generator=%~1"
)

if "%arch%" == "64" (
  set "JAVA_HOME=%JAVA_HOME_64%"
  set "full_generator=%~1 Win64"
)

set PATH=%JAVA_HOME%\bin;%PATH%
echo "Using JAVA_HOME=%JAVA_HOME%"

if exist %build_dir% rmdir %build_dir% /s /q
mkdir %build_dir%
cd %build_dir%

cmake -G "%full_generator%" -DCPACK_PACKAGE_VERSION_MAJOR=%version.1major% -DCPACK_PACKAGE_VERSION_MINOR=%version.2minor% -DCPACK_PACKAGE_VERSION_PATCH="%version.3micro%.%version.4qualifier%" -DSWIG_DIR=%SWIG_DIR% -DSWIG_EXECUTABLE=%SWIG_EXECUTABLE% -DMVN_PROGRAM=%MVN_PROGRAM% ..
if %errorlevel% neq 0 goto fail

cmake --build . --config Debug
if %errorlevel% neq 0 goto fail

ctest -V --timeout 3000
if %errorlevel% neq 0 goto fail

cpack -G ZIP -C Debug -DCPACK_PACKAGE_VERSION_MAJOR=%version.1major% -DCPACK_PACKAGE_VERSION_MINOR=%version.2minor% -DCPACK_PACKAGE_VERSION_PATCH="%version.3micro%.%version.4qualifier%"
if %errorlevel% neq 0 goto fail

endlocal
goto:eof

:usage
REM @echo "%~1" is null
REM @echo This script needs these env variables:
REM @echo 	generator:		Cmake generator
REM @echo 	JAVA_HOME_32:		Java home 32 bit
REM @echo 	JAVA_HOME_64:		Java home 64 bit
REM @echo	INFINISPAN_VERSION:	Version for the Infinispan server
REM @echo 	version.1major:		Major version
REM @echo 	version.2minor:		Minor
REM @echo 	version.3micro:		Micro
REM @echo 	version.4qualifier:	(Qualifier Final,SNAPSHOT, Beta...)
()
    exit /b 2


:fail
    echo "Failure!"
    ()
    exit /b 1
:eof
