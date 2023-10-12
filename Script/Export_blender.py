import bpy
from time import time

def error_window(err_msg):
    def oops(self, context):
        self.layout.label(text="You have done something you shouldn't do!")
    bpy.context.window_manager.popup_menu(oops, title="Error", icon='ERROR')

def export_anim(file_name, mesh_name):
    # Open the files
    # file_name = "test.bvh"
    file = file_name
    bpy.ops.export_anim.bvh(filepath=file + ".bvh",
                            frame_start=0,
                            frame_end=300)
    
def export_weight(file_name, mesh_name):
    """ Export the Weight in custom format """
    # Select object to export
    obj  = bpy.data.objects[mesh_name]
    # obj  = bpy.context.view_layer.objects.active.children[0]
    vertices = obj.to_mesh().vertices[:]
    Vgroups = obj.vertex_groups[:]
        
    # Open the files
    # file_name = "test"
    file = file_name
    weight_file = open(file + ".wgt", "w")
    
    # Write header
    weight_file.write(" ".join([g.name for g in Vgroups]))
    weight_file.write("\n")
    
    # Write all vertices
    for v in vertices:
        # Weight
        Weight = list()
        for g in Vgroups:
            try :
                w = g.weight(v.index)
            except RuntimeError: # If the werices isn't in the group
                w = 0
            Weight.append(f"{w:.8f}")
        weight_file.write(" ".join(Weight)+"\n")
        
    # Close files
    weight_file.close()


    
def export_mesh(file_name, mesh_name):
    """ Export the mesh in OFF format """
    # Select object to export
    obj  = bpy.data.objects[mesh_name]  # bpy.context.view_layer.objects.active.children[0]
        
    mesh = obj.to_mesh()
    print(obj.name)
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
        loc_x = v.co[0]
        loc_y = v.co[1]
        loc_z = v.co[2]
        geom_file.write(f"{loc_x:.8f} {loc_y:.8f} {loc_z:.8f}\n")

    # Write all faces
    for f in faces:
        geom_file.write(f"{len(f.vertices)} ")
        # print([id for id in f.vertices])
        geom_file.write(" ".join([str(id) for id in f.vertices]))
        geom_file.write("\n")
    
    
    # Close files
    geom_file.close()

    
print("---------------------------------------------\n")
mesh_name = "Anim_mesh"
path_file_name = "Human_anim"
print("Export " + mesh_name + " with .off format")


t = time()
# bpy.ops.object.select_all(action='DESELECT')
# bpy.data.objects[mesh_name].select_set(True)
# bpy.context.view_layer.objects.active = bpy.data.objects[mesh_name]
export_mesh(path_file_name, mesh_name)
print(f"The export mesh took {time()-t:.2f} s")
t = time()
export_weight(path_file_name, mesh_name)
print(f"The export weights took {time()-t:.2f} s")
t = time()
export_anim(path_file_name, mesh_name)
print(f"The export anim took {time()-t:.2f} s")
print("")
