extends OSCReceiver

@export var cube : MeshInstance3D
@export var env  : WorldEnvironment

var sky

func _ready():
	pass
	sky = env.get_environment().background_color
	max_queue( 20 ) 			# maximum number of messages in the buffer, default is 100
	avoid_duplicate( true )	# receiver will only keeps the "latest" message for each address
	setup( 14000 )			# listening to port 14000
	start()					# starting the reception of messages

func _process(delta):
	pass
	while( has_message() ): 	# check if there are pending messages
		var msg = get_next()	# retrieval of the messages as a dictionary
		# using message data
		var args = msg["args"]
		if msg["address"] == "/cube/pos" and cube != null:
			cube.translation = Vector3( args[0], args[1], args[2] )
		elif msg["address"] == "/cube/col" and cube != null:
			cube.get_surface_material(0).albedo_color = Color( args[0], args[1], args[2] )
		elif msg["address"] == "/cube/visible" and cube != null:
			cube.visible = args[0]
		elif msg["address"] == "/sky/horizon" and sky != null:
			var c = Color( args[0], args[1], args[2] )
			sky = c
		else:
			print( msg["address"] + "?" )
	pass
	
func _exit_tree ( ):
	pass
	stop()					# stops listening to port, highly recommended!
