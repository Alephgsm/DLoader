echo off

set SRC_DIR=..\..\..\common\bin
set DST_DIR=..\..\..\Release\UpgradeDownload\Bin
set FILES=Channel.dll BMPlatform.dll UpgradeDownload.ini UpgradeDownload.exe BMFileType.ini BMError.ini BMTimeout.ini BMAFrame.dll udl_bkmark.bmp MCPType.ini Guide.chm

for %%f in (%FILES%) do (
@call xcopy /R /Y "%SRC_DIR%\%%f" "%DST_DIR%\%%f"
)

