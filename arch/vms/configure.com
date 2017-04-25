$! configure for GRACE -- VMS version
$! Rolf Niepraschk, 12/97, niepraschk@ptb.de
$! John Hasstedt, 12/98, John.Hasstedt@sunysb.edu
$!
$ echo := WRITE SYS$OUTPUT
$!
$! get versions, hardware type, etc
$!
$ VMSVERSION = F$GETSYI ("NODE_SWVERS")
$ VMS_MAJOR = F$ELEMENT (0, ".", VMSVERSION) - "V"
$ VMS_MINOR = F$ELEMENT (0, "-", F$ELEMENT (1, ".", VMSVERSION))
$ HW = F$GETSYI("ARCH_NAME")
$ @SYS$UPDATE:DECW$GET_IMAGE_VERSION SYS$SHARE:DECW$XLIBSHR.EXE DECWVERSION
$ DECWVERSION = F$ELEMENT (1, " ", DECWVERSION)
$ DECW_MAJOR = F$ELEMENT (0, "-", DECWVERSION)
$ @SYS$UPDATE:DECW$GET_IMAGE_VERSION SYS$SYSTEM:DECC$COMPILER.EXE DECCVERSION
$ DECCVERSION = F$ELEMENT (1, " ", DECCVERSION)
$ DECC_MAJOR = F$ELEMENT (0, ".", DECCVERSION) - "V"
$!
$! set defaults for command line parameters
$!
$ IF (F$SEARCH("SYS$LIBRARY:DECC$CRTL.EXE") .NES. "")
$ THEN DECC$CRTLSHR = "Yes"
$ ELSE DECC$CRTLSHR = "No"
$ ENDIF
$ IF (VMS_MAJOR .LT. 7)
$ THEN DECC$CRTL = DECC$CRTLSHR
$ ELSE DECC$CRTL = "No"
$ ENDIF
$ IF (F$SEARCH("SYS$LIBRARY:DPML$SHR.EXE") .NES. "")
$ THEN DPMLSHR = "Yes"
$ ELSE DPMLSHR = "No"
$ ENDIF
$ DPML = DPMLSHR
$ OPTIMIZE = "Yes"
$ IF (HW .EQS. "Alpha")
$ THEN FLOAT = "IEEE"
$ ELSE FLOAT = "G_FLOAT"
$ ENDIF
$ HOME = ""
$ PRINT = ""
$ QUEUE = "decw$printer_format_ps"
$ EDIT = "edit/tpu/display=motif"
$ HELP = "mosaic"
$ FFTWINC = ""
$ FFTWLIB = ""
$ NETCDFINC = ""
$ NETCDFLIB = ""
$ JPEGINC = ""
$ JPEGLIB = ""
$ ZINC = ""
$ ZLIB = ""
$ PNGINC = ""
$ PNGLIB = ""
$ TIFFINC = ""
$ TIFFLIB = ""
$ PDFINC = ""
$ PDFLIB = ""
$ FORCECOPY = 0
$ SAVE = 0
$!
$! read saved information and add command line parameters
$!
$ N = 1
$ SAVEFILE = F$ELEMENT (0, "]", F$ENVIRONMENT ("PROCEDURE")) + "]SAVED.DAT"
$ IF (F$SEARCH(SAVEFILE) .EQS. "") THEN GOTO NO_SAVEFILE
$ echo ""
$ echo "Using saved information"
$ OPEN/READ IN 'SAVEFILE'
$LOOP_SAVEFILE:
$ READ/END=DONE_SAVEFILE IN PAR'N'
$ N = N + 1
$ GOTO LOOP_SAVEFILE
$DONE_SAVEFILE:
$ CLOSE IN
$NO_SAVEFILE:
$ PAR'N' = P1
$ N = N + 1
$ PAR'N' = P2
$ N = N + 1
$ PAR'N' = P3
$ N = N + 1
$ PAR'N' = P4
$ N = N + 1
$ PAR'N' = P5
$ N = N + 1
$ PAR'N' = P6
$ N = N + 1
$ PAR'N' = P7
$ N = N + 1
$ PAR'N' = P8
$ N = N + 1
$ PAR'N' = ""
$ N = 0
$LOOP_PARAM:
$ N = N + 1
$ P = F$ELEMENT (0, "=", PAR'N')
$ IF (P .EQS. "") THEN GOTO DONE_PARAM
$ IF (P .EQS. "DPML")
$ THEN
$   DPML = DPMLSHR
$   GOTO LOOP_PARAM
$ ENDIF
$ IF (P .EQS. "NODPML")
$ THEN
$   DPML = "No"
$   GOTO LOOP_PARAM
$ ENDIF
$ IF (P .EQS. "DECC$CRTL")
$ THEN
$   DECC$CRTL = DECC$CRTLSHR
$   GOTO LOOP_PARAM
$ ENDIF
$ IF (P .EQS. "NODECC$CRTL")
$ THEN
$   DECC$CRTL = "No"
$   GOTO LOOP_PARAM
$ ENDIF
$ IF (P .EQS. "OPTIMIZE")
$ THEN
$   OPTIMIZE = "Yes"
$   GOTO LOOP_PARAM
$ ENDIF
$ IF (P .EQS. "NOOPTIMIZE")
$ THEN
$   OPTIMIZE = "No"
$   GOTO LOOP_PARAM
$ ENDIF
$ IF (P .EQS. "D_FLOAT")
$ THEN
$   FLOAT = "D_FLOAT"
$   GOTO LOOP_PARAM
$ ENDIF
$ IF (P .EQS. "G_FLOAT")
$ THEN
$   FLOAT = "G_FLOAT"
$   GOTO LOOP_PARAM
$ ENDIF
$ IF (P .EQS. "IEEE")
$ THEN
$   IF (HW .EQS. "VAX")
$   THEN
$     echo ""
$     echo "Ignoring IEEE option on VAX"
$   ELSE
$     FLOAT = "IEEE"
$   ENDIF
$   GOTO LOOP_PARAM
$ ENDIF
$ IF (P .EQS. "HOME")
$ THEN
$   HOME = PAR'N' - "HOME="
$   IF (F$EXTRACT(0,1,HOME) .EQS. """") THEN -
        HOME = F$EXTRACT(1,F$LENGTH(HOME)-1,HOME)
$   IF (F$EXTRACT(F$LENGTH(HOME)-1,1,HOME) .EQS. """") THEN -
        HOME = F$EXTRACT(0,F$LENGTH(HOME)-1,HOME)
$   GOTO LOOP_PARAM
$ ENDIF
$ IF (P .EQS. "PRINT")
$ THEN
$   PRINT = PAR'N' - "PRINT="
$   IF (F$EXTRACT(0,1,PRINT) .EQS. """") THEN -
        PRINT = F$EXTRACT(1,F$LENGTH(PRINT)-1,PRINT)
$   IF (F$EXTRACT(F$LENGTH(PRINT)-1,1,PRINT) .EQS. """") THEN -
        PRINT = F$EXTRACT(0,F$LENGTH(PRINT)-1,PRINT)
$   GOTO LOOP_PARAM
$ ENDIF
$ IF (P .EQS. "QUEUE")
$ THEN
$   QUEUE = PAR'N' - "QUEUE="
$   IF (F$EXTRACT(0,1,QUEUE) .EQS. """") THEN -
        QUEUE = F$EXTRACT(1,F$LENGTH(QUEUE)-1,QUEUE)
$   IF (F$EXTRACT(F$LENGTH(QUEUE)-1,1,QUEUE) .EQS. """") THEN -
        QUEUE = F$EXTRACT(0,F$LENGTH(QUEUE)-1,QUEUE)
$   GOTO LOOP_PARAM
$ ENDIF
$ IF (P .EQS. "EDIT")
$ THEN
$   EDIT = PAR'N' - "EDIT="
$   IF (F$EXTRACT(0,1,EDIT) .EQS. """") THEN -
        EDIT = F$EXTRACT(1,F$LENGTH(EDIT)-1,EDIT)
$   IF (F$EXTRACT(F$LENGTH(EDIT)-1,1,EDIT) .EQS. """") THEN -
        EDIT = F$EXTRACT(0,F$LENGTH(EDIT)-1,EDIT)
$   GOTO LOOP_PARAM
$ ENDIF
$ IF (P .EQS. "HELP")
$ THEN
$   HELP = PAR'N' - "HELP="
$   IF (F$EXTRACT(0,1,HELP) .EQS. """") THEN -
        HELP = F$EXTRACT(1,F$LENGTH(HELP)-1,HELP)
$   IF (F$EXTRACT(F$LENGTH(HELP)-1,1,HELP) .EQS. """") THEN -
        HELP = F$EXTRACT(0,F$LENGTH(HELP)-1,HELP)
$   GOTO LOOP_PARAM
$ ENDIF
$ IF (P .EQS. "FFTW")
$ THEN
$   FFTWINC = F$ELEMENT (1, "=", PAR'N')
$   FFTWLIB = F$ELEMENT (2, "=", PAR'N') - "="
$   GOTO LOOP_PARAM
$ ENDIF
$ IF (P .EQS. "NETCDF")
$ THEN
$   NETCDFINC = F$ELEMENT (1, "=", PAR'N')
$   NETCDFLIB = F$ELEMENT (2, "=", PAR'N') - "="
$   GOTO LOOP_PARAM
$ ENDIF
$ IF (P .EQS. "JPEG")
$ THEN
$   JPEGINC = F$ELEMENT (1, "=", PAR'N')
$   JPEGLIB = F$ELEMENT (2, "=", PAR'N') - "="
$   GOTO LOOP_PARAM
$ ENDIF
$ IF (P .EQS. "ZLIB")
$ THEN
$   ZINC = F$ELEMENT (1, "=", PAR'N')
$   ZLIB = F$ELEMENT (2, "=", PAR'N') - "="
$   GOTO LOOP_PARAM
$ ENDIF
$ IF (P .EQS. "PNG")
$ THEN
$   PNGINC = F$ELEMENT (1, "=", PAR'N')
$   PNGLIB = F$ELEMENT (2, "=", PAR'N') - "="
$   GOTO LOOP_PARAM
$ ENDIF
$ IF (P .EQS. "TIFF")
$ THEN
$   TIFFINC = F$ELEMENT (1, "=", PAR'N')
$   TIFFLIB = F$ELEMENT (2, "=", PAR'N') - "="
$   GOTO LOOP_PARAM
$ ENDIF
$ IF (P .EQS. "PDF")
$ THEN
$   PDFINC = F$ELEMENT (1, "=", PAR'N')
$   PDFLIB = F$ELEMENT (2, "=", PAR'N') - "="
$   GOTO LOOP_PARAM
$ ENDIF
$ IF (P .EQS. "COPY")
$ THEN
$   FORCECOPY = 1
$   GOTO LOOP_PARAM
$ ENDIF
$ IF (P .EQS. "SAVE")
$ THEN
$   SAVE = 1
$   GOTO LOOP_PARAM
$ ENDIF
$ echo "Unrecognized option: ", P
$ EXIT
$DONE_PARAM:
$ IF (FFTWINC .NES. "")
$ THEN
$   FFTWLIB = F$PARSE (FFTWLIB, "''FFTWINC'FFTW.OLB",,, "SYNTAX_ONLY") - ";"
$ ENDIF
$ IF (NETCDFINC .NES. "" .AND. NETCDFLIB .EQS. "")
$ THEN
$   echo "You must specify both the include directory and the libraries"
$   echo "with the NETCDF option."
$   EXIT
$ ENDIF
$ IF (JPEGINC .NES. "")
$ THEN
$   JPEGLIB = F$PARSE (JPEGLIB, "''JPEGINC'LIBJPEG.OLB",,, "SYNTAX_ONLY") - ";"
$ ENDIF
$ IF (ZINC .NES. "")
$ THEN
$   ZLIB = F$PARSE (ZLIB, "''ZINC'LIBZ.OLB",,, "SYNTAX_ONLY") - ";"
$ ENDIF
$ IF (PNGINC .NES. "")
$ THEN
$   PNGLIB = F$PARSE (PNGLIB, "''PNGINC'LIBPNG.OLB",,, "SYNTAX_ONLY") - ";"
$ ENDIF
$ IF (ZLIB .EQS. "" .AND. PNGLIB .NES. "")
$ THEN
$   echo "You must include ZLIB if you want PNG."
$   EXIT
$ ENDIF
$ IF (TIFFINC .NES. "")
$ THEN
$   TIFFLIB = F$PARSE (TIFFLIB, "''TIFFINC'TIFF.OLB",,, "SYNTAX_ONLY") - ";"
$ ENDIF
$ IF (PDFINC .NES. "")
$ THEN
$   PDFLIB = F$PARSE (PDFLIB, "''PDFINC'PDFLIB.OLB",,, "SYNTAX_ONLY") - ";"
$ ENDIF
$ IF (SAVE)
$ THEN
$   IF (F$SEARCH(SAVEFILE) .EQS. "")
$   THEN
$     OPEN/WRITE OUT 'SAVEFILE'
$   ELSE
$     OPEN/APPEND OUT 'SAVEFILE'
$   ENDIF
$   IF (P1 .NES. "" .AND. P1 .NES. "SAVE") THEN WRITE OUT P1
$   IF (P2 .NES. "" .AND. P2 .NES. "SAVE") THEN WRITE OUT P2
$   IF (P3 .NES. "" .AND. P3 .NES. "SAVE") THEN WRITE OUT P3
$   IF (P4 .NES. "" .AND. P4 .NES. "SAVE") THEN WRITE OUT P4
$   IF (P5 .NES. "" .AND. P5 .NES. "SAVE") THEN WRITE OUT P5
$   IF (P6 .NES. "" .AND. P6 .NES. "SAVE") THEN WRITE OUT P6
$   IF (P7 .NES. "" .AND. P7 .NES. "SAVE") THEN WRITE OUT P7
$   IF (P8 .NES. "" .AND. P8 .NES. "SAVE") THEN WRITE OUT P8
$   CLOSE OUT
$ ENDIF
$ IF (PRINT .EQS. "") THEN -
    PRINT = "print/name=""from Grace""/delete/queue=" + QUEUE
$ IF (F$LOCATE("%s",HELP) .EQ. F$LENGTH(HELP)) THEN HELP = HELP + " %s"
$!
$! Define the __CRTL_VER symbol.
$!
$ IF (DECC$CRTL) THEN DEFINE/USER DECC$CRTLMAP SYS$LIBRARY:DECC$CRTL.EXE
$ CC/OBJECT=DEFINE_CRTL_VER.OBJ SYS$INPUT
#include <stdlib.h>
#include <stdio.h>
#include <descrip.h>
#include <lib$routines.h>
#ifndef __CRTL_VER
#   define __CRTL_VER __VMS_VER
#endif
main () {
static $DESCRIPTOR(crtl,"__CRTL_VER");
struct dsc$descriptor_s val = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};
static int tab = {1};
char str[10];
val.dsc$w_length = sprintf (str, "%d", __CRTL_VER);
val.dsc$a_pointer = str;
exit (lib$set_symbol (&crtl, &val, &tab));
}
$ LINK/EXECUTABLE=DEFINE_CRTL_VER.EXE DEFINE_CRTL_VER.OBJ
$ RUN DEFINE_CRTL_VER.EXE
$ DELETE DEFINE_CRTL_VER.OBJ;*,DEFINE_CRTL_VER.EXE;*
$ __CRTL_VER = F$INTEGER(__CRTL_VER)
$!
$! Write the configureation.
$!
$ OPEN/WRITE OUT CONFIGURE.LOG
$ WRITE OUT "Configuration of GRACE for VMS on ", F$GETSYI("NODENAME"), -
            " at ", F$TIME()
$ WRITE OUT ""
$ WRITE OUT "VMS version:       ", VMSVERSION
$ WRITE OUT "Architecture:      ", HW
$ WRITE OUT "GUI:               Motif ", DECWVERSION
$ WRITE OUT "DECC version:      ", DECCVERSION
$ WRITE OUT "Use DECC$CRTL.OLB: ", DECC$CRTL
$ WRITE OUT "CRTL version:      ", __CRTL_VER
$ WRITE OUT "Use DPML:          ", DPML
$ WRITE OUT "Optimize:          ", OPTIMIZE
$ WRITE OUT "Floating point:    ", FLOAT
$ WRITE OUT "Home directory:    ", HOME
$ WRITE OUT "Print command:     ", PRINT
$ WRITE OUT "Edit command:      ", EDIT
$ WRITE OUT "Help viewer:       ", HELP
$ IF (FFTWLIB .EQS. "")
$ THEN
$   WRITE OUT "FFTW:              Not used"
$ ELSE
$   WRITE OUT "FFTW:              Include dir: ", FFTWINC
$   WRITE OUT "                   Library:     ", FFTWLIB
$ ENDIF
$ IF (NETCDFLIB .EQS. "")
$ THEN
$   WRITE OUT "NetCDF:            Not used"
$ ELSE
$   WRITE OUT "NetCDF:            Include dir: ", NETCDFINC
$   WRITE OUT "                   Libraries:   ", NETCDFLIB
$ ENDIF
$ IF (JPEGLIB .EQS. "")
$ THEN
$   WRITE OUT "JPEG:              Not used"
$ ELSE
$   WRITE OUT "JPEG:              Include dir: ", JPEGINC
$   WRITE OUT "                   Library:     ", JPEGLIB
$ ENDIF
$ IF (ZLIB .EQS. "")
$ THEN
$   WRITE OUT "ZLIB:              Not used"
$ ELSE
$   WRITE OUT "ZLIB:              Include dir: ", ZINC
$   WRITE OUT "                   Library:     ", ZLIB
$ ENDIF
$ IF (PNGLIB .EQS. "")
$ THEN
$   WRITE OUT "PNG:               Not used"
$ ELSE
$   WRITE OUT "PNG:               Include dir: ", PNGINC
$   WRITE OUT "                   Library:     ", PNGLIB
$ ENDIF
$ IF (TIFFLIB .EQS. "")
$ THEN
$   WRITE OUT "TIFF:              Not used"
$ ELSE
$   WRITE OUT "TIFF:              Include dir: ", TIFFINC
$   WRITE OUT "                   Library:     ", TIFFLIB
$ ENDIF
$ IF (PDFLIB .EQS. "")
$ THEN
$   WRITE OUT "PDF:               Not used"
$ ELSE
$   WRITE OUT "PDF:               Include dir: ", PDFINC
$   WRITE OUT "                   Library:     ", PDFLIB
$ ENDIF
$ CLOSE OUT
$ echo ""
$ TYPE/NOPAGE CONFIGURE.LOG
$ echo ""
$!
$! define symbols for the other directories
$!
$ MAIN_DIR := [--]
$ CEPHES_DIR := [--.CEPHES]
$ T1LIB_DIR := [--.T1LIB]
$ T1LIB_T1LIB_DIR := [--.T1LIB.T1LIB]
$ T1LIB_TYPE1_DIR := [--.T1LIB.TYPE1]
$ XBAE_DIR := [--.XBAE.XBAE]
$ SRC_DIR := [--.SRC]
$ GRCONVERT_DIR := [--.GRCONVERT]
$ EXAMPLES_DIR := [--.EXAMPLES]
$ CONF_DIR := [--.AC-TOOLS]
$!
$! save the current directory and set default to the vms directory
$!
$ CURDIR = F$ENVIRONMENT ("DEFAULT")
$ VMSDIR = F$ELEMENT (0, "]", F$ENVIRONMENT ("PROCEDURE")) + "]"
$ SET DEFAULT 'VMSDIR'
$!
$! copy files to other directories
$!
$ IF (FORCECOPY .OR. F$SEARCH("''MAIN_DIR'DESCRIP.MMS") .EQS. "") THEN -
      COPY DESCRIP.MMS 'MAIN_DIR'DESCRIP.MMS
$ IF (FORCECOPY .OR. F$SEARCH("''CEPHES_DIR'DESCRIP.MMS") .EQS. "") THEN -
      COPY CEPHES.MMS 'CEPHES_DIR'DESCRIP.MMS
$ IF (FORCECOPY .OR. F$SEARCH("''T1LIB_DIR'DESCRIP.MMS") .EQS. "") THEN -
      COPY T1LIB.MMS 'T1LIB_DIR'DESCRIP.MMS
$ IF (FORCECOPY .OR. F$SEARCH("''T1LIB_T1LIB_DIR'DESCRIP.MMS") .EQS. "") THEN -
      COPY T1LIB_T1LIB.MMS 'T1LIB_T1LIB_DIR'DESCRIP.MMS
$ IF (FORCECOPY .OR. F$SEARCH("''T1LIB_TYPE1_DIR'DESCRIP.MMS") .EQS. "") THEN -
      COPY T1LIB_TYPE1.MMS 'T1LIB_TYPE1_DIR'DESCRIP.MMS
$ IF (FORCECOPY .OR. F$SEARCH("''XBAE_DIR'DESCRIP.MMS") .EQS. "") THEN -
      COPY XBAE.MMS 'XBAE_DIR'DESCRIP.MMS
$ IF (FORCECOPY .OR. F$SEARCH("''SRC_DIR'DESCRIP.MMS") .EQS. "") THEN -
      COPY SRC.MMS 'SRC_DIR'DESCRIP.MMS
$ IF (FORCECOPY .OR. F$SEARCH("''GRCONVERT_DIR'DESCRIP.MMS") .EQS. "") THEN -
      COPY GRCONVERT.MMS 'GRCONVERT_DIR'DESCRIP.MMS
$ IF (FORCECOPY .OR. F$SEARCH("''EXAMPLES_DIR'DOTEST.COM") .EQS. "") THEN -
      COPY DOTEST.COM 'EXAMPLES_DIR'DOTEST.COM
$!
$! Copy the default font encoding file.
$!
$ IF (F$SEARCH("[--.FONTS.ENC]DEFAULT.ENC") .EQS. "") THEN -
      COPY [--.FONTS.ENC]ISOLATIN1.ENC [--.FONTS.ENC]DEFAULT.ENC
$!
$! Rename files in [.DOC].
$!
$RENAMEDOC:
$ FILE = F$SEARCH ("[--.DOC]*.*_*")
$ IF (FILE .NES. "")
$ THEN
$   NAME = F$PARSE (FILE,,, "NAME")
$   TYPE = F$PARSE (FILE,,, "TYPE") - "."
$   REAL = F$ELEMENT (3, "_", TYPE)
$   IF (REAL .EQS. "_")
$   THEN
$     REAL = F$ELEMENT (2, "_", TYPE)
$     IF (REAL .EQS. "_")
$     THEN
$       REAL = F$ELEMENT (1, "_", TYPE)
$     ENDIF
$   ENDIF
$   N = F$LENGTH (TYPE) - F$LENGTH (REAL) - 1
$   NAME = NAME + "_" + F$EXTRACT (0, N, TYPE)
$   RENAME 'FILE' [--.DOC]'NAME'.'REAL'
$   GOTO RENAMEDOC
$ ENDIF
$!
$! Define symbols for make.conf.  These symbols are in make.conf_in; they
$! are set to the value they should be in make.conf.
$!
$ O = ".obj"
$ EXE = ".exe"
$ BAT = ".com"
$ SHELL = ""
$ PREFIX = ""
$ SUBDIRS = "cephes t1lib xbae src"
$ GRACE = "xmgrace$(EXE)"
$ GRACE_HOME = HOME
$ IF (HW .EQS. "Alpha" .OR. DECC_MAJOR .GE. 6)
$ THEN ALLOCA = ""
$ ELSE ALLOCA = "alloca$(O)"
$ ENDIF
$ T1_LIB = ",[-.T1LIB]libt1lib.olb/LIBRARY"
$ T1_INC = ",[-.T1LIB.T1LIB]"
$ T1_AA_TYPE16 = "short"
$ T1_AA_TYPE32 = "int"
$ T1_AA_TYPE64 = ""
$ XDR_LIB = ""
$ DL_LIB = ""
$ IF (FFTWLIB .NES. "")
$ THEN FFTW_LIB = "," + FFTWLIB + "/LIBRARY"
$ ELSE FFTW_LIB = ""
$ ENDIF
$ NETCDF_LIBS = ""
$ IF (NETCDFLIB .EQS. "") THEN GOTO DONE_NETCDF_LIBS
$ N = 0
$LOOP_NETCDF_LIBS:
$ LIB = F$ELEMENT (N, ",", NETCDFLIB)
$ IF (LIB .EQS. ",") THEN GOTO DONE_NETCDF_LIBS
$ NETCDF_LIBS = NETCDF_LIBS + "," + LIB + "/LIBRARY"
$ N = N + 1
$ GOTO LOOP_NETCDF_LIBS
$DONE_NETCDF_LIBS:
$ IF (JPEGLIB .NES. "")
$ THEN JPEG_LIB = "," + JPEGLIB + "/LIBRARY"
$ ELSE JPEG_LIB = ""
$ ENDIF
$ IF (ZLIB .NES. "")
$ THEN Z_LIB = "," + ZLIB + "/LIBRARY"
$ ELSE Z_LIB = ""
$ ENDIF
$ IF (PNGLIB .NES. "")
$ THEN PNG_LIB = "," + PNGLIB + "/LIBRARY"
$ ELSE PNG_LIB = ""
$ ENDIF
$ IF (TIFFLIB .NES. "")
$ THEN TIFF_LIB = "," + TIFFLIB + "/LIBRARY"
$ ELSE TIFF_LIB = ""
$ ENDIF
$ IF (PDFLIB .NES. "")
$ THEN
$   PDF_LIB = "," + PDFLIB + "/LIBRARY"
$   PDFDRV_O = "pdfdrv$(O)"
$ ELSE
$   PDF_LIB = ""
$   PDFDRV_O = ""
$ ENDIF
$ XBAE_INC = "[-.XBAE.XBAE]"
$ YACC = ""
$ CC = "cc"
$ FC = "fortran"
$ AR = "library"
$ RANLIB = ""
$ RM = "delete/log"
$ LN_S = ""
$ INSTALL = ""
$ INSTALL_PROGRAM = ""
$ INSTALL_DATA = ""
$ MKINSTALLDIRS = ""
$ CPPFLAGS = ""
$ IF (DPMLSHR .AND. .NOT. DPML)
$ THEN
$   CFLAGS0 = "/PREFIX=(ALL,EXCEPT=(CBRT,LOG2,RINT,ASINH,ACOSH,ATANH," -
            + "ERF,ERFC,J0,J1,JN,Y0,Y1,YN))"
$ ELSE
$   CFLAGS0 = "/PREFIX=ALL"
$ ENDIF
$ CFLAGS0 = CFLAGS0 + "/FLOAT=" + FLOAT
$ IF (.NOT. OPTIMIZE) THEN CFLAGS0 = CFLAGS0 + "/NOOPTIMIZE"
$ GUI_FLAGS = ""
$ LDFLAGS = ""
$ IF (DECC$CRTL)
$ THEN
$   NOGUI_LIBS = ",sys$library:decc$crtl.olb/LIBRARY"
$   IF (HW .EQS. "VAX") THEN -
        NOGUI_LIBS = NOGUI_LIBS + ",sys$library:vaxc$lcl.opt/OPTION"
$ ELSE
$   NOGUI_LIBS = ""
$ ENDIF
$ GUI_LIBS = ",[-.XBAE.XBAE]libxbae.olb/LIBRARY,[-.ARCH.VMS]motif1_2.opt/OPTION"
$ PRINT_CMD = F$ELEMENT (0, """", PRINT)
$ N = 1
$LOOP_PRINT_CMD:
$ P = F$ELEMENT (N, """", PRINT)
$ IF (P .NES. """")
$ THEN
$   PRINT_CMD = PRINT_CMD + "\\042" + P
$   N = N + 1
$   GOTO LOOP_PRINT_CMD
$ ENDIF
$ GRACE_EDITOR = EDIT
$ HELPVIEWER = HELP
$!
$! create make.conf
$!
$ echo "Creating make.conf"
$ OPEN/READ IN 'CONF_DIR'MAKE.CONF_IN
$ OPEN/WRITE OUT 'MAIN_DIR'MAKE.CONF
$LOOP_MAKE_CONF:
$ READ/END=DONE_MAKE_CONF IN REC
$ IF (F$LOCATE("=",REC) .NE. F$LENGTH(REC))
$ THEN
$   SYM = F$ELEMENT(0,"=",REC)
$   IF (F$TYPE('SYM') .EQS. "")
$   THEN
$     WRITE SYS$OUTPUT "No DCL symbol for ", REC
$   ELSE
$     REC = SYM + "=" + 'SYM'
$   ENDIF
$ ELSE
$   IF (REC .EQS. ".SUFFIXES:") THEN REC = "#.SUFFIXES:"  ! allow make rules
$ ENDIF
$ WRITE OUT REC
$ GOTO LOOP_MAKE_CONF
$DONE_MAKE_CONF:
$ CLOSE IN
$ LIB_INC = ""
$ IF (FFTWINC   .NES. "") THEN LIB_INC = LIB_INC + "," + FFTWINC
$ IF (NETCDFINC .NES. "") THEN LIB_INC = LIB_INC + "," + NETCDFINC
$ IF (JPEGINC   .NES. "") THEN LIB_INC = LIB_INC + "," + JPEGINC
$ IF (ZINC      .NES. "") THEN LIB_INC = LIB_INC + "," + ZINC
$ IF (PNGINC    .NES. "") THEN LIB_INC = LIB_INC + "," + PNGINC
$ IF (TIFFINC   .NES. "") THEN LIB_INC = LIB_INC + "," + TIFFINC
$ IF (PDFINC    .NES. "") THEN LIB_INC = LIB_INC + "," + PDFINC
$ WRITE OUT ""
$ WRITE OUT "# Library include directories"
$ WRITE OUT "LIB_INC=", LIB_INC
$ WRITE OUT ""
$ WRITE OUT "# Use DECC$CRTL.OLB object library"
$ IF (DECC$CRTL)
$ THEN WRITE OUT "USE_DECC$CRTL=1"
$ ELSE WRITE OUT "#USE_DECC$CRTL=1"
$ ENDIF
$ WRITE OUT ""
$ WRITE OUT "# C compiler"
$ CCOMPILER = "DECC " + DECCVERSION
$ IF (DECC$CRTL) THEN CCOMPILER = CCOMPILER + "/DECC$CRTL.OLB"
$ IF (DPML)
$ THEN CCOMPILER = CCOMPILER + "/DPML"
$ ELSE CCOMPILER = CCOMPILER + "/No DPML"
$ ENDIF
$ IF (.NOT. OPTIMIZE) THEN CCOMPILER = CCOMPILER + "/No Optimize"
$ CCOMPILER = CCOMPILER + "/" + FLOAT
$ IF (NETCDFINC .NES. "") THEN CCOMPILER = CCOMPILER + "/NETCDF"
$ WRITE OUT "CCOMPILER=", CCOMPILER
$ CLOSE OUT
$!
$! define symbols for config.h
$! These symbols are in config.h_in; if the DCL symbol is equal to 0,
$! the symbol should be undefined in config.h; otherwise, it should be
$! defined to the value of the DCL symbol.  I define all values in DCL
$! (instead of just those that need to be defined in config.h) so I can
$! check when symbols are added to config.h_in.
$!
$ _ALL_SOURCE = 0
$ _POSIX_SOURCE = 0
$ STDC_HEADERS = 1
$ __CHAR_UNSIGNED__ = 0
$ SIZEOF_CHAR = "sizeof(char)"
$ SIZEOF_SHORT = "sizeof(short)"
$ SIZEOF_INT = "sizeof(int)"
$ SIZEOF_LONG = "sizeof(long)"
$ IF (HW .EQS. "Alpha")
$ THEN SIZEOF_LONG_LONG = "sizeof(long long)"
$ ELSE SIZEOF_LONG_LONG = "0"
$ ENDIF
$ SIZEOF_FLOAT = "sizeof(float)"
$ SIZEOF_DOUBLE = "sizeof(double)"
$ IF (HW .EQS. "Alpha")
$ THEN SIZEOF_LONG_DOUBLE = "sizeof(long double)"
$ ELSE SIZEOF_LONG_DOUBLE = "0"
$ ENDIF
$ SIZEOF_VOID_P = "sizeof(void *)"
$ const = 0
$ pid_t = 0
$ size_t = 0
$ HAVE_UNISTD_H = 1
$ CRAY_STACKSEG_END = 0
$ HAVE_ALLOCA = (HW .EQS. "Alpha") .OR. (DECC_MAJOR .GE. 6)
$ C_ALLOCA = (.NOT. HAVE_ALLOCA) .AND. 1
$ HAVE_ALLOCA_H = 0
$ RETSIGTYPE = "void"
$ HAVE_SYS_WAIT_H = 1
$ HAVE_FCNTL_H = 1
$ HAVE_SYS_PARAM_H = 0
$ HAVE_SYS_TIME_H = 1
$ HAVE_SYS_SELECT_H = 0
$ TM_IN_SYS_TIME = 1
$ TIME_WITH_SYS_TIME = 1
$ HAVE_GETTIMEOFDAY = __CRTL_VER .GE. 70000000
$ HAVE_GETCWD = 1
$ HAVE_GETHOSTNAME = __CRTL_VER .GE. 50500000
$ HAVE_MEMCPY = 1
$ HAVE_MEMMOVE = 1
$ HAVE_UNLINK = 0
$ HAVE_FCNTL = __CRTL_VER .GE. 70200000
$ HAVE_POPEN = __CRTL_VER .GE. 70000000
$ HAVE_FNMATCH = 0
$ HAVE_ON_EXIT = 0
$ HAVE_STRSTR = 1
$ HAVE_STRERROR = 1
$ HAVE_SYS_ERRLIST_DECL = 0
$ HAVE_VSNPRINTF = 0
$ HAVE_DLOPEN = 0
$ HAVE_RTLD_NOW = 0
$ HAVE_SHL_LOAD = 0
$ WORDS_BIGENDIAN = 0
$ HAVE_DEC_FPU = FLOAT .NES. "IEEE"
$ HAVE_LIEEE_FPU = (.NOT. HAVE_DEC_FPU) .AND. 1
$ HAVE_BIEEE_FPU = 0
$ REALLOC_IS_BUGGY = 0
$ HAVE_DRAND48 = __CRTL_VER .GE. 70000000
$ HAVE_SETLOCALE = __CRTL_VER .GE. 60200000
$ HAVE_DRAND48_DECL = HAVE_DRAND48
$ HAVE_LIBM = 1
$ HAVE_MATH_H = 1
$ HAVE_FLOAT_H = 1
$ HAVE_IEEEFP_H = 0
$ HAVE_HYPOT = 1
$ HAVE_HYPOT_DECL = HAVE_HYPOT
$ HAVE_CBRT = F$INTEGER(DPML)
$ HAVE_CBRT_DECL = HAVE_CBRT
$ HAVE_LOG2 = F$INTEGER(DPML)
$ HAVE_LOG2_DECL = HAVE_LOG2
$ HAVE_RINT = F$INTEGER(DPML)
$ HAVE_RINT_DECL = HAVE_RINT
$ HAVE_LGAMMA = 0
$ HAVE_LGAMMA_DECL = HAVE_LGAMMA
$ HAVE_SIGNGAM_DECL = (HW .EQS. "Alpha")
$ HAVE_ASINH = F$INTEGER(DPML)
$ HAVE_ASINH_DECL = HAVE_ASINH
$ HAVE_ACOSH = F$INTEGER(DPML)
$ HAVE_ACOSH_DECL = HAVE_ACOSH
$ HAVE_ATANH = F$INTEGER(DPML)
$ HAVE_ATANH_DECL = HAVE_ATANH
$ HAVE_ERF = F$INTEGER(DPML)
$ HAVE_ERF_DECL = HAVE_ERF
$ HAVE_ERFC = F$INTEGER(DPML)
$ HAVE_ERFC_DECL = HAVE_ERFC
$ HAVE_FINITE = (HW .EQS. "Alpha")
$ HAVE_FINITE_DECL = HAVE_FINITE
$ HAVE_ISFINITE = 0
$ HAVE_ISFINITE_DECL = HAVE_ISFINITE
$ HAVE_ISNAN = (HW .EQS. "Alpha")
$ HAVE_ISNAN_DECL = HAVE_ISNAN
$ HAVE_J0 = F$INTEGER(DPML)
$ HAVE_J0_DECL = HAVE_J0
$ HAVE_J1 = F$INTEGER(DPML)
$ HAVE_J1_DECL = HAVE_J1
$ HAVE_JN = F$INTEGER(DPML)
$ HAVE_JN_DECL = HAVE_JN
$ HAVE_Y0 = F$INTEGER(DPML)
$ HAVE_Y0_DECL = HAVE_Y0
$ HAVE_Y1 = F$INTEGER(DPML)
$ HAVE_Y1_DECL = HAVE_Y1
$ HAVE_YN = F$INTEGER(DPML)
$ HAVE_YN_DECL = HAVE_YN
$ HAVE_NETCDF = NETCDFLIB .NES. ""
$ HAVE_FFTW = FFTWLIB .NES. ""
$ HAVE_LIBPNG = PNGLIB .NES. ""
$ HAVE_LIBJPEG = JPEGLIB .NES. ""
$ HAVE_LIBPDF = PDFLIB .NES. ""
$ WITH_F77_WRAPPER = 1
$ X_DISPLAY_MISSING = 0
$ HAVE_MOTIF = 1
$ HAVE_LESSTIF = 0
$ HAVE__XMVERSIONSTRING = 0
$ HAVE_XPM = 0
$ HAVE_XPM_H = 0
$ HAVE_X11_XPM_H = 0
$ WITH_LIBHELP = 0
$ WITH_XMHTML = 0
$ WITH_EDITRES = 0
$ PRINT_CMD_UNLINKS = 1
$ WITH_DEBUG = 0
$!
$! create config.h
$! Any lines beginning with #define SIZEOF or #undef are rewritten; all
$! other lines are copied to the output.
$!
$ echo "Creating config.h"
$ OPEN/READ IN 'CONF_DIR'CONFIG.H_IN
$ OPEN/WRITE OUT 'MAIN_DIR'CONFIG.H
$LOOP_CONFIG_H:
$ READ/END=DONE_CONFIG_H IN REC
$ IF (F$ELEMENT(0," ",REC) .EQS. "#define")   ! check for #define SIZEOF*
$ THEN
$   SYM = F$ELEMENT(1," ",REC)
$   IF (F$EXTRACT(0,6,SYM) .EQS. "SIZEOF")
$   THEN
$     IF (F$TYPE('SYM') .EQS. "")
$     THEN
$       WRITE SYS$OUTPUT "No DCL symbol for ", REC
$     ELSE
$       REC = "#define " + SYM + " " + 'SYM'
$     ENDIF
$   ENDIF
$ ELSE
$   IF (F$ELEMENT(0," ",REC) .EQS. "#undef")  ! check for #undef *
$   THEN
$     SYM = F$ELEMENT(1," ",REC)
$     IF (F$TYPE('SYM') .EQS. "")
$     THEN
$       WRITE SYS$OUTPUT "No DCL symbol for ", REC
$     ELSE
$       VAL = 'SYM'
$       IF (F$TYPE(VAL) .EQS. "STRING" .OR. VAL .NE. 0)
$       THEN
$         REC = "#define " + SYM + " " + F$STRING(VAL)
$       ENDIF
$     ENDIF
$   ENDIF
$ ENDIF
$ WRITE OUT REC
$ GOTO LOOP_CONFIG_H
$DONE_CONFIG_H:
$ CLOSE IN
$ CLOSE OUT
$!
$! restore directory and exit
$!
$ SET DEFAULT 'CURDIR'
$ EXIT
