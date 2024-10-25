TARGETS=.htaccess about.html api-reference.php bootstrap-select.min.css bootstrap-select.min.js
TARGETS+=demo.php filesaver.min.js fill-using-params.js footer.php header.php js-treex-view.min.js icon.png info.php udpipe.css run.php
all: $(TARGETS)

refresh:
	$(MAKE) -C ../../doc/ web

about.html: refresh
	sed -n '/^<div class="body"/,/^<\/div/p' ../../doc/web_$@ | sed 's/<div class="body"[^>]*>/<div>/' >$@

install: $(TARGETS)
	rsync -avc $(TARGETS) udpipe:udpipe/www

install-flags:
	rsync -av flags/*.png udpipe:udpipe/www/flags/

.PHONY: clean
clean:
	rm -f about.html
