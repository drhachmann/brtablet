#http://www.gnu.org/s/hello/manual/automake/Scripts.html

SUBDIRS = src
dist_doc_DATA = README

#brtablet_initdir = /home/diego
#bin_DIR = /home/diego
#configdir = /home/diego

bootdir = /etc/init.d
boot_SCRIPTS = scripts/init-brtablet.sh

install-exec-hook:
#	@if test -x /usr/sbin/update-rc.d; then \
#		echo "/usr/sbin/update-rc.d default"; \
		update-rc.d init-brtablet.sh\
#	fi

uninstall-local:
	update-rc.d init-brtablet remove


#EXTRA_DIST = $(bin_SCRIPTS)

#man_MANS = man/brtablet.8
#update-rc.d foobar remove remove script
#update-rc.d -f foobar remove deixa script
#update-rc.d foobar defaults

#INCLUDES = $(helloworld_CFLAGS)
#helloworld_LDADD = $(helloworld_LIBS)
 

