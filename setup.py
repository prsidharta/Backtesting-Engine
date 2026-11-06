from pathlib import Path
import subprocess
import sys
from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext


class CMakeBuild(build_ext):
    def build_extension(self, ext):
        import pybind11

        output = Path(self.get_ext_fullpath(ext.name)).resolve().parent
        build = Path(self.build_temp).resolve()
        build.mkdir(parents=True, exist_ok=True)
        subprocess.check_call(
            [
                "cmake",
                "-S",
                str(Path(__file__).resolve().parent),
                "-B",
                str(build),
                "-DBUILD_CLI=OFF",
                "-DBUILD_PYTHON_MODULE=ON",
                "-DCMAKE_BUILD_TYPE=Release",
                f"-Dpybind11_DIR={pybind11.get_cmake_dir()}",
                f"-DPYTHON_EXECUTABLE={sys.executable}",
                f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={output}",
            ]
        )
        subprocess.check_call(
            ["cmake", "--build", str(build), "--config", "Release", "-j2"]
        )


setup(
    ext_modules=[Extension("backtest_env", sources=[])],
    cmdclass={"build_ext": CMakeBuild},
)
