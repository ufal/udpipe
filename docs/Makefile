HTMLS:=models.html udpipe_2.html

.PHONY: all
all: $(HTMLS)

%.md: %.md.sh $(wildcard *.md)
	sh $< >$@

%.fragment.html: %.md
	scripts/preprocess <$< | scripts/md >$@

%.html: %.fragment.html
	scripts/html <$< >$@

%.noheading.html: %.html
	sed '/<h1/d' $< >$@

.PHONY: web_install
web_install: udpipe_2.fragment.noheading.html models.fragment.html
	scp $^ ufal:public_html/udpipe/2/

.PHONY: clean
clean:
	rm -rf *.html $(patsubst %.md.sh,%.md,$(wildcard *.md.sh))
