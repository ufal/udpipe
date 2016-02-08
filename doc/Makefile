include ../src/Makefile.builtem
include t2t_docsys/Makefile.include

.PHONY: all doc web install
all: doc
doc: readme.txt readme_md.md manual.html manual.txt manual_install.txt
doc: manual_bindings_csharp.txt manual_bindings_java.txt manual_bindings_perl.txt manual_bindings_python.txt
web: web.html web_install.html web_online.html
install: doc
	$(call cp,readme.txt,../README)
	$(call cp,readme_md.md,../README.md)
	$(call cp,manual.txt,../MANUAL)
	$(call cp,manual.html,../MANUAL.html)
	$(call cp,manual_install.txt,../INSTALL)
	$(call cp,manual_bindings_csharp.txt,../bindings/README.CS)
	$(call cp,manual_bindings_java.txt,../bindings/README.JAVA)
	$(call cp,manual_bindings_perl.txt,../bindings/README.PERL)
	$(call cp,manual_bindings_python.txt,../bindings/README.PYTHON)

%.html: %.t2t $(wildcard *.t2t)
	$(call t2t_docsys_html,$@,$<,manual $(if $(findstring _,$*),striplevel))

web%html: manual%t2t $(wildcard *.t2t)
	$(call t2t_docsys_html,$@,$<,web html_addlevel $(if $(findstring _,$*),striplevel))

%.txt: %.t2t $(wildcard *.t2t)
	$(call t2t_docsys_txt,$@,$<,manual $(if $(findstring _,$*),striplevel))

%.md: %.t2t $(wildcard *.t2t)
	$(call t2t_docsys_md,$@,$<,manual $(if $(findstring _,$*),striplevel))

%.pdf: %.t2t $(wildcard *.t2t)
	$(call t2t_docsys_pdf,$@,$<,manual $(if $(findstring _,$*),striplevel))

.PHONY: clean
clean:
	@$(call rm,$(T2T_DOCSYS_OUTPUT_WILDCARDS))
