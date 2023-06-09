extends Node2D

@onready
var circuit = get_node("SubViewportContainer/SubViewport/Circuit")

func set_move():
	circuit.tool = 0
func set_delete():
	circuit.tool = 1
func set_nmos():
	circuit.tool = 2
	circuit.create_part_type = 1
func set_pmos():
	circuit.tool = 2
	circuit.create_part_type = 2
func set_rotate():
	circuit.tool = 4

func import_json_btn():
	circuit.load_json(get_node("JSON").text)
func export_json_btn():
	get_node("JSON").text = circuit.to_json()

func _ready():
	get_node("MoveButton").connect("pressed", set_move)
	get_node("DeleteButton").connect("pressed", set_delete)
	get_node("RotateButton").connect("pressed", set_rotate)
	get_node("NMOSButton").connect("pressed", set_nmos)
	get_node("PMOSButton").connect("pressed", set_pmos)
	get_node("ImportButton").connect("pressed", import_json_btn)
	get_node("ExportButton").connect("pressed", export_json_btn)
