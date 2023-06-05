extends Part

func _ready():
	add_cable(get_node("Source"))
	add_cable(get_node("Drain"))
	add_cable(get_node("Gate"))
	add_cable(get_node("Bulk"))
