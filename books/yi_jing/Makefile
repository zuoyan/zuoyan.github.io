generated_htmls = IChing.html

all: $(generated_htmls) 

$(generated_htmls): %.html: %.md ../../template/include.hs ../../template/template.html
	pandoc --template ../../template/template.html --toc --filter ../../template/include.hs -t html5 $< -o $@
