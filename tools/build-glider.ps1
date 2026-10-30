param([switch]$CoreOnly)
$ErrorActionPreference = 'Stop'
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
$toolchain = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $toolchain) { throw 'Visual Studio C++ Build Tools were not found.' }
$env:PATH = (Split-Path -Parent $vswhere) + ';' + $env:PATH
$build = if ($CoreOnly) { 'core-ninja' } else { 'windows-ninja' }
$xr = if ($CoreOnly) { 'OFF' } else { 'ON' }
$observer = if ($CoreOnly) { 'OFF' } else { 'ON' }
& (Join-Path $PSScriptRoot 'build-native.cmd') $toolchain $build 'Release' $xr $observer
if ($LASTEXITCODE) { throw 'Glider build or CTest failed.' }
