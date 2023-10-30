bl_info = {
    "name": "Simuscle",
    "author": "Arthur Gouinguenet",
    "category": "3D View",
    "description": "The Blender Addon to create muscuol-skeleton compatible with Simuscle",
    "version": (1, 0),
    "blender": (3, 6, 0),
    "location": "View3D > Add > Mesh > New Object",
    "description": "Adds a Muscle Object",
    "warning": "",
    "doc_url": "",
    "category": "Add Mesh",
 }
# import all classes
if "bpy" in locals():
    import importlib
    importlib.reload(Addon_Add_muscle_object)
    importlib.reload(Menu_Simuscle)
    print("Reload files")
else:
    from . import Addon_Add_muscle_object
    from . import Menu_Simuscle
    print("Import multifiles")

import bpy

# classes = [OBJECT_OT_add_object, VIEW3D_MT_simuscle, VIEW3D_MT_editor_menus]
addon_keymaps = list()

def register():
  print ("Registering ", __name__)
  bpy.utils.register_class(Addon_Add_muscle_object.OBJECT_OT_add_object)
  bpy.utils.register_class(Menu_Simuscle.VIEW3D_MT_simuscle)
  bpy.utils.register_class(Menu_Simuscle.VIEW3D_MT_editor_menus)

  # Set the addon keymap
  wm = bpy.context.window_manager
  km = wm.keyconfigs.addon.keymaps.new(name='Object Mode', space_type='EMPTY')

  kmi = km.keymap_items.new('wm.call_menu', 'D', 'PRESS', ctrl=True, shift=False)
  kmi.properties.name = 'VIEW3D_MT_simuscle'

  addon_keymaps.append((km, kmi))

def unregister():
  print ("Unregistering ", __name__)
  # reset keymap
  for km, kmi in addon_keymaps:
      km.keymap_items.remove(kmi)
  addon_keymaps.clear()

  bpy.utils.unregister_class(Addon_Add_muscle_object.OBJECT_OT_add_object)
  bpy.utils.unregister_class(Menu_Simuscle.VIEW3D_MT_simuscle)
  bpy.utils.unregister_class(Menu_Simuscle.VIEW3D_MT_editor_menus)