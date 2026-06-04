@echo off
chcp 65001 >nul
echo ==============================
echo   正在编译...
echo ==============================
gcc -o orderdish.exe main.c admin.c customer.c main_form.c order.c shopping_cart.c kitchen.c adjust.c income.c -Wall
if %errorlevel% neq 0 (
    echo.
    echo 编译失败！请检查错误信息。
    pause
    exit /b
)
echo 编译成功！正在运行...
echo ==============================
orderdish.exe
pause
