# escape=`

################################################################################
# IMPORTANT: docker images for windows containers can be extremely large (tens
# of gigabytes) and take a longer time to build. Until the windows VM used for
# building the windows docker images can have its storage space expanded, please
# do not make any changes to this file without first discussing with Lisanna
# Dettwyler <lisanna.dettwyler@intel.com>
################################################################################

# ltsc2016-amd64
# FROM mcr.microsoft.com/windows/servercore:ltsc2016-amd64
FROM mcr.microsoft.com/dotnet/framework/sdk:4.7.2-20190312-windowsservercore-ltsc2016 AS toolchain

ENV HTTP_PROXY http://proxy-chain.intel.com:911
ENV HTTPS_PROXY http://proxy-chain.intel.com:911

SHELL ["powershell"]

# Install Visual Studio 16 (2019)
ARG VS2019PRO_PRODUCT_KEY
ENV VS2019PRO_PRODUCT_KEY $VS2019PRO_PRODUCT_KEY
RUN mkdir C:\TEMP; `
    wget -Uri https://aka.ms/vscollect.exe -OutFile C:\TEMP\collect.exe; `
    wget `
      -Uri https://aka.ms/vs/16/release/vs_professional.exe `
      -OutFile C:\TEMP\vs_professional.exe
SHELL ["cmd", "/S", "/C"]
RUN C:\TEMP\vs_professional.exe `
      --quiet --wait --norestart --nocache `
      --installPath C:\VS `
      --productKey %VS2019PRO_PRODUCT_KEY% `
      --includeRecommended `
      --add Microsoft.VisualStudio.Workload.NativeDesktop `
      --add Microsoft.VisualStudio.Workload.ManagedDesktop `
      --add Microsoft.VisualStudio.Component.Git `
    || IF "%ERRORLEVEL%"=="3010" EXIT 0

SHELL ["powershell"]

ENV chocolateyVersion "0.10.15"
RUN [System.Net.ServicePointManager]::SecurityProtocol = 3072 -bor 768 -bor 192 -bor 48; `
    iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1')); `
    wget `
      -Uri https://github.com/frerich/clcache/releases/download/v4.2.0/clcache.4.2.0.nupkg `
      -OutFile clcache.4.2.0.nupkg `
      -UserAgent [Microsoft.PowerShell.Commands.PSUserAgent]::InternetExplorer; `
    choco install -y --no-progress --fail-on-error-output 7zip; `
    choco install -y --no-progress --fail-on-error-output clcache --source=.

# cpack (an alias from chocolatey) and cmake's cpack conflict.
RUN Remove-Item -Force 'C:\ProgramData\chocolatey\bin\cpack.exe'

# install irepo and gta-asset tools
RUN git clone https://github.intel.com/GSDI/irepo C:\irepo; `
    cd C:\irepo; `
    .\irepo.bat download_tools; `
    [Environment]::SetEnvironmentVariable('Path', $env:Path + ';C:\irepo;C:\irepo\.irepo\tools\gta-asset', [EnvironmentVariableTarget]::Machine);

FROM toolchain AS build_deps

