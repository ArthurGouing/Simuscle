import bpy
import os
from time import time

def export_simuscle(context, filepath, exp_skin, exp_muscles, exp_bones, exp_anim):
    print("Create directory : ", filepath)
    t = time()
    t0 = t
    os.system("rm -r "+filepath)
    os.mkdir(filepath)
    os.mkdir(filepath+"/Muscles")
    os.mkdir(filepath+"/Bones")
    os.mkdir(filepath+"/Skin")
    
    # Export Bones
    if exp_bones:
        bones = [obj for obj in bpy.data.collections["Skeleton"].objects if obj.parent is not None]
        print(bones)
        file = filepath+"/Bones/bones_info"
        bone_file = open(file + ".json", "w")
        bone_file.write("[\n")
        for i, bone in enumerate(bones):
            is_last = i ==(len(bones)-1)
            bone_file.write(bone_info(bone, is_last))
            print("Write "+bone.name, end=" ")
            path_file = filepath+"/Bones/"+bone.name
            export_mesh(path_file, bone)
            print(f"(written in {time()-t:3f} sec)")
            t = time()
        bone_file.close()
        t_bone = t
    
    # Export Muscles
    if exp_muscles:
        muscles = [obj for obj in bpy.data.collections["Muscles"].objects if obj.type=='MESH']
        for coll in bpy.data.collections["Muscles"].children_recursive:
            muscles += [obj for obj in coll.objects if obj.type=='MESH']
        file = filepath+"/Muscles/muscles_info"
        muscle_file = open(file + ".json", "w")
        muscle_file.write("[")
        for i, muscle in enumerate(muscles):
            is_last = i==(len(muscles)-1)
            muscle_file.write(muscle_info(muscle, is_last))
            print("Write "+muscle.name, end="")
            path_file = filepath+"/Muscles/"+muscle.name.replace(" ", "_")
            export_mesh(path_file, muscle)
            print(f"(written in {time()-t:.3f} sec)")
            t = time()
        muscle_file.write("\n]")
        muscle_file.close()
        t_muscle = t
    
    # Export Skin
    if exp_skin:
        skin = bpy.data.collections["Skin"].objects[0]
        path_file = filepath+"/Skin/skin"
        print("Write ", skin.name, end="")
        export_mesh(path_file, skin)
        print(f"(written in {time()-t:.3f} sec)")
        # TODO: write armature weight
        # And write simu mask
        t = time()
        t_skin = t
    
    # Export animation
    if exp_anim:
        print("Write ", skin.parent.name, end="")
        path_file = filepath+"/animation"
        export_anim(path_file, skin.parent)
        t = time()

    # Timer
    print(f"T_bone : {t_bone-t0:.3f} sec")
    print(f"T_muscle : {t_muscle-t_bone:.3f} sec")
    print(f"T_skin : {t_skin-t_muscle:.3f} sec")
    print(f"T_anim : {t-t_skin:.3f} sec")
    print(f"T_tot : {t-t0:.3f} sec")

    return {'FINISHED'}



def bone_info(bone, is_last):
    """ Return JSON formated string containing all bone info"""
    info  = '  {\n'
    info += f'    "name": "{bone.name}",\n'
    info += f'    "parent": "{bone.vertex_groups[0].name}"\n'
    # add origin info ???
    if is_last:
        info += '  }\n]'
    else:
        info += '  },\n'
    return info



def muscle_info(muscle, is_last):
    """ Return JSON formated string containing all muscle info (curve)"""
    start_target = bpy.data.objects.get("start_target_"+muscle.name)
    end_target = bpy.data.objects.get("end_target_"+muscle.name)
    print("\n"+muscle.name+" : ")
    print(start_target)
    print(end_target)
    curve = bpy.data.objects.get("curve_"+muscle.name)
    p0 = curve.data.splines[0].bezier_points[0].co
    p1 = curve.data.splines[0].bezier_points[0].handle_right
    p2 = curve.data.splines[0].bezier_points[1].handle_left
    p3 = curve.data.splines[0].bezier_points[1].co
    bezier_points = [p0, p1, p2, p3]
    info =   '\n  {\n'
    info += f'    "name": "{muscle.name}",\n'
    info += f'    "geometry": "{muscle.name.replace(" ", "_")}.off",\n'
    info += f'    "insertion_1": "{start_target.parent_bone}",\n'
    info += f'    "insertion_2": "{end_target.parent_bone}",\n'
    info += f'    "curve": ['
    for i, p in enumerate(bezier_points):
        info+= f' [ {p.x}, {p.y}, {p.z} ]'
        if i!=len(bezier_points)-1:
            info += ','
    info +=  f' ]\n'
    if is_last:
        info += '  }'
    else:
        info += '  },'
    return info



