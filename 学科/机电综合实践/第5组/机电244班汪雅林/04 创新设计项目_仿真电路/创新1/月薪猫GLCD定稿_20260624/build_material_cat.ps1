node generate_material_cat.js
sdcc --model-small --std-sdcc89 --code-size 65535 yuexin_cat_material.c
Copy-Item -LiteralPath 'yuexin_cat_material.ihx' -Destination 'yuexin_cat_material.hex' -Force
Get-Item -LiteralPath 'yuexin_cat_material.hex' | Select-Object FullName,Length,LastWriteTime
