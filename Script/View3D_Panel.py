import bpy
from bpy.types import Panel

class MusclePanel(Panel):
    """Creates a Panel where you have the Simuscle parameter in the View3D tab
    General parameters abour the muscle """
    bl_category = 'Simuscle'
    bl_label = "Muscle Info"
    bl_idname = "OBJECT_PT_muscle"
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'UI'

    def draw(self, context):
        layout = self.layout
        print("")
        print("Muscle pannel :")
        obj = context.object # En theorie inutile car obj =! du muscle object
        coll = obj.users_collection[0]

        layout.prop(coll, "muscle_name")
        layout.label(text="Muscle length: "+ str(2.235))


class InsertionPanel(bpy.types.Panel):
    """Creates a Panel where you have the Simuscle parameter in the View3D tab"""
    bl_category = 'Simuscle'
    bl_label = "Insertion"
    bl_idname = "OBJECT_PT_simuscle"
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'UI'
    # bl_context = "object"

    def draw(self, context):
        layout = self.layout
        obj = context.object
        coll = obj.users_collection[0]
        armature = None
        for obj in bpy.data.armatures:
            if obj.name == coll.armature:
                armature = obj

        layout.prop_search(coll, "armature", bpy.data, "armatures")

        if armature:
            layout.label(text="Bones insertion: ")
            layout.prop_search(coll, "start_bone", armature, "bones")
            layout.prop_search(coll, "end_bone"  , armature, "bones")

        # TODO: make a simuscle_type parameter for the object ??? 
        # check if it is a simucle composante before printing the pannel
        
        layout.separator()
        # Check if the muscle can be apply...
        layout.operator("simuscle.apply_muscle", icon="CHECKMARK")
        layout.operator("simuscle.delete_muscle", icon="PANEL_CLOSE")


def register():
    bpy.utils.register_class(MusclePanel)
    bpy.utils.register_class(InsertionPanel)


def unregister():
    bpy.utils.unregister_class(MusclePanel)
    bpy.utils.unregister_class(InsertionPanel)


if __name__ == "__main__":
    register()
