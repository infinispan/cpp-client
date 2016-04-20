if [%generator%] == [] call:usage generator
if [%generator%] == [] call:usage generator
if [%JAVA_HOME_32%] == [] call:usage JAVA_HOME_32
if [%JAVA_HOME_64%] == [] call:usage JAVA_HOME_64
if [%INFINISPAN_VERSION%] == [] call:usage INFINISPAN_VERSION
rem if [%version.1major%] == [] call:usage version.1major
rem if [%version.2minor%] == [] call:usage version.2minor
rem if [%version.3micro%] == [] call:usage version.3micro
rem if [%version.4qualifier%] == [] call:usage version.4qualifier
if [%SWIG_DIR%] == [] call:usage SWIG_DIR
if [%SWIG_EXECUTABLE%] == [] call:usage SWIG_EXECUTABLE
if [%MVN_PROGRAM%] == [] call:usage MVN_PROGRAM
if [%PROTOBUF_LIBRARY_32%] == [] call:usage PROTOBUF_LIBRARY_32
if [%PROTOBUF_LIBRARY_64%] == [] call:usage PROTOBUF_LIBRARY_64
if [%PROTOBUF_INCLUDE_DIR%] == [] call:usage PROTOBUF_INCLUDE_DIR

set JBOSS_HOME=%cd%\infinispan-server-%INFINISPAN_VERSION%
set JBOSS_ZIP=infinispan-server-%INFINISPAN_VERSION%-bin.zip
set JBOSS_URL=http://download.jboss.org/infinispan/%INFINISPAN_VERSION%/%JBOSS_ZIP%

if not exist %JBOSS_HOME% (
  if not exist %JBOSS_ZIP% (
     powershell.exe -Command "(new-object System.Net.WebClient).DownloadFile('%JBOSS_URL%','%JBOSS_ZIP%')"
   )
"C:\Program Files\7-zip\7z.exe" x %JBOSS_ZIP% 
)
copy test\data\*.* %JBOSS_HOME%\standalone\configuration

call:do_build %generator% 32
call:do_build %generator% 64
goto:eof

:do_build
setlocal
set "arch=%~2"
set "build_dir=build_win%arch%"

if "%arch%" == "32" (
  set "JAVA_HOME=%JAVA_HOME_32%"
  set "PROTOBUF_LIBRARY=%PROTOBUF_LIBRARY_32%"
  set "PROTOBUF_LIBRARY_DLL=%PROTOBUF_LIBRARY_DLL_32%"
  set "OPENSSL_ROOT_DIR=%OPENSSL_ROOT_DIR_32%"
  set "full_generator=%~1"
)

if "%arch%" == "64" (
  set "JAVA_HOME=%JAVA_HOME_64%"
  set "PROTOBUF_LIBRARY=%PROTOBUF_LIBRARY_64%"
  set "PROTOBUF_LIBRARY_DLL=%PROTOBUF_LIBRARY_DLL_64%"
  set "OPENSSL_ROOT_DIR=%OPENSSL_ROOT_DIR_64%"
  set "full_generator=%~1 Win64"
)

set PATH=%JAVA_HOME%\bin;%PATH%
echo "Using JAVA_HOME=%JAVA_HOME%"

if exist %build_dir% rmdir %build_dir% /s /q
mkdir %build_dir%
cd %build_dir%

cmake -G "%full_generator%" -DCPACK_PACKAGE_VERSION_MAJOR=%version.1major% -DCPACK_PACKAGE_VERSION_MINOR=%version.2minor% -DCPACK_PACKAGE_VERSION_PATCH="%version.3micro%.%version.4qualifier%" -DSWIG_DIR=%SWIG_DIR% -DSWIG_EXECUTABLE=%SWIG_EXECUTABLE% -DMVN_PROGRAM=%MVN_PROGRAM% -DPROTOBUF_LIBRARY="%PROTOBUF_LIBRARY%" -DPROTOBUF_LIBRARY_DLL="%PROTOBUF_LIBRARY_DLL%" -DPROTOBUF_INCLUDE_DIR="%PROTOBUF_INCLUDE_DIR%" -DPROTOBUF_PROTOC_EXECUTABLE="%PROTOBUF_PROTOC_EXECUTABLE%" -DOPENSSL_ROOT_DIR="%OPENSSL_ROOT_DIR%" ..
if %errorlevel% neq 0 goto fail

cmake --build . --config RelWithDebInfo
if %errorlevel% neq 0 goto fail

ctest -V --timeout 3000
if %errorlevel% neq 0 goto fail

cpack -G ZIP -C RelWithDebInfo -DCPACK_PACKAGE_VERSION_MAJOR=%version.1major% -DCPACK_PACKAGE_VERSION_MINOR=%version.2minor% -DCPACK_PACKAGE_VERSION_PATCH="%version.3micro%.%version.4qualifier%"
if %errorlevel% neq 0 goto fail

endlocal
goto:eof

:usage
@echo "%~1" is null
@echo This script needs these env variables:
@echo 	generator:		Cmake generator
@echo 	JAVA_HOME_32:		Java home 32 bit
@echo 	JAVA_HOME_64:		Java home 64 bit
@echo 	INFINISPAN_VERSION:	Version for the Infinispan server
@echo 	version.1major:		Major version
@echo 	version.2minor:		Minor
@echo 	version.3micro:		Micro
@echo 	version.4qualifier:	(Qualifier Final,SNAPSHOT, Beta...)
@echo 	SWIG_DIR:	 	Swig installation dir
@echo 	SWIG_EXECUTABLE:	Swig executable
@echo 	MVN_PROGRAM:	 	Mvn executable
()
    exit /b 2


:fail
    echo "Failure!"
    ()
    exit /b 1
:eof
