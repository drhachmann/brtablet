SUBDIRS = gui

bin_PROGRAMS = saida

saida_SOURCES = calibrator_rts.c \
					io.c\
					util.c\
					main_common.c\
					inputattach.c\
					gui/gui_gtk.c\
					gui/gui_common.c

saida_LDADD = $(GTK_LIBS)
saida_CFLAGS = $(GTK_CFLAGS)
 


