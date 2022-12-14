echo off

set SRC_DIR=..\..\..\common\bin
set DST_DIR=..\..\..\Release\FactoryDownload\Bin
set FILES=Channel9.dll BMPlatform9.dll FactoryDownload.ini FactoryDownload.exe BMFileType.ini BMError.ini BMTimeout.ini BMAFrame9.dll fdl_bkmark.bmp MCPType.ini PhaseCheck.ini Guide.chm

for %%f in (%FILES%) do (
xcopy /R /Y "%SRC_DIR%\%%f" "%DST_DIR%\%%f"
)

