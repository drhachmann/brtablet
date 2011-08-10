#http://www.gnu.org/s/hello/manual/automake/Scripts.html

AUTOMAKE_OPTIONS = foreign

SUBDIRS = src/driver\
			 src/attach\
			 src/brtablet\
			 scripts

dist_doc_DATA = README

uninstall-local:
	rm /etc/brtablet -R

