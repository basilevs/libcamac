from distutils.core import setup, Extension
from os.path import join


sources = map(lambda x: join("src", x+".cpp"), (
	"CamacServer",
	"MamkinServer",
	"pycamac",
	"lecroy2249"
))


module1 = Extension(
	'pycamac',
	sources = sources,
	include_dirs = ["include", join("..", "..", "cheblakov")],
	libraries = ["boost_python"]
)

setup (name = 'pycamac',
		version = '0.2',
		description = 'Python CAMAC API',
		ext_modules = [module1])

