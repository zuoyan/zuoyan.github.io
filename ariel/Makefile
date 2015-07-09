generated_htmls = ds_array.html

all: $(generated_htmls) array_test

$(generated_htmls): %.html: %.md ../template/include.hs ../ariel/template.html
	pandoc --template ../template/template.html --toc --filter ../template/include.hs -t html5 $< -o $@

ds_array.html: array_test

array_test: array_test.c array.c array.h double_array.h
	clang -std=c11 -Wall -O0 -g array_test.c array.c -o $@
	valgrind ./array_test
