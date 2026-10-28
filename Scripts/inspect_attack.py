import unreal
a = unreal.EditorAssetLibrary
key = unreal.Key()
result = key.import_text('LeftMouseButton')
unreal.log('CX_ATTACK key=%s imported=%s' % (key.export_text(), result))
action = a.load_asset('/Game/CXMultiplayer/Input/IA_Attack')
unreal.log('CX_ATTACK action=%s triggers=%s modifiers=%s' % (action, action.get_editor_property('triggers'), action.get_editor_property('modifiers')))
imc = a.load_asset('/Game/VehicleTemplate/Input/IMC_Default')
for mapping in imc.get_editor_property('mappings'):
    unreal.log('CX_ATTACK mapping=%s' % mapping.export_text())
for path in ['/Game/VehicleTemplate/Blueprints/SportsCar/BP_SportsCar_Pawn', '/Game/VehicleTemplate/Blueprints/BP_VehicleAdvPlayerController']:
    obj = unreal.get_default_object(a.load_blueprint_class(path))
    for prop in ['attack_action', 'input_mapping_context']:
        try:
            unreal.log('CX_ATTACK %s %s=%s' % (path, prop, obj.get_editor_property(prop)))
        except Exception:
            pass
