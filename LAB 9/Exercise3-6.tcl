#Create a simulator object
set ns [new Simulator]

$ns color 1 Red
# $ns color 2 Blue
# $ns color 3 chocolate

#Open the output files
set f0 [open tp0.tr w]
set f1 [open tp1.tr w]
set f6 [open tp6.tr w]
set f2 [open tp2.tr w]
set f3 [open tp3.tr w]
set f7 [open tp_total.tr w]
set f4 [open tp4.tr w]
set f5 [open tp5 w]
set nf [open out.nam w]
$ns namtrace-all $nf
set tf [open outall.tr w]
$ns trace-all $tf



#Create 5 nodes
# set n0 [$ns node]
# set n1 [$ns node]
# set n2 [$ns node]
# set n3 [$ns node]

#Connect the nodes
# $ns duplex-link $n0 $n1 3Mb 100ms DropTail
# $ns duplex-link $n3 $n1 3Mb 100ms DropTail
# $ns duplex-link $n1 $n2 1Mb 100ms RED

#$ns queue-limit $n1 $n2 30

#set qmon [$ns monitor-queue $n1 $n2  [open qtrace.tr w] 0.03]
#[$ns link $n1 $n2] queue-sample-timeout
#$ns duplex-link-op $n1 $n2 queuePos 0.5

for {set i 0} {$i < 7} {incr i} {
set n($i) [$ns node]
}
#Create links between the nodes
for {set i 0} {$i < 7} {incr i} {
$ns duplex-link $n($i) $n([expr ($i+1)%7]) 1Mb 10ms DropTail
}

$ns cost $n(1) $n(2) 1000
$ns cost $n(2) $n(1) 1000
#Run DV agents on nodes
$ns rtproto DV 

proc rtdump {} {
    global ns
    set now [$ns now]
    puts "Routing table at time $now"
    # Use any one of the following
    $ns dump-routelogic-nh
    #Table in terms of next hops
    $ns dump-routelogic-distance
    #Table in terms of distance
}



#Define a 'finish' procedure
proc finish {} {
	global ns f0 f1 f2 f3 f3 f4 f5 f6 f7 nf tf
	#Close the trace and output files
		close $f0
		close $f1
		close $f2
        close $f3
        close $f4
        close $f5
        close $f6
        close $f7
        close $tf
        close $nf
	#Call xgraph to display the results
	#exec xgraph tp1.tr -color 2 tp2.tr -color 3 tp_total.tr -color 4 cwnd1.tr -color 5 cwnd2.tr -color 6 q_size.tr -color 7 &
        exec xgraph tp0.tr tp1.tr tp2.tr tp3.tr tp4.tr tp5.tr tp6.tr -geometry 800x400  &
	#Execute nam on the trace file
        exec nam out.nam &
        exit 0
}



#Define a procedure which periodically records the bandwidth received by the
#three traffic sinks sink0/1/2 and writes it to the three files f0/1/2.
proc record {} {
        global udp0 sink0 f0 f1 f2 f3 f4 f5 f6 f7
	#Get an instance of the simulator
	set ns [Simulator instance]
	#Set the time after which the procedure should be called again
        set time .1
	#How many bytes have been received by the traffic sinks?
        set bw0 [$sink0 set bytes_]
        # set cw0 [$tcp0 set cwnd_]
        # set bw1 [$sink1 set bytes_]
        # set bw2 [$sink2 set bytes_]
        # set cw1 [$tcp1 set cwnd_]
        # set cw2 [$tcp2 set cwnd_]
        # set qsi [$qmon set pkts_]
        #set bw2 [$sink2 set bytes_]
	#Get the current time
        set now [$ns now]
	#Calculate the bandwidth (in MBit/s) and write it to the files
        puts $f0 "$now [expr $bw0/$time*8/1000000]"
        # puts $f1 "$now [expr $bw1/$time*8/1000000]"
        # puts $f6 "$now [expr $bw2/$time*8/1000000]"
        # puts $f2 "$now [expr ($bw0+$bw1+$bw2)/$time*8/1000000]"
        # puts $f3 "$now [expr $cw0/100]"
        # puts $f4 "$now [expr $cw1/100]"
        # puts $f7 "$now [expr $cw2/100]"
        # puts $f5 "$now [expr $qsi]"
        #puts $f2 "$now [expr $bw2/$time*8/1000000]"
	#Reset the bytes_ values on the traffic sinks
        $sink0 set bytes_ 0
        # $sink1 set bytes_ 0
        # $sink2 set bytes_ 0
	#Re-schedule the procedure
        $ns at [expr $now+$time] "record"
}

set udp0 [new Agent/UDP]
$ns attach-agent $n(0) $udp0
set cbr0 [new Application/Traffic/CBR]
$cbr0 attach-agent $udp0


#Create three traffic sinks and attach them to the node n4
set sink0 [new Agent/LossMonitor]

$ns attach-agent $n(3) $sink0
$ns connect $udp0 $sink0

#Link down and up at 1s and 2s
$ns rtmodel-at 1.0 down $n(1) $n(2)
$ns rtmodel-at 2.0 up $n(1) $n(2)

#Set interval for remaking routing table
Agent/rtProto/DV set advertInterval 4


#Start logging the received bandwidth
$ns at 0.0 "record"
#Start the traffic sources
$ns at 0.0 "$cbr0 start"

$ns at 0.5 rtdump
$ns at 1.5 rtdump
$ns at 2.5 rtdump

#Stop the traffic sources
$ns at 5.0 "$cbr0 stop"
#Call the finish procedure after 60 seconds simulation time
$ns at 5.0 "finish"

#Run the simulation
$ns run
