extends OSCSender


func _ready():
		# [mandatory] will send messages to ip:port
	setup( "127.0.0.1", 12000 )
	# [mandatory] enabling emission
	start()

func _process(delta):
	# creation of new message
	msg("/hello")
	# adding an object, will send its name
	add( self )
	# adding a float
	add( delta )
	# adding a Vector2
	add( Vector2( randf(), randf() ) )
	# adding a Vector3 (position, scale, rotation, etc)
	add( Vector3( randf(), randf(), randf() ) )
	# sending the message
	send()
	pass
	
func _exit_tree ( ):
	# disable the sender, highly recommended!
	stop()
