generated_htmls = ds_array.html

all: $(generated_htmls)

$(generated_htmls): %.html: %.md include.hs template.html
	pandoc --template template.html --toc --filter include.hs -t html5 $< -o $@

array_test: array_test.c array.c array.h double_array.h
	clang -O0 -g array_test.c array.c -o $@
	valgrind ./array_test
