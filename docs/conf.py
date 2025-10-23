# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html
import sys
from pathlib import Path

from sphinx.application import Sphinx

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = "LychSim"
copyright = "2025, LychSim Team"
author = "LychSim Team"
release = "0.1.0"

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

sys.path.append(str(Path("_ext").resolve()))

extensions = [
    "sphinx.ext.autodoc",
    "mybadges",
    "sphinx_design",
    "mytoctree",
]

templates_path = ["_templates"]
exclude_patterns = ["_build", "Thumbs.db", ".DS_Store"]

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = "pydata_sphinx_theme"
html_static_path = ["_static"]
html_css_files = ["custom.css", "mycss.css", "mybadges.css"]

html_title = "LychSim"
html_theme_options = {
   "logo": {
      "text": "LychSim",
      "image_light": "_static/lychee.png",
   },
   "footer_start": ["copyright"],
   "footer_end": ["theme-version.html"],
   # "secondary_sidebar_items": [],
   "navbar_end": [],
}
html_show_sourcelink = False
html_sidebars = {
   "**": [],
}

html_theme_options = {
  "external_links": [
      {"name": "GitHub", "url": "https://github.com/wufeim/LychSim"},
  ]
}

rst_prolog = """
.. |area-llm| replace:: :bdg-blue-line:`LLM`
.. |area-vlm| replace:: :bdg-lightblue-line:`VLM`
.. |area-rl| replace:: :bdg-purple-line:`Reinforcement Learning`
.. |area-3d| replace:: :bdg-orange-line:`3D`
.. |area-pt| replace:: :bdg-green-line:`Pretraining`
.. |area-dl| replace:: :bdg-amber-line:`Deep Learning`
.. |area-video| replace:: :bdg-pink-line:`Video`
.. |area-robot| replace:: :bdg-indigo-line:`Robotics`
.. |area-gen| replace:: :bdg-deeporange-line:`Generation`
.. |area-math| replace:: :bdg-teal-line:`Math`
.. role:: ul
   :class: ul
.. role:: bul
   :class: bul
.. role:: blue-bul
   :class: blue-bul
.. role:: orange
.. role:: mono
   :class: mono
"""

def setup(app: Sphinx) -> None:
    app.add_css_file("css/custom.css")
    app.add_css_file("css/mycss.css")
    app.add_css_file("css/mybadges.css")
