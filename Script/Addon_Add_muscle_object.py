import bpy
from bpy.types import Operator
from bpy.props import FloatVectorProperty, StringProperty, PointerProperty, CollectionProperty, EnumProperty
from bpy_extras.object_utils import AddObjectHelper, object_data_add
from mathutils import Vector

# import addon files
import sys
import os

if __name__=="__main__":
    dir = r"C:\Users\picol\Documents\Simuscle\Script"
    print(dir)
    print(sys.path)
    if not dir in sys.path:
        sys.path.append(dir )
        print(dir)


import Muscle_add
# from Muscle_add import AddMuscleOperator, create_muscle
if "bpy" in locals():
    import importlib
    importlib.reload(Muscle_add)


class OBJECT_OT_add_object(Operator, AddObjectHelper):
    """Create a Muscle Object"""
    # Some property you can set up when you create the Object 
    # (https://docs.blender.org/api/current/bpy.types.Property.html#bpy.types.Property)
    bl_idname = "muscle.add_muscle_2"
    bl_label = "Add Muscle Object"
    bl_options = {'REGISTER', 'UNDO'}

    muscle_name: StringProperty(
        name="Name",
        default="Muscle",
        description="Name of the muscle",
    )
    start_bone: EnumProperty(
        item=[("id", "choice_1", "descr", "BLENDER", 0), ("id1", "choice_2", "descr", "BLENDER", 1)],
        name="Test_obj_prop",
        description="The bone which the muscle is insert to",
    )

    def execute(self, context):

        Muscle_add.create_muscle(self.muscle_name)
        print(self.start_bone)

        return {'FINISHED'}


# Registration

def add_object_button(self, context):
    self.layout.operator(
        OBJECT_OT_add_object.bl_idname,
        text="Muscle",
        icon='AUTOMERGE_ON')


# This allows you to right click on a button and link to documentation
def add_object_manual_map():
    url_manual_prefix = "https://docs.blender.org/manual/en/latest/"
    url_manual_mapping = (
        ("bpy.ops.mesh.add_object", "scene_layout/object/types.html"),
    )
    return url_manual_prefix, url_manual_mapping


def register():
    Muscle_add.register()
    bpy.utils.register_class(OBJECT_OT_add_object)
    # bpy.utils.register_manual_map(add_object_manual_map)
    # bpy.types.VIEW3D_MT_add.append(add_object_button)


def unregister():
    Muscle_add.unregister()
    bpy.utils.unregister_class(OBJECT_OT_add_object)
    # bpy.utils.unregister_manual_map(add_object_manual_map)
    # bpy.types.VIEW3D_MT_mesh_add.remove(add_object_button)

if __name__ == "__main__":
    register()
