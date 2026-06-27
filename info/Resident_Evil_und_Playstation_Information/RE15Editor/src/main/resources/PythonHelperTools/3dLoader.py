import pyvista as pv
import sys

objectPath = sys.argv[1]
texturePath = sys.argv[2]

mesh = pv.read(objectPath)
tex = pv.read_texture(texturePath)

camera = pv.Camera()

#plotter = pv.Plotter(off_screen=True)
plotter = pv.Plotter()

#rotX = mesh.rotate_x(120, inplace=False)
rotX = mesh.rotate_x(90, inplace=True)
rotY = mesh.rotate_y(0, inplace=True)
rotZ = mesh.rotate_z(-45, inplace=True)

plotter.add_mesh(rotX)
plotter.add_mesh(rotY)
plotter.add_mesh(rotZ)
plotter.add_mesh(mesh, texture=tex)

#plotter.camera.position = (30.0,30.0,30.0)
plotter.camera.position = (30.0,30.0,30.0)
plotter.camera.zoom(9)

plotter.show(screenshot='leon.png')


##plt = Plotter(
##    N=4,
##    sharecam=False,
##    bg="test/tropical.jpg",
##    bg2='light blue',
##)
#
##mesh = load("test/PL00-Leon.OBJ").rotate_x(-90)
#
#mesh = load("test/PL00-Leon.OBJ")
#
#mesh.texture("test/PL00.bmp", scale=0.1)
#
#plt.at(2).show(__doc__)
#
## after first rendering, picture can be zoomed to fill the window:
#plt.background_renderer.GetActiveCamera().Zoom(1.8)
#plt.at(0).show(VedoLogo(distance=2))
#plt.at(3).show(mesh).screenshot("test/myPng.png")
#plt.interactive().close()