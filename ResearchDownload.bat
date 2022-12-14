echo off

set SRC_DIR=..\..\..\common\bin
set DST_DIR=..\..\..\Release\ResearchDownload\Bin
set FILES=Channel.dll BMPlatform.dll ResearchDownload.ini ResearchDownload.exe BMFileType.ini BMError.ini BMTimeout.ini BMAFrame.dll rdl_bkmark.bmp MCPType.ini PhaseCheck.ini Guide.chm

for %%f in (%FILES%) do (
@call xcopy /R /Y "%SRC_DIR%\%%f" "%DST_DIR%\%%f"
)

