from setuptools import find_packages, setup

setup(
    name="phasesim",
    version="0.0.1",
    install_requires=[
        "numpy",
        "scipy",
        "cython",
        "scikit-learn",
        "mlpack",
        "ipython",
        "flake8",
        "click",
        "clickutil",
        "tqdm",
        "pandas",
        "seaborn",
        "matplotlib",
        "sacred",
        "compiledb",
        "cloudpickle",
        "jupyter",
        "ipynb",
        "pixiedust"
    ],
    extras_require={
        "cuda": [
            "chainer",
            "cupy",
            "pytorch"
        ]
    },
    author="Daniel Suo",
    author_email="dsuo@cs.princeton.edu",
    description="PhaseSim",
    python_requires=">=3.6",
    packages=find_packages(),
    entry_points="""
    [console_scripts]
    run=phasesim.scripts.run:_run
    pin=phasesim.scripts.pin:_pin
    """
)