# install boost
# boost 1.70 is required to be compatible with Visual Studio 2019
RUN [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; `
    cd C:\TEMP; `
    wget `
      -Uri https://dl.bintray.com/boostorg/release/1.70.0/source/boost_1_70_0.zip `
      -OutFile boost_1_70_0.zip `
      -UserAgent [Microsoft.PowerShell.Commands.PSUserAgent]::InternetExplorer; `
    . 'C:\Program Files\7-Zip\7z.exe' x boost_1_70_0.zip; `
    rm boost_1_70_0.zip
SHELL ["cmd", "/S", "/C"]
ENV BOOST_ROOT "C:\boost"
RUN cd C:\VS\Common7\Tools & VsDevCmd && `
    cd C:\TEMP\boost_1_70_0 && `
    .\bootstrap && `
    .\b2 install `
      address-model=64 `
      --prefix=%BOOST_ROOT% `
      --with-program_options `
      --with-timer `
      --with-chrono `
      --with-log `
      --with-system `
      -j4 && `
    cd C:\ && `
    rmdir /S /Q TEMP\boost_1_70_0

# install zlib
SHELL ["powershell"]
RUN [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; `
    cd C:\TEMP; `
    wget `
      -Uri https://zlib.net/zlib1211.zip `
      -OutFile zlib1211.zip `
      -UserAgent [Microsoft.PowerShell.Commands.PSUserAgent]::InternetExplorer; `
    . 'C:\Program Files\7-Zip\7z.exe' x zlib1211.zip; `
    rm zlib1211.zip
SHELL ["cmd", "/S", "/C"]
ENV ZLIB_ROOT "C:\zlib"
RUN cd C:\VS\Common7\Tools & VsDevCmd && `
    cd C:\TEMP\zlib-1.2.11 && `
    mkdir build && `
    cd build && `
    cmake .. -A x64 -T host=x64  -DCMAKE_INSTALL_PREFIX:PATH=%ZLIB_ROOT% && `
    cmake --build . --target INSTALL --config Release && `
    cd C:\ && `
    rmdir /S /Q TEMP\zlib-1.2.11

# install libpng
SHELL ["powershell"]
RUN [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; `
    cd C:\TEMP; `
    wget `
      -Uri https://prdownloads.sourceforge.net/libpng/lpng1637.zip `
      -OutFile lpng1637.zip `
      -UserAgent [Microsoft.PowerShell.Commands.PSUserAgent]::InternetExplorer; `
    . 'C:\Program Files\7-Zip\7z.exe' x lpng1637.zip; `
    rm lpng1637.zip
SHELL ["cmd", "/S", "/C"]
ENV LIBPNG_ROOT "C:\libpng"
RUN cd C:\VS\Common7\Tools & VsDevCmd && `
    cd C:\TEMP\lpng1637 && `
    mkdir build && `
    cd build && `
    cmake .. `
      -T host=x64 `
      -A x64 `
      -DPNG_BUILD_ZLIB=ON `
      -DZLIB_INCLUDE_DIR=%ZLIB_ROOT%\include `
      -DZLIB_LIBRARY=%ZLIB_ROOT%\lib\zlibstatic.lib `
      -DPNG_SHARED=OFF `
      -DCMAKE_INSTALL_PREFIX:PATH=%LIBPNG_ROOT% && `
    cmake --build . --target INSTALL --config Release && `
    cd C:\ && `
    rmdir /S /Q TEMP\lpng1637

# install OpenCL headers
SHELL ["powershell"]
RUN [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; `
    cd C:\TEMP; `
    wget `
      -Uri "https://github.com/KhronosGroup/OpenCL-Headers/archive/de26592167b9fdea503885e40e8755393c56523d.zip" `
      -OutFile OpenCL-Headers.zip; `
    . 'C:\Program Files\7-Zip\7z.exe' x OpenCL-Headers.zip; `
    rm OpenCL-Headers.zip;
ENV OPENCL_ROOT "C:\opencl"
RUN mkdir $ENV:OPENCL_ROOT\include -Force | Out-Null; `
    mv C:\TEMP\OpenCL-Headers-de26592167b9fdea503885e40e8755393c56523d\CL $ENV:OPENCL_ROOT\include\CL;

# install OpenCL ICD Loader
SHELL ["powershell"]
RUN [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; `
    cd C:\TEMP; `
    wget `
      -Uri https://github.com/KhronosGroup/OpenCL-ICD-Loader/archive/b342ff7b7f70a4b3f2cfc53215af8fa20adc3d86.zip `
      -OutFile OpenCL-ICD-Loader-b342ff7b7f70a4b3f2cfc53215af8fa20adc3d86.zip `
      -UserAgent [Microsoft.PowerShell.Commands.PSUserAgent]::InternetExplorer; `
    . 'C:\Program Files\7-Zip\7z.exe' x OpenCL-ICD-Loader-b342ff7b7f70a4b3f2cfc53215af8fa20adc3d86.zip; `
    rm OpenCL-ICD-Loader-b342ff7b7f70a4b3f2cfc53215af8fa20adc3d86.zip
SHELL ["cmd", "/S", "/C"]
RUN cd C:\VS\Common7\Tools & VsDevCmd && `
    cd C:\TEMP\OpenCL-ICD-Loader-b342ff7b7f70a4b3f2cfc53215af8fa20adc3d86 && `
    mkdir build && `
    cd build && `
    cmake .. `
      -T host=x64 `
      -A x64 `
      -DOPENCL_INCLUDE_DIRS="%OPENCL_ROOT%\include" `
      -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY="%OPENCL_ROOT%\lib" `
      -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE="%OPENCL_ROOT%\lib" && `
    cmake --build . --config Release && `
    cd C:\ && `
    rmdir /S /Q TEMP\OpenCL-ICD-Loader-b342ff7b7f70a4b3f2cfc53215af8fa20adc3d86

FROM toolchain AS dev

COPY --from=build_deps C:\boost C:\boost
COPY --from=build_deps C:\zlib C:\zlib
COPY --from=build_deps C:\libpng C:\libpng
COPY --from=build_deps C:\opencl C:\opencl

SHELL ["cmd", "/S", "/C"]

ENTRYPOINT C:\VS\Common7\Tools\VsDevCmd.bat &&
