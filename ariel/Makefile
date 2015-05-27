generated_htmls = ds_array.html

all: $(generated_htmls)

$(generated_htmls): %.html: %.md include.hs template.html
	pandoc --template template.html --toc --filter include.hs -t html5 $< -o $@
