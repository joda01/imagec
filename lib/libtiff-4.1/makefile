#
#   Makefile to make libtiff, libjpeg, and libz for inclusion in Aperio
#   software.  Compatible with Linux (e.g. RedHat).
#
#   050109   Eichhorn	Initial creation
#   060608   Eichhorn	change subdir logic for multiple architectures
#   070316   Eichhorn	update to support multiple versions of each library
#
COPT=-O3

#--- rel 8.1 and earlier
# ZDIR=libz/zlib-1.1.3
# TIFFDIR=tiff-v3.5.5/libtiff

#--- rel 8.2
ZDIR=libz/zlib123
JPEGDIR=libjpeg/jpeg-6b
TIFFDIR=tiff-4.1/libtiff

all: libz.a libjpeg.a libtiff.a

clean:
	rm -f $(ZDIR)/*.o $(ZDIR)/*.a $(JPEGDIR)/*.o $(JPEGDIR)/*.a $(TIFFDIR)/*.o $(TIFFDIR)/*.a

$(ZDIR)/libz.a: $(ZDIR)/*.c $(ZDIR)/*.h
	cd $(ZDIR); make -f Makefile CFLAGS="$(COPT) -DHAVE_UNISTD_H -DUSE_MMAP $(ARCH)"

libz.a: $(ZDIR)/libz.a
	cp $(ZDIR)/libz.a .

$(JPEGDIR)/libjpeg.a: $(JPEGDIR)/*.c $(JPEGDIR)/*.h
	cd $(JPEGDIR); make -f makefile.ansi CFLAGS="$(COPT) $(ARCH)" LDFLAGS="$(ARCH)"

libjpeg.a: $(JPEGDIR)/libjpeg.a
	cp $(JPEGDIR)/libjpeg.a .

$(TIFFDIR)/libtiff.a: $(TIFFDIR)/*.c $(TIFFDIR)/*.h
	cd $(TIFFDIR); make -f Makefile OPTIMIZER=$(COPT) COPTS="$(ARCH)"

libtiff.a: $(TIFFDIR)/libtiff.a
	cp $(TIFFDIR)/libtiff.a .
