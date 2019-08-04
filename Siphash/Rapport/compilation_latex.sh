rm Sipash.aux Siphash.log Siphash.out Siphash.bcf
lualatex -shell-escape Siphash
biber Siphash
lualatex -shell-escape Siphash
lualatex -shell-escape Siphash
