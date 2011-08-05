#http://www.gnu.org/s/hello/manual/automake/Scripts.html

AUTOMAKE_OPTIONS = foreign

SUBDIRS = src/driver\
			 src/attach\
			 src/calibrator\
			 scripts

dist_doc_DATA = README

#boot_SCRIPTS = scripts/brtablet-init.sh
#bootdir = /etc/init.d


#brtablet_initdir = /home/diego
#bin_DIR = /home/diego
#configdir = /home/diego



install-post:
	echo "install" > /etc/brtablet/install
#	@if test -x /usr/sbin/update-rc.d; then \
#		echo "/usr/sbin/update-rc.d default"; \
#		mkdir -p /etc/brtablet
#		update-rc.d init-brtablet.sh defaults
#	fi

uninstall-local:
	echo "uninstall" > /etc/brtablet/uninstall
#	rm -rf /etc/brtablet/
#	update-rc.d init-brtablet remove


#EXTRA_DIST = $(bin_SCRIPTS)

#man_MANS = man/brtablet.8
#update-rc.d foobar remove remove script
#update-rc.d -f foobar remove deixa script
#update-rc.d foobar defaults

#INCLUDES = $(helloworld_CFLAGS)
#helloworld_LDADD = $(helloworld_LIBS)
 

