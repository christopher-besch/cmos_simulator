extends Part

func _ready():
	for child in get_children():
		if child is Cable:
			add_cable(child)
