dnl ACX_SAVE_STATE/ACX_RESTORE_STATE
dnl Save/restore flags
dnl 
dnl ACX_SAVE_STATE
AC_DEFUN(ACX_SAVE_STATE,
[
    save_CFLAGS=$CFLAGS
    save_CPPFLAGS=$CPPFLAGS
    save_LDFLAGS=$LDFLAGS
    save_LIBS=$LIBS
])
dnl ACX_RESTORE_STATE
AC_DEFUN(ACX_RESTORE_STATE,
[
    CFLAGS=$save_CFLAGS
    CPPFLAGS=$save_CPPFLAGS
    LDFLAGS=$save_LDFLAGS
    LIBS=$save_LIBS
])


AC_DEFUN(ACX_CHECK_CC_FLAGS,
[
AC_REQUIRE([AC_PROG_CC])
AC_CACHE_CHECK(whether ${CC-cc} accepts $1, ac_$2,
[echo 'void f(){}' > conftest.c
if test -z "`${CC-cc} $1 -c conftest.c 2>&1`"; then
	ac_$2=yes
else
	ac_$2=no
fi
rm -f conftest*
])
if test "$ac_$2" = yes; then
	:
	$3
else
	:
	$4
fi
])

dnl **** Check for gcc strength-reduce bug ****
AC_DEFUN(ACX_GCC_STRENGTH_REDUCE,
[
AC_REQUIRE([AC_PROG_CC])
AC_CACHE_CHECK([for gcc strength-reduce bug], ac_cv_c_gcc_strength_bug,
  AC_TRY_RUN([
    int main(void) {
      static int Array[[3]];
      unsigned int B = 3;
      int i;
      for(i=0; i<B; i++) Array[[i]] = i - 3;
      exit( Array[[1]] != -2 );
    }
    ],

    ac_cv_c_gcc_strength_bug="no",
    ac_cv_c_gcc_strength_bug="yes",
    ac_cv_c_gcc_strength_bug="yes")
  )
  
  if test "$ac_cv_c_gcc_strength_bug" = "yes"
  then
    :
    $1
  else
    :
    $2
  fi
])

dnl **** Checks for FPU arithmetics
AC_DEFUN(ACX_CHECK_FPU,
[
AC_CACHE_CHECK([for FPU arithmetics type], ac_cv_c_fpu_arithmetics_type,
                  AC_TRY_RUN([
#include <stdio.h>
#include <string.h>

#define LOG2EA 0.44269504088896340735992
#define ACCURACY "%1.4f"

typedef union {unsigned short s[[4]]; double d;} XTYPE;

XTYPE X[[]] = { 
              {{0,0,0,0}},			    /* Unknown             */
              {{0x3fdc,0x551d,0x94ae,0x0bf8}}, 	    /* Big endian     IEEE */
              {{0x0bf8,0x94ae,0x551d,0x3fdc}}, 	    /* Littile endian IEEE */
              {{0037742,0124354,0122560,0057703}}   /* DEC                 */
            };
            
int main (void)
{
    int i; char TMPSTR[[1024]]; char LOG2EA_STR[[80]];
    i = 0;
 
    sprintf(LOG2EA_STR, ACCURACY, LOG2EA);
 
    for (i=3; i >= 0; i--)
    {
      	sprintf(TMPSTR, ACCURACY, X[[i]].d);
      	if (strcmp(TMPSTR, LOG2EA_STR) == 0) {
    	    break;
      	}
    }
 
    exit(i);
}],
    ac_cv_c_fpu_arithmetics_type="Unknown",
    [case "$?" in
      "1"[)] ac_cv_c_fpu_arithmetics_type="Big endian IEEE" ;;
      "2"[)] ac_cv_c_fpu_arithmetics_type="Little endian IEEE" ;;
      "3"[)] ac_cv_c_fpu_arithmetics_type="DEC" ;;
    esac],
    ac_cv_c_fpu_arithmetics_type="Unknown") )

