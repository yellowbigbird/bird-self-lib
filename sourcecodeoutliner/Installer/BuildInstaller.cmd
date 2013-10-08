@REM Copyright (C) Microsoft Corporation. All rights reserved.

setlocal enabledelayedexpansion

@REM The path to the WiX directory
set WiXPath=%~1

@REM Setup MSI File
set SETUP_MSI="%~dp0SourceCodeOutliner.msi"

@REM WixObj File
set WIX_OBJ="%~dp0SourceCodeOutliner.wixobj"


@REM Delete the previous MSI, if present.
del /F %SETUP_MSI%

@REM Build the registry include
"%VSSDK90Install%\VisualStudioIntegration\Tools\Bin\regpkg.exe" /codebase /wixfile:SourceCodeOutliner.Registry.wxi "%~dp0Bin\Microsoft.Shared.PowerToy.SourceOutlinerPackage.dll"


@REM Compile the wxs file
"!WixPath!candle.exe" -out %WIX_OBJ% "%~dp0SourceCodeOutliner.wxs"
if "!ERRORLEVEL!"=="0" (
  echo "Candle generated %WIX_OBJ%"
) else (
  echo "Candle returned !ERRORLEVEL!"
  exit /b !ERRORLEVEL!
)

@REM Build the installer
"!WixPath!light.exe" -out %SETUP_MSI% %WIX_OBJ% "!WixPath!lib\wixui_featuretree.wixlib" "!WixPath!ca\wixca.wixlib" -loc "!WixPath!lib\WixUI_en-us.wxl"

if "!ERRORLEVEL!"=="0" (
  echo "Light generated %SETUP_MSI%"
) else (
  echo "Light returned !ERRORLEVEL!"
  exit /b !ERRORLEVEL!
)

exit /b !ERRORLEVEL!
