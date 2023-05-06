extends OSCSender

@export var cube : Node

func _ready():
	setup( "127.0.0.1", 12000 )		# will send messages to ip:port
	start()							# enabling emission

func _process(delta):
	cube.rotate_y( delta * 1 )
	msg("/gdosc")					# creation of new message internally
	add( cube )						# adding an object, will send its name
	add( cube.rotation )			# adding a vector3, will be transform into 3 floats
	add( cube.translation )
	send()							# sending the message
	pass
	
func _exit_tree ( ):
	stop()							# disable the sender, highly recommended!
