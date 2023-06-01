extends Cable

func _ready():
	set_connector_origin(get_node("Origin"))
	set_connector_end(get_node("End"))
