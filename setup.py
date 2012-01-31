from distutils.core import setup, Extension
from os.path import join


sources = map(lambda x: join("src", x+".cpp"), (
	"CamacServer",
	"MamkinServer",
	"pycamac"
))


module1 = Extension(
	'pycamac',
	sources = sources,
	include_dirs = ["include", join("..", "..", "cheblakov")],
	libraries = ["boost_python"]
)

setup (name = 'pycamac',
		version = '0.1',
		description = 'Python CAMAC API',
		ext_modules = [module1])