case "$ac_cv_c_fpu_arithmetics_type" in
  "DEC")                AC_DEFINE(HAVE_DEC_FPU) ;;
  "Little endian IEEE") AC_DEFINE(HAVE_LIEEE_FPU) ;;
  "Big endian IEEE")    AC_DEFINE(HAVE_BIEEE_FPU) ;;
esac

])


AC_DEFUN(ACX_ANSI_TYPES,
[
  dnl **** Check which ANSI integer type is 16 bit
  AC_CACHE_CHECK( "which ANSI integer type is 16 bit", ac_cv_16bit_type,
                  AC_TRY_RUN([
  int main(void) {
    if (sizeof(short)==2)
      return(0);
    else if (sizeof(int)==2)
      return(1);
    else
      return(2);
  }], ac_cv_16bit_type="short", ac_cv_16bit_type="int", ac_cv_16bit_type=))
  if test "$ac_cv_16bit_type" = "short"
  then
    T1_AA_TYPE16="short"
  else
    T1_AA_TYPE16="int"
  fi

  dnl **** Check which ANSI integer type is 32 bit
  AC_CACHE_CHECK( "which ANSI integer type is 32 bit", ac_cv_32bit_type,
                  AC_TRY_RUN([
  int main(void) {
    if (sizeof(int)==4)
      return(0);
    else if (sizeof(long)==4)
      return(1);
    else
      return(2);
  }], ac_cv_32bit_type="int", ac_cv_32bit_type="long", ac_cv_32bit_type=))
  if test "$ac_cv_32bit_type" = "int"
  then
    T1_AA_TYPE32="int"
  else
    T1_AA_TYPE32="long"
  fi

  dnl **** Check which ANSI integer type is 64 bit 
  AC_CACHE_CHECK( "which ANSI integer type is 64 bit", ac_cv_64bit_type,
                  AC_TRY_RUN([
  int main(void) {
    if (sizeof(long)==8)
      return(0);
    else
      return(1);
  }], ac_cv_64bit_type="long", ac_cv_64bit_type="<none>", ac_cv_64bit_type=))
  if test "$ac_cv_64bit_type" = "long"
  then
    T1_AA_TYPE64="long"
  else
    T1_AA_TYPE64=
  fi
])

dnl **** Check for buggy realloc()
AC_DEFUN(ACX_CHECK_REALLOC,
[
AC_CACHE_CHECK([whether realloc is buggy], ac_cv_c_realloc_bug,
                  AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
int main(void) {
  void *ptr;
  ptr = NULL;
  ptr = realloc(ptr, 1);
  exit(ptr == NULL);
}],
  ac_cv_c_realloc_bug="no",
  ac_cv_c_realloc_bug="yes",
  ac_cv_c_realloc_bug="yes") )
if test "$ac_cv_c_realloc_bug" = "yes"
  then
    :
    $1
  else
    :
    $2
fi
])


