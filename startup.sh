#! /bin/csh -f

set port = 8787
if ( "$1" != "" ) set port="$1"

cd ../area

nohup
nice
limit stack 1024k
unlimit core
if ( -e shutdown.txt ) rm -f shutdown.txt
set index = 1000
while ( 1 )
	set logfile = ../log/$index.log
	if ( ! -e $logfile ) break
	@ index++
    end

    date > $logfile
    date > ../area/boot.txt

# Run SWR.
# Check if already running
    set matches = `netstat -an | grep ":$port " | grep -c LISTEN`
    if ( $matches >= 1 ) then
        # Already running
        echo Port $port is already in use.
        exit 0
    endif
    ../bin/swr $port >&! $logfile

    if ( -e shutdown.txt ) then
	rm -f shutdown.txt
	exit 0
    endif
