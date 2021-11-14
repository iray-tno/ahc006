clang++ -target x86_64-pc-windows-gnu ./ans.cpp -O3 -D LOCAL
if %ERRORLEVEL% NEQ 0 goto FAILURE


powershell -C (Measure-Command {"((Get-Content ./input.txt | ./a.exe > out.txt))"}).TotalMilliseconds
if %ERRORLEVEL% NEQ 0 goto FAILURE

goto END

:FAILURE
echo ERROR STOP: %ERRORLEVEL%

:END
exit /b
