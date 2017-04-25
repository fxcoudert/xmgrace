       PROGRAM TESTF_NP
C
       IMPLICIT NONE
       INTEGER i
       CHARACTER*64 buf
C
       INTEGER GraceOpenf, GraceIsOpenf
       EXTERNAL GraceOpenf, GraceIsOpenf
       EXTERNAL GraceRegistErerrorFunctionf
       EXTERNAL GraceCommandf, GraceClosef
       EXTERNAL MyError
C
       CALL GraceRegistErerrorFunctionf (MyError)
C
C      Start Grace with a buffer size of 2048 and open the pipe
C
       IF (GraceOpenf(2048) .EQ. -1) THEN
           WRITE (*,*) 'Can not run grace.'
           CALL EXIT (1)
       ENDIF
C
C      Send some initialization commands to Grace
C
       CALL GraceCommandf ('world xmax 100')
       CALL GraceCommandf ('world ymax 10000')
       CALL GraceCommandf ('xaxis tick major 20')
       CALL GraceCommandf ('xaxis tick minor 10')
       CALL GraceCommandf ('yaxis tick major 2000')
       CALL GraceCommandf ('yaxis tick minor 1000')
       CALL GraceCommandf ('s0 on')
       CALL GraceCommandf ('s0 symbol 1')
       CALL GraceCommandf ('s0 symbol size 0.3')
       CALL GraceCommandf ('s0 symbol fill pattern 1')
       CALL GraceCommandf ('s1 on')
       CALL GraceCommandf ('s1 symbol 1')
       CALL GraceCommandf ('s1 symbol size 0.3')
       CALL GraceCommandf ('s1 symbol fill pattern 1')
C
C      Display sample data
C
       DO i = 1, 100, 1
           IF (GraceIsOpenf () .NE. 0) THEN
               WRITE (buf, 1) i, i
               CALL GraceCommandf (buf)
               WRITE (buf, 2) i, i**2
               CALL GraceCommandf (buf)
C
C              Update the Grace display after every ten steps
C
               IF (10*(i / 10) .EQ. i) THEN
                   CALL GraceCommandf ('redraw')
C                  Wait a second, just to simulate some time needed
C                  for calculations. Your real application shouldn't wait
                   CALL SLEEP (1)
               ENDIF
           ENDIF
       ENDDO
C
       IF (GraceIsOpenf () .NE. 0) THEN
C
C          Tell Grace to save the data
C
           CALL GraceCommandf ('saveall "sample.agr"')
C
C          Flush the output buffer and close Grace
C
           CALL GraceClosef ()
C
C          We are done
C
           CALL EXIT (0)
       ELSE
           CALL EXIT (1)
       ENDIF
C      
 1     FORMAT ('g0.s0 point ', I6, ' , ', I6)
 2     FORMAT ('g0.s1 point ', I6, ' , ', I6)
C
       END
C
       SUBROUTINE MyError (str)
C
       IMPLICIT NONE
       CHARACTER*(*) str
C
       WRITE (0, '(''library message : "'', a, ''"'')') str
C
       RETURN
       END
