$ ! This command file reads DOTEST. and executes the GRACE commands.
$ !
$ XMGRACE = XMGRACE + " -noask"
$ !
$ ON CONTROL_Y THEN GOTO DONE
$ ON ERROR THEN GOTO DONE
$ OPEN IN DOTEST.
$LOOP:
$ READ/END=DONE IN REC
$ IF (F$EXTRACT (0, 6, REC) .EQS. "$GRACE")
$ THEN
$   REDIR = F$ELEMENT (1, "<", REC)
$   IF (REDIR .NES. "<")
$   THEN
$     WRITE SYS$OUTPUT "$ DEFINE/USER SYS$INPUT ''REDIR'"
$     DEFINE/USER SYS$INPUT 'REDIR'
$     REC = F$ELEMENT (0, "<", REC)
$   ENDIF
$   REC = "XM" + (REC - "$")
$   WRITE SYS$OUTPUT "$ ", REC
$   'REC'
$ ENDIF
$ GOTO LOOP
$DONE:
$ CLOSE IN
$ EXIT
