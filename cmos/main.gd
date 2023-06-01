extends Node2D

@onready
var circuit = get_node("SubViewportContainer/SubViewport/Circuit")

func set_move():
	circuit.set_tool(0)
func set_delete():
	circuit.set_tool(1)
func set_nmos():
	circuit.set_tool(2)
func set_pmos():
	circuit.set_tool(3)

func _ready():
	get_node("MoveButton").connect("pressed", set_move)
	get_node("DeleteButton").connect("pressed", set_delete)
	get_node("NMOSButton").connect("pressed", set_nmos)
	get_node("PMOSButton").connect("pressed", set_pmos)
