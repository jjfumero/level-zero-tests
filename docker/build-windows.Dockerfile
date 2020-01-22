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

# install devtool and gta-asset tools
# TODO: using a patched version that fixes several issues for now, waiting for
# the fixes to get merged to master.
RUN git clone https://gitlab.devtools.intel.com/ledettwy/devtool.git C:\devtool; `
    cd C:\devtool; `
    git fetch origin ledettwy/mount_as_root_dir; `
    git checkout f7390409fe05e4a3f201ffb876aaf34555113314; `
    cmd /c dt self-install; `
    [Environment]::SetEnvironmentVariable('Path', $env:Path + ';C:\devtool;C:\devtool\.deps\tools\gta-asset', [EnvironmentVariableTarget]::Machine)

FROM toolchain AS toolchain_deps_zlib

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
RUN cd C:\VS\Common7\Tools & VsDevCmd && `
    cd C:\TEMP\zlib-1.2.11 && `
    mkdir build && `
    cd build && `
    cmake .. -A x64 -T host=x64 `
      -D BUILD_SHARED_LIBS=YES `
      -D CMAKE_INSTALL_PREFIX:PATH=/zlib && `
    cmake --build . --target INSTALL --config Release
ENV CMAKE_PREFIX_PATH /zlib

FROM toolchain_deps_zlib AS toolchain_deps_libpng

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
RUN cd C:\VS\Common7\Tools & VsDevCmd && `
    cd C:\TEMP\lpng1637 && `
    mkdir build && `
    cd build && `
    cmake .. -T host=x64 -A x64 -D CMAKE_INSTALL_PREFIX:PATH=/libpng && `
    cmake --build . --target INSTALL --config Release

FROM toolchain AS toolchain_deps_opencl

# install OpenCL headers
SHELL ["powershell"]
RUN [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; `
    cd C:\TEMP; `
    wget `
      -Uri "https://github.com/KhronosGroup/OpenCL-Headers/archive/de26592167b9fdea503885e40e8755393c56523d.zip" `
      -OutFile OpenCL-Headers.zip; `
    . 'C:\Program Files\7-Zip\7z.exe' x OpenCL-Headers.zip; `
    rm OpenCL-Headers.zip;
RUN mkdir /opencl/include -Force | Out-Null; `
    mv C:\TEMP\OpenCL-Headers-de26592167b9fdea503885e40e8755393c56523d\CL /opencl/include/CL;

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
      -D OPENCL_INCLUDE_DIRS="/opencl/include" `
      -D CMAKE_ARCHIVE_OUTPUT_DIRECTORY="/opencl/lib" `
      -D CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE="/opencl/lib" && `
    cmake --build . --config Release

FROM toolchain AS toolchain_deps_boost

# install boost
SHELL ["powershell"]
RUN [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; `
    cd C:\TEMP; `
    wget `
      -Uri https://dl.bintray.com/boostorg/release/1.70.0/source/boost_1_70_0.zip `
      -OutFile boost_1_70_0.zip `
      -UserAgent [Microsoft.PowerShell.Commands.PSUserAgent]::InternetExplorer; `
    . 'C:\Program Files\7-Zip\7z.exe' x boost_1_70_0.zip; `
    rm boost_1_70_0.zip
SHELL ["cmd", "/S", "/C"]
RUN cd C:\VS\Common7\Tools & VsDevCmd && `
    call "%VCInstallDir%\Auxiliary\Build\vcvars64.bat" && `
    cd C:\TEMP\boost_1_70_0 && `
    .\bootstrap && `
    .\b2 install `
      -j 4 `
      address-model=64 `
      --prefix=/boost `
      --with-chrono `
      --with-log `
      --with-program_options `
      --with-system `
      --with-timer

FROM toolchain AS dev

COPY --from=toolchain_deps_boost ["/boost", "/boost"]
COPY --from=toolchain_deps_zlib ["/zlib", "/zlib"]
COPY --from=toolchain_deps_libpng ["/libpng", "/libpng"]
COPY --from=toolchain_deps_opencl ["/opencl", "/opencl"]

ENV CMAKE_PREFIX_PATH "/boost;/zlib;/libpng;/opencl"

SHELL ["cmd", "/S", "/C"]

ENTRYPOINT C:\VS\Common7\Tools\VsDevCmd.bat &&
