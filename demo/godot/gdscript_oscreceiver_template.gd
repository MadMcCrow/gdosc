extends OSCReceiver

var oscrcv

func _ready():
		# [optional] maximum number of messages in the buffer, default is 100
	max_queue( 20 )
	# [optional]  receiver will only keeps the "latest" message for each address
	avoid_duplicate( true )
	# [mandatory] listening to port 14000
	setup( 14000 )
	# [mandatory] starting the reception of messages
	start()

func _process(delta):
	
	# check if there are pending messages
	while( has_message() ):
		# retrieval of the messages as a dictionary
		var msg = get_next()
		# printing the values, check console
		print( "address:", msg["address"] )
		print( "typetag:", msg["typetag"] )
		print( "from:" + str( msg["ip"] ) + ":" + str( msg["port"] ) )
		print( "arguments: ")
		for i in range( 0, msg["arg_num"] ):
			print( "\t", i, " = ", msg["args"][i] )
	pass
	
func _exit_tree ( ):
	# disable the receiver, highly recommended!
	stop()
