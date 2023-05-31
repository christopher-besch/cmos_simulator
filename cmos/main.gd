extends Node2D

func set_move():
	get_node("SubViewportContainer/SubViewport/Circuit").set_tool(0)
func set_nmos():
	get_node("SubViewportContainer/SubViewport/Circuit").set_tool(1)
func set_pmos():
	get_node("SubViewportContainer/SubViewport/Circuit").set_tool(2)

func _ready():
	get_node("MoveButton").connect("pressed", set_move)
	get_node("NMOSButton").connect("pressed", set_nmos)
	get_node("PMOSButton").connect("pressed", set_pmos)
