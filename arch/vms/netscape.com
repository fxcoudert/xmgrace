$ lock = f$trnlnm ("sys$login") - "]" + ".NETSCAPE]LOCK.VMS"
$ if (f$search(lock) .eqs. "") then goto not_running
$ open/read/error=running lock 'lock'
$ close lock
$not_running:
$ netscape 'p1'
$ exit
$running:
$ netscape -noraise -remote "openURL(''p1',newwindow)"
$ exit
