@echo off

for /r %%f in (*.tmpl) do (
  subwcrev . "%%f" "%%~pf%%~nf"
)

exit 0