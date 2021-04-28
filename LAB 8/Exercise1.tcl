#Create a simulator object
set ns [new Simulator]

#Open the output files
set f0 [open out0.tr w]
set f1 [open out1.tr w]
#set f2 [open out2.tr w]
set nf [open out.nam w]
$ns namtrace-all $nf


#Create 5 nodes
set n0 [$ns node]
set n1 [$ns node]
set n2 [$ns node]

#Connect the nodes
$ns duplex-link $n0 $n1 2Mb 100ms DropTail
$ns duplex-link $n1 $n2 2Mb 100ms DropTail

#Define a 'finish' procedure
proc finish {} {
	global ns f0 f1 nf
	#Close the trace and output files
		close $f0
		close $f1
		#close $f2
        close $nf
	#Call xgraph to display the results
	exec xgraph out0.tr out1.tr -geometry 800x400 &
	#Execute nam on the trace file
        exec nam out.nam &
        exit 0
}



#Define a procedure which periodically records the bandwidth received by the
#three traffic sinks sink0/1/2 and writes it to the three files f0/1/2.
proc record {} {
        global tcp0 sink0 f0 f1
	#Get an instance of the simulator
	set ns [Simulator instance]
	#Set the time after which the procedure should be called again
        set time .1
	#How many bytes have been received by the traffic sinks?
        set bw0 [$sink0 set bytes_]
        set cw1 [$tcp0 set cwnd_]
        #set bw2 [$sink2 set bytes_]
	#Get the current time
        set now [$ns now]
	#Calculate the bandwidth (in MBit/s) and write it to the files
        puts $f0 "$now [expr $bw0/$time*8/1000000]"
        puts $f1 "$now [expr $cw1/$time*8/1000]"
        #puts $f2 "$now [expr $bw2/$time*8/1000000]"
	#Reset the bytes_ values on the traffic sinks
        $sink0 set bytes_ 0
        #$sink1 set bytes_ 0
        #$sink2 set bytes_ 0
	#Re-schedule the procedure
        $ns at [expr $now+$time] "record"
}

set tcp0 [new Agent/TCP]
$ns attach-agent $n0 $tcp0
set ftp0 [new Application/FTP]
$ftp0 attach-agent $tcp0
$tcp0 set type_ FTP


#Create three traffic sinks and attach them to the node n4
set sink0 [new Agent/TCPSink]
$ns attach-agent $n2 $sink0
$ns connect $tcp0 $sink0


#Start logging the received bandwidth
$ns at 0.0 "record"
#Start the traffic sources
$ns at 1.0 "$ftp0 start"
#Stop the traffic sources
$ns at 30.0 "$ftp0 stop"
#Call the finish procedure after 60 seconds simulation time
$ns at 30.0 "finish"

#Run the simulation
$ns run
