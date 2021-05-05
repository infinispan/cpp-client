if [%generator%] == [] call:usage generator
if [%JAVA_HOME%] == [] call:usage JAVA_HOME
if [%INFINISPAN_VERSION%] == [] call:usage INFINISPAN_VERSION
if [%SWIG_DIR%] == [] call:usage SWIG_DIR
if [%SWIG_EXECUTABLE%] == [] call:usage SWIG_EXECUTABLE
if [%MVN_PROGRAM%] == [] call:usage MVN_PROGRAM
if [%PROTOBUF_LIBRARY%] == [] call:usage PROTOBUF_LIBRARY
if [%PROTOBUF_INCLUDE_DIR%] == [] call:usage PROTOBUF_INCLUDE_DIR

set JBOSS_HOME=%cd%\infinispan-server-%INFINISPAN_VERSION%
set JBOSS_ZIP=infinispan-server-%INFINISPAN_VERSION%.zip
set JBOSS_URL=http://download.jboss.org/infinispan/%INFINISPAN_VERSION%/%JBOSS_ZIP%

if not exist %JBOSS_HOME% (
  if not exist %JBOSS_ZIP% (
     powershell.exe -Command "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; (new-object System.Net.WebClient).DownloadFile('%JBOSS_URL%','%JBOSS_ZIP%')"
   )
"C:\Program Files\7-zip\7z.exe" x %JBOSS_ZIP% 
)
copy test\data\*.* %JBOSS_HOME%\server\conf
xcopy /E /I /Y %JBOSS_HOME%\server %JBOSS_HOME%\server1\

call:do_build %generator%
goto:eof

:do_build
setlocal
set build_dir=build_win

set PATH=%JAVA_HOME%\bin;%PATH%
echo "Using JAVA_HOME=%JAVA_HOME%"

if exist %build_dir% rmdir %build_dir% /s /q
mkdir %build_dir%
cd %build_dir%

if [%CLIENT_VERSION%] neq [] set V1=%CLIENT_VERSION:*/=%

for /f "tokens=1,2,3,4 delims=." %%a in ("%V1%") do (
   set version_1major=%%a
   set version_2minor=%%b
   set version_3micro=%%c
   set version_4qualifier=%%d
)
rem If empty or not a numnber, set values to default
if [%version_1major%] equ [] set version_1major=0
if [%version_2minor%] equ [] set version_2minor=1
if [%version_3micro%] equ [] set version_3micro=0
if [%version_4qualifier%] equ [] set version_4qualifier=SNAPSHOT

if 1%version_1major% neq +1%version_1major% set version_1major=0
if 1%version_2minor% neq +1%version_2minor% set version_2minor=1
if 1%version_3micro% neq +1%version_3micro% set version_3micro=0

set version_patch=%version_3micro%.%version_4qualifier%

cmake -G "%~1" -DCPACK_PACKAGE_VERSION_MAJOR="%version_1major%" -DCPACK_PACKAGE_VERSION_MINOR="%version_2minor%" -DCPACK_PACKAGE_VERSION_PATCH="%version_patch%" -DSWIG_EXECUTABLE=%SWIG_EXECUTABLE% -DMVN_PROGRAM=%MVN_PROGRAM% -DPROTOBUF_LIBRARY="%PROTOBUF_LIBRARY%" -DPROTOBUF_PROTOC_LIBRARY="%PROTOBUF_PROTOC_LIBRARY%" -DPROTOBUF_INCLUDE_DIR="%PROTOBUF_INCLUDE_DIR%" -DPROTOBUF_PROTOC_EXECUTABLE="%PROTOBUF_PROTOC_EXECUTABLE%" -DOPENSSL_ROOT_DIR="%OPENSSL_ROOT_DIR%" ..
if %errorlevel% neq 0 goto fai

set home_drive=%CD:~0,2%

subst /D Y:
subst Y: .

Y:

PATH=Y:\RelWithDebInfo;%PATH%
cmake --build . --config RelWithDebInfo
if %errorlevel% neq 0 goto fail

ctest -V --timeout 120
if %errorlevel% neq 0 goto fail

cpack -G ZIP -C RelWithDebInfo -DCPACK_PACKAGE_VERSION_MAJOR=%version_1major% -DCPACK_PACKAGE_VERSION_MINOR=%version_2minor% -DCPACK_PACKAGE_VERSION_PATCH="%version_patch%"

%home_drive%

subst /D Y:

if %errorlevel% neq 0 goto fail

endlocal
goto:eof

:usage
@echo "%~1" is null
@echo This script needs these env variables:
@echo 	generator:		Cmake generator
@echo 	JAVA_HOME_64:		Java home 64 bit
@echo 	INFINISPAN_VERSION:	Version for the Infinispan server
@echo 	SWIG_DIR:	 	Swig installation dir
@echo 	SWIG_EXECUTABLE:	Swig executable
@echo 	MVN_PROGRAM:	 	Mvn executable
()
    exit /b 2


:fail
    subst /D Y:
    echo "Failure!"
    ()
    exit /b 1
:eof