dnl ICE_CHECK_DECL (FUNCTION, HEADER-FILE...)
dnl -----------------------------------------
dnl
dnl If FUNCTION is available, define `HAVE_FUNCTION'.  If it is declared
dnl in one of the headers named in the whitespace-separated list
dnl HEADER_FILE, define `HAVE_FUNCTION_DECL` (in all capitals).
dnl
AC_DEFUN(ICE_CHECK_DECL,
[
changequote(,)dnl
ice_tr=`echo $1 | tr '[a-z]' '[A-Z]'`
changequote([,])dnl
ice_have_tr=HAVE_$ice_tr
ice_have_decl_tr=${ice_have_tr}_DECL
ice_have_$1=no
AC_CHECK_FUNCS($1, ice_have_$1=yes)
if test "${ice_have_$1}" = yes; then
AC_MSG_CHECKING(for $1 declaration in $2)
AC_CACHE_VAL(ice_cv_have_$1_decl,
[
ice_cv_have_$1_decl=no
changequote(,)dnl
ice_re_params='[a-zA-Z_][a-zA-Z0-9_]*'
ice_re_word='(^|[^a-zA-Z_0-9_])'
changequote([,])dnl
for header in $2; do
# Check for ordinary declaration
AC_EGREP_HEADER([${ice_re_word}$1 *\(], $header,
	ice_cv_have_$1_decl=yes)
if test "$ice_cv_have_$1_decl" = yes; then
	break
fi
# Check for "fixed" declaration like "getpid _PARAMS((int))"
AC_EGREP_HEADER([${ice_re_word}$1 *$ice_re_params\(\(], $header,
	ice_cv_have_$1_decl=yes)
if test "$ice_cv_have_$1_decl" = yes; then
	break
fi
done
])
AC_MSG_RESULT($ice_cv_have_$1_decl)
if test "$ice_cv_have_$1_decl" = yes; then
AC_DEFINE_UNQUOTED(${ice_have_decl_tr})
fi
fi
])dnl


dnl ACX_CHECK_MOTIF
dnl --------------
AC_DEFUN(ACX_CHECK_MOTIF,
[
  AC_REQUIRE([AC_PATH_XTRA])
  AC_ARG_WITH(motif_library,
  [  --with-motif-library=OBJ     use OBJ as Motif library [[-lXm]]],
  motif_library="$withval")
  if test "x$motif_library" = "x"
  then
    motif_library=-lXm
  fi

  ACX_SAVE_STATE
  
  AC_CACHE_CHECK([for a Motif >= $1 compatible API], acx_cv_motif,
    AC_CACHE_VAL(acx_cv_motif_library, acx_cv_motif_library=$motif_library)
    LIBS="$acx_cv_motif_library $GUI_LIBS"
    CFLAGS="$X_CFLAGS $CFLAGS"
    CPPFLAGS="$X_CFLAGS $CPPFLAGS"
    LDFLAGS="$X_LIBS $LDFLAGS"
    AC_TRY_RUN([
#include <Xm/XmAll.h>
      int main(void) {
        int vlibn, vincn;
        vincn = XmVersion;
        XmRegisterConverters();
        vlibn = xmUseVersion;
        if (vincn < [$1]) {
          exit(1);
        }
        if (vincn != vlibn) {
          exit(2);
        }
        exit(0);
      }
      ],

      acx_cv_motif="yes",
      acx_cv_motif="no",
      acx_cv_motif="no"
    )
  )
  if test "$acx_cv_motif" = "yes"
  then
    AC_DEFINE(HAVE_MOTIF)
    MOTIF_LIB="$acx_cv_motif_library"
    $2
    dnl **** Check whether Motif is actually Lesstif
    ICE_CHECK_LESSTIF
    dnl **** Check whether _XmVersionString[] can be referred to
    ACX_CHECK_XMVERSIONSTRING
  else
    MOTIF_LIB=
    $3
  fi
  
  ACX_RESTORE_STATE
])dnl


dnl ICE_CHECK_LESSTIF
dnl -----------------
dnl
dnl Define `HAVE_LESSTIF' if the Motif library is actually a LessTif library
dnl
AC_DEFUN(ICE_CHECK_LESSTIF,
[
AC_MSG_CHECKING(whether the Motif library is actually a LessTif library)
AC_CACHE_VAL(ice_cv_have_lesstif,
AC_EGREP_CPP(yes,
[#include <Xm/Xm.h>
#ifdef LesstifVersion
yes
#endif
], ice_cv_have_lesstif=yes, ice_cv_have_lesstif=no))
AC_MSG_RESULT($ice_cv_have_lesstif)
if test "$ice_cv_have_lesstif" = yes; then
AC_DEFINE(HAVE_LESSTIF)
fi
])dnl


