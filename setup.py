import os

from setuptools import setup, Extension
from distutils.command.build_ext import build_ext as build_ext_orig


class CTypesExtension(Extension): pass


class BuildExtension(build_ext_orig):

    def build_extension(self, ext):
        self._ctypes = isinstance(ext, CTypesExtension)
        return super().build_extension(ext)

    def get_export_symbols(self, ext):
        if self._ctypes:
            return ext.export_symbols
        return super().get_export_symbols(ext)

    def get_ext_filename(self, ext_name):
        if self._ctypes:
            return ext_name + '.so'
        return super().get_ext_filename(ext_name)


os.environ["CXX"] = "xlclang++"

setup(
    name="pyzfile",
    version="1.0.0",
    py_modules=["pyzfile.pyzfile"],
    ext_modules=[
        CTypesExtension(
            "pyzfile.libzfile",
            ["pyzfile/src/libzfile.cpp"],
            include_dirs=['nlohman-json/single_include'],
        )
    ],
    cmdclass={'build_ext': BuildExtension},
)
