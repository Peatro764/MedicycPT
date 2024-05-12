TEMPLATE = subdirs
SUBDIRS = src gui tests
gui.depends = src
tests.depends = src


