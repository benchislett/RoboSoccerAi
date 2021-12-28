import sys

try:
    from skbuild import setup
except ImportError:
    print(
        "Please update pip, you need pip 10 or greater,\n",
        file=sys.stderr,
    )
    raise

from setuptools import find_packages

setup(
    name="robopy",
    version="0.0.1",
    description="a robo ai c++ library (with pybind11)",
    author="Benjamin Chislett",
    license="",
    packages=find_packages(where="src"),
    package_dir={"": "src"},
    cmake_install_dir="src/build",
    include_package_data=True,
    python_requires=">=3.6",
)