dnl ACX_CHECK_XMVERSIONSTRING
dnl --------------
AC_DEFUN(ACX_CHECK_XMVERSIONSTRING,
[
  AC_CACHE_CHECK([whether _XmVersionString[] can be referred to],
    acx_cv__xmversionstring,
    AC_TRY_LINK([#include <stdio.h>],
                [extern char _XmVersionString[[]]; printf("%s\n", _XmVersionString);],
                [acx_cv__xmversionstring="yes"],
                [acx_cv__xmversionstring="no"]
    )
  )
  if test "$acx_cv__xmversionstring" = "yes"
  then
    AC_DEFINE(HAVE__XMVERSIONSTRING)
    $1
  else
    :
    $2
  fi
])dnl


dnl ACX_CHECK_T1LIB
dnl --------------
AC_DEFUN(ACX_CHECK_T1LIB,
[
  AC_CACHE_CHECK([for T1lib >= $1], acx_cv_t1lib,
    ACX_SAVE_STATE
    LIBS="-lt1 -lm $LIBS"
    AC_TRY_RUN([
#include <string.h>
#include <t1lib.h>
      int main(void) {
        char *vlib;
        vlib = T1_GetLibIdent();
        if (strcmp(vlib, "[$1]") < 0) {
          exit(1);
        }
        exit(0);
      }
      ],

      acx_cv_t1lib="yes",
      acx_cv_t1lib="no",
      acx_cv_t1lib="no"
    )
    ACX_RESTORE_STATE
  )
  if test "$acx_cv_t1lib" = "yes"
  then
    T1_LIB="-lt1"
    $2
  else
    T1_LIB=
    $3
  fi
])dnl

dnl ACX_CHECK_ZLIB
dnl --------------
AC_DEFUN(ACX_CHECK_ZLIB,
[
  AC_ARG_WITH(zlib_library,
  [  --with-zlib-library=OBJ      use OBJ as ZLIB library [[-lz]]],
  zlib_library="$withval")
  if test "x$zlib_library" = "x"
  then
    zlib_library=-lz
  fi

  AC_CACHE_CHECK([for zlib >= $1], acx_cv_zlib,
    AC_CACHE_VAL(acx_cv_zlib_library, acx_cv_zlib_library=$zlib_library)
    ACX_SAVE_STATE
    LIBS="$acx_cv_zlib_library $LIBS"
    AC_TRY_RUN([
#include <string.h>
#include <zlib.h>
      int main(void) {
        char *vlib, *vinc;
        vlib = zlibVersion();
        vinc = ZLIB_VERSION;
        if (strcmp(vinc, "[$1]") < 0) {
          exit(1);
        }
        if (strcmp(vinc, vlib) != 0) {
          exit(2);
        }
        exit(0);
      }
      ],

      acx_cv_zlib="yes",
      acx_cv_zlib="no",
      acx_cv_zlib="no"
    )
    ACX_RESTORE_STATE
  )
  if test "$acx_cv_zlib" = "yes"
  then
    Z_LIB="$acx_cv_zlib_library"
    $2
  else
    Z_LIB=
    $3
  fi
])dnl

dnl ACX_CHECK_JPEG
dnl --------------
AC_DEFUN(ACX_CHECK_JPEG,
[
  AC_ARG_WITH(jpeg_library,
  [  --with-jpeg-library=OBJ      use OBJ as JPEG library [[-ljpeg]]],
  jpeg_library="$withval")
  if test "x$jpeg_library" = "x"
  then
    jpeg_library=-ljpeg
  fi
  
  AC_CACHE_CHECK([for IJG JPEG software >= $1], acx_cv_jpeg,
    AC_CACHE_VAL(acx_cv_jpeg_library, acx_cv_jpeg_library=$jpeg_library)
    ACX_SAVE_STATE
    LIBS="$acx_cv_jpeg_library $LIBS"
    AC_TRY_RUN([
#include <stdio.h>
#include <jpeglib.h>
      int main(void) {
        int vinc;
        struct jpeg_compress_struct cinfo;
        jpeg_create_compress(&cinfo);
        vinc = JPEG_LIB_VERSION;
        if (vinc < [$1]) {
          exit(1);
        }
        exit(0);
      }
      ],

      acx_cv_jpeg="yes",
      acx_cv_jpeg="no",
      acx_cv_jpeg="no"
    )
    ACX_RESTORE_STATE
  )
  if test "$acx_cv_jpeg" = "yes"
  then
    JPEG_LIB=$acx_cv_jpeg_library
    $2
  else
    JPEG_LIB=
    $3
  fi
])dnl

dnl ACX_CHECK_PNG
dnl --------------
AC_DEFUN(ACX_CHECK_PNG,
[
  AC_ARG_WITH(png_library,
  [  --with-png-library=OBJ       use OBJ as PNG library [[-lpng]]],
  png_library="$withval")
  if test "x$png_library" = "x"
  then
    png_library=-lpng
  fi

  AC_CACHE_CHECK([for libpng >= $1], acx_cv_png,
    AC_CACHE_VAL(acx_cv_png_library, acx_cv_png_library=$png_library)
    ACX_SAVE_STATE
    LIBS="$acx_cv_png_library $Z_LIB $LIBS"
    AC_TRY_RUN([
#include <string.h>
#include <png.h>
      int main(void) {
        char *vlib, *vinc;
        vlib = png_libpng_ver;
        vinc = PNG_LIBPNG_VER_STRING;
        if (strcmp(vinc, "[$1]") < 0) {
          exit(1);
        }
        if (strcmp(vinc, vlib) != 0) {
          exit(2);
        }
        exit(0);
      }
      ],

      acx_cv_png="yes",
      acx_cv_png="no",
      acx_cv_png="no"
    )
    ACX_RESTORE_STATE
  )
  if test "$acx_cv_png" = "yes"
  then
    PNG_LIB="$acx_cv_png_library"
    $2
  else
    PNG_LIB=
    $3
  fi
])dnl

dnl ACX_CHECK_PDFLIB
dnl --------------
AC_DEFUN(ACX_CHECK_PDFLIB,
[
  AC_ARG_WITH(pdf_library,
  [  --with-pdf-library=OBJ       use OBJ as PDFlib library [[-lpdf]]],
  pdf_library="$withval")
  if test "x$pdf_library" = "x"
  then
    pdf_library=-lpdf
  fi

  AC_CACHE_CHECK([for PDFlib >= $1], acx_cv_pdflib,
    AC_CACHE_VAL(acx_cv_pdf_library, acx_cv_pdf_library=$pdf_library)
    ACX_SAVE_STATE
    LIBS="$acx_cv_pdf_library $JPEG_LIB $PNG_LIB $Z_LIB $LIBS"
    AC_TRY_RUN([
#include <pdflib.h>
      int main(void) {
        char *vinc;
        int vlibn, vincn;
        vlibn = 100*PDF_get_majorversion() + PDF_get_minorversion();
        vincn = 100*PDFLIB_MAJORVERSION + PDFLIB_MINORVERSION;
        vinc = PDFLIB_VERSIONSTRING;
        if (strcmp(vinc, "[$1]") < 0) {
          exit(1);
        }
        if (vincn != vlibn) {
          exit(2);
        }
        exit(0);
      }
      ],

      acx_cv_pdflib="yes",
      acx_cv_pdflib="no",
      acx_cv_pdflib="no"
    )
    ACX_RESTORE_STATE
  )
  if test "$acx_cv_pdflib" = "yes"
  then
    PDF_LIB="$acx_cv_pdf_library"
    $2
  else
    PDF_LIB=
    $3
  fi
])dnl

dnl ACX_CHECK_NETCDF
dnl --------------
AC_DEFUN(ACX_CHECK_NETCDF,
[
  AC_ARG_WITH(netcdf_libraries,
  [  --with-netcdf-libraries=OBJ  use OBJ as netCDF libraries [[-lnetcdf]]],
  netcdf_libraries="$withval")
  if test "x$netcdf_libraries" = "x"
  then
    netcdf_libraries=-lnetcdf
  fi

  AC_CACHE_CHECK([for netCDF API version >= $1], acx_cv_netcdf,
    AC_CACHE_VAL(acx_cv_netcdf_libraries, acx_cv_netcdf_libraries=$netcdf_libraries)
    ACX_SAVE_STATE
    LIBS="$acx_cv_netcdf_libraries $LIBS"


    AC_TRY_RUN([
#include <stdio.h>
#include <netcdf.h>
      int main(void) {
        char *vlib;
        vlib = nc_inq_libvers();
        if (strcmp(vlib, "[$1]") < 0) {
          exit(1);
        }
        exit(0);
      }
      ],

      acx_cv_netcdf="yes",
      acx_cv_netcdf="no",
      acx_cv_netcdf="no"
    )
    ACX_RESTORE_STATE
  )
  if test "$acx_cv_netcdf" = "yes"
  then
    NETCDF_LIBS="$acx_cv_netcdf_libraries"
    $2
  else
    NETCDF_LIBS=
    $3
  fi
])dnl

dnl ACX_CHECK_FFTW
dnl --------------
AC_DEFUN(ACX_CHECK_FFTW,
[
  AC_ARG_WITH(fftw_library,
  [  --with-fftw-library=OBJ      use OBJ as FFTW library [[-lfftw]]],
  fftw_library="$withval")
  if test "x$fftw_library" = "x"
  then
    fftw_library=-lfftw
  fi

  AC_CACHE_CHECK([for FFTW library >= $1], acx_cv_fftw,
    AC_CACHE_VAL(acx_cv_fftw_library, acx_cv_fftw_library=$fftw_library)
    ACX_SAVE_STATE
    LIBS="$acx_cv_fftw_library $LIBS"
    AC_TRY_RUN([
#include <fftw.h>
#include <string.h>
      int main(void) {
        char *vlib = (char *) fftw_version;
        if (strcmp(vlib, "[$1]") < 0) {
          exit(1);
        }
        exit(0);
      }
      ],

      acx_cv_fftw="yes",
      acx_cv_fftw="no",
      acx_cv_fftw="no"
    )

    ACX_RESTORE_STATE
  )
  if test "$acx_cv_fftw" = "yes"
  then
    FFTW_LIB="$acx_cv_fftw_library"
    $2
  else
    FFTW_LIB=
    $3
  fi
])dnl


dnl ACX_CHECK_XMHTML
dnl --------------
AC_DEFUN(ACX_CHECK_XMHTML,
[
  AC_ARG_WITH(xmhtml_library,
  [  --with-xmhtml-library=OBJ    use OBJ as XmHTML library [[-lXmHTML]]],
  xmhtml_library="$withval")
  if test "x$xmhtml_library" = "x"
  then
    xmhtml_library=-lXmHTML
  fi

  AC_CACHE_CHECK([for XmHTML widget >= $1], acx_cv_xmhtml,
    AC_CACHE_VAL(acx_cv_xmhtml_library, acx_cv_xmhtml_library=$xmhtml_library)
    ACX_SAVE_STATE
    LIBS="$acx_cv_xmhtml_library $JPEG_LIB $PNG_LIB $Z_LIB $LIBS"
    AC_TRY_RUN([
#include <XmHTML/XmHTML.h>
      int main(void) {
        int vlib, vinc;
        vlib = XmHTMLGetVersion();
        vinc = XmHTMLVersion;
        if (vinc < [$1]) {
          exit(1);
        }
        if (vinc != vlib) {
          exit(2);
        }
        exit(0);
      }
      ],

      acx_cv_xmhtml="yes",
      acx_cv_xmhtml="no",
      acx_cv_xmhtml="no"
    )
    ACX_RESTORE_STATE
  )
  if test "$acx_cv_xmhtml" = "yes"
  then
    XMHTML_LIB="$acx_cv_xmhtml_library"
    $2
  else
    XMHTML_LIB=
    $3
  fi
])dnl

