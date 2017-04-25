/* dotest.cmd, the REXX-based equivalent to shell-script 'dotest' */
/* demos for GRACE      */
/* define the location  */

curdir = Directory()
curdir = Strip(curdir, 'T', '\') /* handle worst case: root */

/* find binary */
guess = Stream('..\src\xmgrace.exe', 'C', 'QUERY EXISTS')
if guess = '' then
  do
  x11root = Value('X11ROOT', , 'OS2ENVIRONMENT')
  guess = Stream(x11root'/XFree86/bin/xmgrace.exe', 'C', 'QUERY EXISTS')
  if guess <> '' then 
    do
    GRACE = guess
    end
  else
     do
     say 'No binary found'
     exit
     end
  end

/* find examples */
gracehome = Value('GRACE_HOME', , 'OS2ENVIRONMENT')
guess = Stream(gracehome'/examples/explain.agr', 'C', 'QUERY EXISTS')
if guess = '' then 
  do
  say 'No examples found'
  exit
  end
else
  do
  exampledir = gracehome'/examples'
  end

/* switch to example dir */
newdir=directory(exampledir)

/* command line parameters */
call ExecuteCmd GRACE' -usage'

/* call ExecuteCmd sleep 3 */

/* don't ask stupid questions */
GRACE = "xmgrace -noask"

/* explain the row of single character buttons and a few other things */
GRACE" explain.agr"

/* display the various axes available */
GRACE" axes.agr"

/* display the symbols and line styles */
GRACE" symslines.agr"

/* display various fill styles */
GRACE" fills.agr"

/* some graph stuff and ticks */
GRACE" -p graphs.par"

/* some graph stuff and ticks */
GRACE" props.agr"

/* demonstration of many graphs */
GRACE" manygraphs.agr"

/* some graph stuff and ticks */
GRACE" brw.dat -p regions.par"

/* test of a graph inset */
GRACE" tinset.agr"

/* some time and date formats */
GRACE" times.agr"

/* some more tick label formats */
GRACE" -p tforms.par"

/* Australia map */
GRACE" au.agr"

/* log plots */
GRACE" -autoscale none -p logtest.par log.dat -graph 1 log.dat"

/* more log plots */
GRACE" tlog.agr"

/* a log2 example */
GRACE" log2.agr"

/* a logit scale sample */
GRACE" logit.agr"

/* display fonts and font mappings */
GRACE" tfonts.agr"

/* text transforms */
GRACE" txttrans.agr"

/* advaned typesetting */
GRACE" typeset.agr"

/* example of world stack */
GRACE" tstack.agr"

/* a graph with a parameter file */
GRACE" -p test1.par -autoscale xy test.dat"

/* a graph with a parameter file in reverse video */
GRACE" -rvideo -p test1.par -autoscale xy test.dat"

GRACE" test2.agr"

/* explanation of arrow shape parameters */
GRACE" arrows.agr"

/* multiple graphs with a parameter file */
GRACE" mlo.dat -graph 1 brw.dat -p co2.par"

/* multiple graphs created with arrange feature */
GRACE" co2.agr"

/* a nice sample */
GRACE" spectrum.agr"

/* a graph with alternate axes */
GRACE" -p altaxis.par test.dat -autoscale xy"

/* a graph with error bars */
GRACE" terr.agr"

/* a fixed graph with XY RADIUS format */
GRACE" txyr.agr"

/* string annotations */
GRACE" motif.agr"

/* a graph with an XYZ set */
GRACE" xyz.agr"

/* a graph with HILO data */
GRACE" hilo.agr"

/* a graph with BOXPLOT data */
GRACE" boxplot.agr"

/* polar plots */
GRACE" polar.agr"

/* bar charts */

/* a bar graph demonstrating specified ticks and tick labels */
GRACE" bar.agr"

/* a stacked bar chart */
GRACE" stackedb.agr"

/* a bar chart with error bars */
GRACE" chartebar.agr"

/* display all types of XY charts */
GRACE" charts.agr"

/* pie charts */
GRACE" pie.agr"

/* vector map */
GRACE" vmap.agr"

/* a bubble plot */
GRACE" xysize.agr"

/* non-linear curve fitting */
GRACE" logistic.agr"

/* some interesting stuff */
GRACE" -b test.com"

/* need a program */
/* modified from previous versions, a thank you goes to Bruce Barnett */
/* this modification allows others without write permission */
/* to run the demos. */

say
rs = Stream('tmc.exe', 'C', 'QUERY EXISTS')
if rs = '' then
  do
  say "Compiling a short program to test the -pipe option"
  say "Executing 'gcc tmc.c -o tmc.exe'"
  '@gcc tmc.c -o tmc.exe'
  say "Done compilation"
  say
  end

/* a graph with the -pipe option */
say "Testing -pipe option, executing './tmc | $GRACE -pipe' "
'.\tmc.exe | 'GRACE' -pipe'
call ExecuteCmd 'rm -f tmc.exe'

/* switch back */
newdir=directory(curdir)

exit /* end of main program */


/* Procedures */

ExecuteCmd: PROCEDURE
Parse Arg EC_param
_silent_pre = "@"
ADDRESS CMD _silent_pre""EC_param
return rc
