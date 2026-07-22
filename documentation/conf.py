# Trifecta SDK documentation configuration.
#
# Building locally:
#   pip install -r requirements.txt
#   sphinx-build -M html . _build
#
# (Or simply: make html  --  see the Makefile in this directory.)
#
# The ReadTheDocs theme is used when available (it ships with readthedocs.io),
# otherwise the build falls back to the built-in "classic" theme, so the only
# strict dependency is Sphinx itself.

import os
import sys

project = 'Trifecta SDK'
author = '4rge.ai / Triangle Man LLC'
copyright = '2026, 4rge.ai / Triangle Man LLC'

# Short version (e.g. "1.0") and full version (e.g. "1.0.1b1")
version = '0.1'
release = '0.1'

extensions = [
    'sphinx.ext.todo',
]

# The empty _static directory is committed via .gitkeep; it is the place to
# drop custom CSS (e.g. _static/custom.css) later on.
html_static_path = ['_static']
html_last_updated_fmt = '%Y-%m-%d'
html_title = 'Trifecta SDK'

try:
    import sphinx_rtd_theme  # noqa: F401
    html_theme = 'sphinx_rtd_theme'
except ImportError:
    html_theme = 'classic'

exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

todo_include_todos = True

# Keep the RST tidy: warn (but do not fail) on missing references.
nitpicky = False
