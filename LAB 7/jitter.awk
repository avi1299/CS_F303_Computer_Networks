BEGIN {
 # simple awk script to generate jitter statistics
 # in a form suitable for plotting with xgraph.
 highest_packet_id = 0;
}
# # jitter ＝((recvtime(j)-sendtime(j))-(recvtime(i)-sendtime(i)))/(j-i), j > i
{
 action = $1;
 time = $2;
 node_1 = $3;
 node_2 = $4;
 src = $5;
 flow_id = $8;
 node_1_address = $9;
 node_2_address = $10;
 seq_no = $11;
 packet_id = $12;
 if ( packet_id > highest_packet_id ) highest_packet_id = packet_id;
 # getting start time is not a problem, provided you're not starting
 # traffic at 0.0.
 # could test for sending node_1_address or flow_id here.
 if ( start_time[packet_id] == 0 ){
     pkt_seqno[packet_id] = seq_no;
     start_time[packet_id] = time;
 } 

 # only useful for small unicast where packet_id doesn't wrap.
 # checking receive means avoiding recording drops
 if (flow_id == 1){
 if ( action != "d" ) {
 if ( action == "r" ) {
 # could test for receiving node_2_address or flow_id here.
 end_time[packet_id] = time;
 }
 } else {
 end_time[packet_id] = -1;
 }
 }
}


END {
last_seqno = 0;
last_delay = 0;
seqno_diff = 0;
for ( packet_id = 0; packet_id <= highest_packet_id; packet_id++ ) {
start = start_time[packet_id];
end = end_time[packet_id];
packet_duration = end - start;
if ( start < end ) {
seqno_diff = pkt_seqno[packet_id] - last_seqno;
delay_diff = packet_duration - last_delay;
if (seqno_diff == 0) {
jitter =0;
} else {
jitter = delay_diff/seqno_diff;
}
printf("%f %f\n", start, jitter);
last_seqno = pkt_seqno[packet_id];
last_delay = packet_duration;
}
}
}






# BEGIN {

# # Initialization

# highest_packet_id = 0;

# }

# {

# action = $1;

# time = $2;

# from = $3;

# to = $4;

# type = $5;

# pktsize = $6;

# flow_id = $8;

# src = $9;

# dst = $10;

# seq_no = $11;

# packet_id = $12;


# if ( packet_id > highest_packet_id ) {

# highest_packet_id = packet_id;

# }


# #Record the transmission time

# if ( start_time[packet_id] == 0 ) {

# # Record the sequence number

# pkt_seqno[packet_id] = seq_no;

# start_time[packet_id] = time;

# }


# #Record the receiving time for CBR (flow_id=2)

# if ( flow_id == 2 && action != “d” ) {

# if ( action == “r” ) {

# end_time[packet_id] = time;

# }

# } else {

# end_time[packet_id] = -1;

# }

# }

# END {

# last_seqno = 0;

# last_delay = 0;

# seqno_diff = 0;


# for ( packet_id = 0; packet_id <= highest_packet_id; packet_id++ ) {

# start = start_time[packet_id];

# end = end_time[packet_id];

# packet_duration = end – start;


# if ( start < end ) {

# seqno_diff = pkt_seqno[packet_id] – last_seqno;

# delay_diff = packet_duration – last_delay;

# if (seqno_diff == 0) {

# jitter =0;

# } else {

# jitter = delay_diff/seqno_diff;

# }

# printf(“%f %f\n”, start, jitter);

# last_seqno = pkt_seqno[packet_id];

# last_delay = packet_duration;

# }

# }

# }