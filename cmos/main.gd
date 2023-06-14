extends Node2D

@onready
var circuit = get_node("SubViewportContainer/SubViewport/Circuit")
@onready
var label_edit = get_node("LabelEdit")

@onready
var cable_scene = preload("res://cable.tscn");
@onready
var gnd_scene = preload("res://parts/gnd.tscn");
@onready
var nmos_scene = preload("res://parts/nmos.tscn");
@onready
var pmos_scene = preload("res://parts/pmos.tscn");
@onready
var label_scene = preload("res://parts/label.tscn");

func set_move():
	circuit.tool = 0
func set_delete():
	circuit.tool = 1
func set_rotate():
	circuit.tool = 4
	
func set_nmos():	
	circuit.tool = 2
	circuit.next_part = nmos_scene.instantiate()
func set_pmos():
	circuit.tool = 2
	circuit.next_part = pmos_scene.instantiate()
func set_gnd():
	circuit.tool = 2
	circuit.next_part = gnd_scene.instantiate()
func set_label():
	circuit.tool = 2
	circuit.next_part = label_scene.instantiate()
	circuit.next_part.get_node("Label").text = label_edit.text

func import_json_btn():
	circuit.load_json(get_node("JSON").text)
func export_json_btn():
	get_node("JSON").text = circuit.to_json()
	
func trace_circuit_btn():
	circuit.trace_circuit()

func _ready():
	circuit.set_scenes(cable_scene, nmos_scene, pmos_scene, gnd_scene, label_scene)
	
	get_node("MoveButton").connect("pressed", set_move)
	get_node("DeleteButton").connect("pressed", set_delete)
	get_node("RotateButton").connect("pressed", set_rotate)
	get_node("NMOSButton").connect("pressed", set_nmos)
	get_node("PMOSButton").connect("pressed", set_pmos)
	get_node("GNDButton").connect("pressed", set_gnd)
	get_node("LabelButton").connect("pressed", set_label)
	get_node("ImportButton").connect("pressed", import_json_btn)
	get_node("ExportButton").connect("pressed", export_json_btn)
	get_node("TraceButton").connect("pressed", trace_circuit_btn)
