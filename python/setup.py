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
    author="MLM4LYFE",
    author_email="dsuo@cs.princeton.edu",
    description="Wrangling and learning from ChampSim output",
    python_requires=">=3.6",
    packages=find_packages()
)
