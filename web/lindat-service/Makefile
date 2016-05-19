TARGETS=.htaccess about.html api-reference.php blob.min.js demo.php filesaver.min.js
TARGETS+=footer.php header.php js-treex-view.min.js icon.png info.php udpipe.css
all: $(TARGETS)

refresh:
	$(MAKE) -C ../../doc/ web

about.html: refresh
	sed -n '/^<div class="body"/,/^<\/div/p' ../../doc/web_$@ | sed 's/<div class="body"[^>]*>/<div>/' >$@

install: $(TARGETS)
	scp $(TARGETS) quest:/var/www/udpipe/

.PHONY: clean
clean:
	rm -f about.html
