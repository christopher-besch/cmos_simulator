extends Part

func _ready():
	add_connector(get_node("Source"))
	add_connector(get_node("Drain"))
	add_connector(get_node("Gate"))
	add_connector(get_node("Bulk"))
