# Minimal Makefile for building the Trifecta SDK documentation with Sphinx.

SPHINXOPTS    ?=
SPHINXBUILD   ?= sphinx-build
SOURCEDIR     = .
BUILDDIR      = _build

# Put the "sphinx-build" executable on PATH via a virtualenv if desired:
#   VIRTUAL_ENV=... make html

.PHONY: help
help:
	@echo "Build the documentation with, e.g.:"
	@echo "  make html      -> $(BUILDDIR)/html/"
	@echo "  make clean"

# Generic rule: 'make latexpdf', 'make linkcheck', 'make doctest', ...
$(BUILDDIR)/%:
	$(SPHINXBUILD) -M $* "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

.PHONY: html
html:
	$(SPHINXBUILD) -M html "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

.PHONY: latexpdf
latexpdf:
	$(SPHINXBUILD) -M latexpdf "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

.PHONY: linkcheck
linkcheck:
	$(SPHINXBUILD) -M linkcheck "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

.PHONY: doctest
doctest:
	$(SPHINXBUILD) -M doctest "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)
