SUBDIRS = gui

bin_PROGRAMS = brtablet

brtablet_SOURCES = calibrator_rts.c \
					io.c\
					util.c\
					main_common.c\
					inputattach.c\
					gui/gui_gtk.c\
					gui/gui_common.c

brtablet_LDADD = $(GTK_LIBS)
brtablet_CFLAGS = $(GTK_CFLAGS)
 


