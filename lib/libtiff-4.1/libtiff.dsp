# Microsoft Developer Studio Project File - Name="libtiff" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libtiff - Win32 Console_Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libtiff.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libtiff.mak" CFG="libtiff - Win32 Console_Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libtiff - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libtiff - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "libtiff - Win32 Console_Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libtiff - Win32 Console_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "libtiff"
# PROP Scc_LocalPath "."
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "libtiff - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "libtiff___Win32_Release"
# PROP BASE Intermediate_Dir "libtiff___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "includes" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\Release\libtiff.lib"

!ELSEIF  "$(CFG)" == "libtiff - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "libtiff___Win32_Debug"
# PROP BASE Intermediate_Dir "libtiff___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "includes" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WIN32__" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\Debug\libtiff.lib"

!ELSEIF  "$(CFG)" == "libtiff - Win32 Console_Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "libtiff___Win32_Console_Release"
# PROP BASE Intermediate_Dir "libtiff___Win32_Console_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Console_Release"
# PROP Intermediate_Dir "Console_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "libjpeg\jpeg-6b" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "JPEG_SUPPORT" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "includes" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "TIF_PLATFORM_CONSOLE" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\Release\libtiff.lib"
# ADD LIB32 /nologo /out:"..\lib\Release\Console\libtiff.lib"

!ELSEIF  "$(CFG)" == "libtiff - Win32 Console_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "libtiff___Win32_Console_Debug"
# PROP BASE Intermediate_Dir "libtiff___Win32_Console_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Console_Debug"
# PROP Intermediate_Dir "Console_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "libjpeg\jpeg-6b" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "JPEG_SUPPORT" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "includes" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "TIF_PLATFORM_CONSOLE" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo /out:"..\lib\Debug\libtiff.lib"
# ADD LIB32 /nologo /out:"..\lib\Debug\Console\libtiff.lib"

!ENDIF 

# Begin Target

# Name "libtiff - Win32 Release"
# Name "libtiff - Win32 Debug"
# Name "libtiff - Win32 Console_Release"
# Name "libtiff - Win32 Console_Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "jpeg"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jcapimin.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jcapistd.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jccoefct.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jccolor.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jcdctmgr.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jchuff.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jcinit.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jcmainct.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jcmarker.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jcmaster.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jcomapi.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jcparam.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jcphuff.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jcprepct.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jcsample.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jctrans.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jdapimin.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jdapistd.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jdatadst.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jdatasrc.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jdcoefct.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jdcolor.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jddctmgr.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jdhuff.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jdinput.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jdmainct.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jdmarker.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jdmaster.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jdmerge.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jdphuff.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jdpostct.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jdsample.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jdtrans.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jerror.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jerror.h"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jfdctflt.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jfdctfst.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jfdctint.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jidctflt.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jidctfst.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jidctint.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jidctred.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jmemmgr.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jmemnobs.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jquant1.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jquant2.c"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jutils.c"
# End Source File
# End Group
# Begin Group "tiff"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_aux.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_bigtiff.txt"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_close.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_codec.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_color.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_compress.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_dir.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_dirinfo.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_dirread.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_dirwrite.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_dumpmode.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_error.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_extension.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_fax3.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_fax3sm.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_flush.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_getimage.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_jpeg.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_luv.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_lzw.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_next.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_ojpeg.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_open.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_packbits.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_pixarlog.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_predict.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_print.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_read.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_strip.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_swab.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_thunder.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_tile.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_version.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_warning.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_win32.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_write.c"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_zip.c"
# End Source File
# End Group
# Begin Group "libz"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libz\zlib123\adler32.c
# End Source File
# Begin Source File

SOURCE=.\libz\zlib123\compress.c
# End Source File
# Begin Source File

SOURCE=.\libz\zlib123\crc32.c
# End Source File
# Begin Source File

SOURCE=.\libz\zlib123\deflate.c
# End Source File
# Begin Source File

SOURCE=.\libz\zlib123\example.c
# End Source File
# Begin Source File

SOURCE=.\libz\zlib123\gzio.c
# End Source File
# Begin Source File

SOURCE=.\libz\zlib123\infback.c
# End Source File
# Begin Source File

SOURCE=.\libz\zlib123\inffast.c
# End Source File
# Begin Source File

SOURCE=.\libz\zlib123\inflate.c
# End Source File
# Begin Source File

SOURCE=.\libz\zlib123\inftrees.c
# End Source File
# Begin Source File

SOURCE=.\libz\zlib123\trees.c
# End Source File
# Begin Source File

SOURCE=.\libz\zlib123\uncompr.c
# End Source File
# Begin Source File

SOURCE=.\libz\zlib123\zutil.c
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "jpeg headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\cderror.h"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\cdjpeg.h"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jchuff.h"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jconfig.h"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jdct.h"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jdhuff.h"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jinclude.h"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jmemsys.h"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jmorecfg.h"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jpegint.h"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jpeglib.h"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\jversion.h"
# End Source File
# Begin Source File

SOURCE=".\libjpeg\jpeg-6b\transupp.h"
# End Source File
# End Group
# Begin Group "tiff headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\t4.h"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_config.h"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_dir.h"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_fax3.h"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tif_predict.h"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tiff.h"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tiffconf.h"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tiffio.h"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tiffiop.h"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\tiffvers.h"
# End Source File
# Begin Source File

SOURCE=".\tiff-4.0\libtiff\uvcode.h"
# End Source File
# End Group
# Begin Group "libz headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libz\zlib123\crc32.h
# End Source File
# Begin Source File

SOURCE=.\libz\zlib123\deflate.h
# End Source File
# Begin Source File

SOURCE=.\libz\zlib123\inffast.h
# End Source File
# Begin Source File

SOURCE=.\libz\zlib123\inffixed.h
# End Source File
# Begin Source File

SOURCE=.\libz\zlib123\inflate.h
# End Source File
# Begin Source File

SOURCE=.\libz\zlib123\inftrees.h
# End Source File
# Begin Source File

SOURCE=.\libz\zlib123\trees.h
# End Source File
# Begin Source File

SOURCE=.\libz\zlib123\zconf.h
# End Source File
# Begin Source File

SOURCE=.\libz\zlib123\zconf.in.h
# End Source File
# Begin Source File

SOURCE=.\libz\zlib123\zlib.h
# End Source File
# Begin Source File

SOURCE=.\libz\zlib123\zutil.h
# End Source File
# End Group
# End Group
# Begin Group "reference headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\includes\jconfig.h
# End Source File
# Begin Source File

SOURCE=.\includes\jerror.h
# End Source File
# Begin Source File

SOURCE=.\includes\jmorecfg.h
# End Source File
# Begin Source File

SOURCE=.\includes\jpeglib.h
# End Source File
# Begin Source File

SOURCE=.\includes\tiff.h
# End Source File
# Begin Source File

SOURCE=.\includes\tiffconf.h
# End Source File
# Begin Source File

SOURCE=.\includes\tiffio.h
# End Source File
# Begin Source File

SOURCE=.\includes\tiffvers.h
# End Source File
# Begin Source File

SOURCE=.\includes\zconf.h
# End Source File
# Begin Source File

SOURCE=.\includes\zlib.h
# End Source File
# End Group
# End Target
# End Project