def export_mesh(file_name, obj):
    """ Export the mesh in OFF format """
    # Select object to export
   
    mesh = obj.data
    # Select vert and face
    vertices   = mesh.vertices[:]
    n_vertices = len(vertices)
    faces   = mesh.polygons[:]
    n_faces = len(faces)
    
    # Open the files
    file = file_name
    geom_file = open(file + ".off", "w")
    
    # Write Headers
    geom_file.write("OFF\n")
    geom_file.write(f"{n_vertices} {n_faces} 0\n")
    
    # Write all vertices
    for v in vertices:
        index = v.index
        # Geometry
        co = obj.matrix_world @ v.co
        loc_x = co[0]
        loc_y = co[1]
        loc_z = co[2]
        geom_file.write(f"{loc_x:.8f} {loc_y:.8f} {loc_z:.8f}\n")

    # Write all faces
    for f in faces:
        geom_file.write(f"{len(f.vertices)} ")
        # print([id for id in f.vertices])
        geom_file.write(" ".join([str(id) for id in f.vertices]))
        geom_file.write("\n")
    
    
    # Close files
    geom_file.close()

def export_anim(file_name, armature):
    # Open the files
    bpy.context.view_layer.objects.active = armature
    start_frame = bpy.context.scene.frame_start
    end_frame = bpy.context.scene.frame_end
    bpy.ops.export_anim.bvh(filepath=file_name + ".bvh",
                            frame_start=start_frame,
                            frame_end=end_frame)

# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator


class ExportSomeData(Operator, ExportHelper):
    """Export all the geometry and the animation to the Simuscle format"""
    bl_idname = "export_simuscle.all"  # important since its how bpy.ops.import_test.some_data is constructed
    bl_label = "Export for Simuscle"

    # ExportHelper mixin class uses this
    filename_ext = "_Simuscle"

    filter_glob: StringProperty(
        default="*",
        options={'HIDDEN'},
        maxlen=255,  # Max internal buffer length, longer would be clamped.
    )

    # List of operator properties, the attributes will be assigned
    # to the class instance from the operator settings before calling.
    exp_skin: BoolProperty(
        name="Skin",
        description="Example Tooltip",
        default=True,
    )
    exp_mucsles: BoolProperty(
        name="Muscles",
        description="Example Tooltip",
        default=True,
    )
    exp_bones: BoolProperty(
        name="Bones",
        description="Example Tooltip",
        default=True,
    )
    exp_anim: BoolProperty(
        name="Animation",
        description="Example Tooltip",
        default=True,
    )

    type: EnumProperty(
        name="Example Enum",
        description="Choose between two items",
        items=(
            ('OPT_A', "First Option", "Description one"),
            ('OPT_B', "Second Option", "Description two"),
        ),
        default='OPT_A',
    )

    def execute(self, context):
        return export_simuscle(context, self.filepath, self.exp_skin, self.exp_mucsles, self.exp_bones, self.exp_anim)


# Only needed if you want to add into a dynamic menu
def menu_func_export(self, context):
    self.layout.operator(ExportSomeData.bl_idname, text="Simuscle files (dir)")


# Register and add to the "file selector" menu (required to use F3 search "Text Export Operator" for quick access).
def register():
    bpy.utils.register_class(ExportSomeData)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)


def unregister():
    bpy.utils.unregister_class(ExportSomeData)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)


if __name__ == "__main__":
    filepath = "/home/arthos/Simuscle/Blender_scene/Test_2"
    # export_simuscle(bpy.context, filepath, "True")
    # unregister()
    register()

