@echo off
cd /d "%~dp0"

echo 清空 Core...
powershell -Command "Get-ChildItem -Recurse 'Core' -File | ForEach-Object { Set-Content -LiteralPath $_.FullName -Value '' }"

echo 清空 Engine\IAL...
powershell -Command "Get-ChildItem -Recurse 'Engine\IAL' -File | ForEach-Object { Set-Content -LiteralPath $_.FullName -Value '' }"

echo 清空 Engine\Implementations\NCLGL_Impl...
powershell -Command "Get-ChildItem -Recurse 'Engine\Implementations\NCLGL_Impl' -File | ForEach-Object { Set-Content -LiteralPath $_.FullName -Value '' }"

echo 完成
pause
